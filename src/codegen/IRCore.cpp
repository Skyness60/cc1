#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Constructor
// ============================================================================

IRGenerator::IRGenerator(bool is64bit) : is64bit_(is64bit) {
    // Initialize with global scope
    enterScope();
    
    // Set up target triple and data layout
    if (is64bit_) {
        headerBuffer_ << "; ModuleID = 'cc1'\n";
        headerBuffer_ << "source_filename = \"cc1\"\n";
        headerBuffer_ << "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n";
        headerBuffer_ << "target triple = \"x86_64-pc-linux-gnu\"\n\n";
    } else {
        headerBuffer_ << "; ModuleID = 'cc1'\n";
        headerBuffer_ << "source_filename = \"cc1\"\n";
        headerBuffer_ << "target datalayout = \"e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128\"\n";
        headerBuffer_ << "target triple = \"i386-pc-linux-gnu\"\n\n";
    }
}

void IRGenerator::setDebugInfo(bool enabled, const std::string& primaryFilename) {
    debugInfo_ = enabled;
    debugFilename_ = primaryFilename;

    // Split directory/filename for DIFile.
    auto pos = debugFilename_.find_last_of('/');
    if (pos == std::string::npos) {
        debugDirectory_.clear();
    } else {
        debugDirectory_ = debugFilename_.substr(0, pos);
        debugFilename_ = debugFilename_.substr(pos + 1);
    }
}

void IRGenerator::initDebugMetadataIfNeeded() {
    if (!debugInfo_) return;
    if (diFileId_ != -1 && diCompileUnitId_ != -1) return;

    diFileId_ = newDebugMetaId();
    // Best-effort: if directory is empty, use ".".
    std::string dir = debugDirectory_.empty() ? "." : debugDirectory_;
    debugMetaBuffer_ << "!" << diFileId_ << " = !DIFile(filename: \"" << debugFilename_ << "\", directory: \"" << dir << "\")\n";

    diCompileUnitId_ = newDebugMetaId();
    debugMetaBuffer_ << "!" << diCompileUnitId_
                     << " = distinct !DICompileUnit(language: DW_LANG_C89, file: !" << diFileId_
                     << ", producer: \"cc1\", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug)\n";
}

int IRGenerator::newDebugMetaId() {
    return nextDebugMetaId_++;
}

int IRGenerator::getOrCreateDILocationId(int line, int col, int scopeId) {
    auto key = std::make_tuple(line, col, scopeId);
    auto it = diLocationIds_.find(key);
    if (it != diLocationIds_.end()) return it->second;

    int id = newDebugMetaId();
    diLocationIds_[key] = id;
    debugMetaBuffer_ << "!" << id << " = !DILocation(line: " << line << ", column: " << col
                     << ", scope: !" << scopeId << ")\n";
    return id;
}

void IRGenerator::pushDebugLoc(int line, int col) {
    DebugLoc loc;
    loc.line = line;
    loc.column = col;
    debugLocStack_.push_back(loc);
}

void IRGenerator::popDebugLoc() {
    if (!debugLocStack_.empty()) debugLocStack_.pop_back();
}

std::string IRGenerator::dbgSuffixForCurrentLoc() {
    if (!debugInfo_) return "";
    if (currentSubprogramId_ < 0) return "";
    if (debugLocStack_.empty()) return "";

    const DebugLoc& loc = debugLocStack_.back();
    int line = loc.line > 0 ? loc.line : 1;
    int col = loc.column > 0 ? loc.column : 1;
    int locId = getOrCreateDILocationId(line, col, currentSubprogramId_);
    return ", !dbg !" + std::to_string(locId);
}

// ============================================================================
// Main Generation Entry Point
// ============================================================================

void IRGenerator::generate(AST::TranslationUnit& unit) {
    currentBuffer_ = &globalBuffer_;
    unit.accept(*this);
}

std::string IRGenerator::getIR() const {
    // Build struct type definitions
    std::stringstream structs;
    for (const auto& pair : namedStructDefs_) {
        if (!pair.second.first.empty()) {
            structs << pair.second.first << "\n";
        }
    }
    
    // Build metadata for PIE (Position Independent Executable) and (optionally) debug info.
    std::stringstream meta;
    meta << "!llvm.module.flags = !{";
    if (debugInfo_) {
        meta << "!0, !1, !2, !3";
    } else {
        meta << "!0, !1";
    }
    meta << "}\n";
    meta << "!0 = !{i32 8, !\"PIC Level\", i32 2}\n";
    meta << "!1 = !{i32 7, !\"PIE Level\", i32 2}\n";
    if (debugInfo_) {
        meta << "!2 = !{i32 2, !\"Dwarf Version\", i32 4}\n";
        meta << "!3 = !{i32 2, !\"Debug Info Version\", i32 3}\n";
        if (diCompileUnitId_ >= 0) {
            meta << "!llvm.dbg.cu = !{!" << diCompileUnitId_ << "}\n";
        }
    }
    
    // Build function declarations (only for functions not defined)
    std::stringstream decls;
    for (const auto& pair : functionDeclarations_) {
        if (!definedFunctions_.count(pair.first)) {
            decls << pair.second;
        }
    }
    
        // Order: header, struct type definitions, globals, string literals, function declarations, function definitions, debug metadata, module metadata
        return headerBuffer_.str() + structs.str() + globalBuffer_.str() + stringBuffer_.str() +
            decls.str() + funcDefBuffer_.str() + debugMetaBuffer_.str() + meta.str();
}

// ============================================================================
// Temporary and Label Generation
// ============================================================================

std::string IRGenerator::newTemp() {
    return "%" + std::to_string(tempCounter_++);
}

std::string IRGenerator::newLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter_++);
}

std::string IRGenerator::newGlobalString(const std::string& str) {
    std::string name = "@.str." + std::to_string(stringCounter_++);
    
    // Escape the string for LLVM IR
    std::string escaped;
    for (char c : str) {
        if (c == '\\') {
            escaped += "\\5C";
        } else if (c == '"') {
            escaped += "\\22";
        } else if (c == '\n') {
            escaped += "\\0A";
        } else if (c == '\t') {
            escaped += "\\09";
        } else if (c == '\r') {
            escaped += "\\0D";
        } else if (c == '\0') {
            escaped += "\\00";
        } else if (c < 32 || c > 126) {
            // Non-printable: emit as hex
            char hex[5];
            snprintf(hex, sizeof(hex), "\\%02X", static_cast<unsigned char>(c));
            escaped += hex;
        } else {
            escaped += c;
        }
    }
    
    // Add null terminator
    escaped += "\\00";
    
    // Emit global string constant to string buffer
    stringBuffer_ << name << " = private unnamed_addr constant ["
                  << (str.length() + 1) << " x i8] c\"" << escaped << "\"\n";
    
    return name;
}

// ============================================================================
// IR Emission
// ============================================================================

void IRGenerator::emit(const std::string& ir) {
    if (currentBuffer_) {
        *currentBuffer_ << "  " << ir;
        if (currentBuffer_ == &functionBuffer_) {
            *currentBuffer_ << dbgSuffixForCurrentLoc();
        }
        *currentBuffer_ << "\n";
    }

    // Best-effort tracking of terminator instructions.
    // This generator emits labels explicitly after terminators for dead code paths;
    // tracking helps avoid producing invalid IR when introducing new labels.
    if (ir.rfind("br ", 0) == 0 || ir.rfind("ret ", 0) == 0 || ir.rfind("switch ", 0) == 0 || ir.rfind("unreachable", 0) == 0) {
        blockTerminated_ = true;
    }
}

void IRGenerator::emitLabel(const std::string& label) {
    if (currentBuffer_) {
        *currentBuffer_ << label << ":\n";
    }

    // A label starts a new basic block.
    blockTerminated_ = false;
}

void IRGenerator::emitComment(const std::string& comment) {
    if (currentBuffer_) {
        *currentBuffer_ << "  ; " << comment << "\n";
    }
}

void IRGenerator::emitGlobal(const std::string& ir) {
    globalBuffer_ << ir << "\n";
}

// ============================================================================
// Value Management
// ============================================================================

IRValue IRGenerator::loadValue(const IRValue& val) {
    if (val.isBitfieldRef) {
        // Load and extract the bitfield value from its storage unit.
        std::string storagePtr = val.name;
        std::string storageTy = val.bitfieldStorageType.empty() ? val.derefType() : val.bitfieldStorageType;

        std::string loaded = newTemp();
        emit(loaded + " = load " + storageTy + ", " + storageTy + "* " + storagePtr);

        std::string shifted = loaded;
        if (val.bitfieldOffset != 0) {
            shifted = newTemp();
            emit(shifted + " = lshr " + storageTy + " " + loaded + ", " + std::to_string(val.bitfieldOffset));
        }

        long long mask = 0;
        if (val.bitfieldWidth >= 64) {
            mask = -1LL;
        } else if (val.bitfieldWidth <= 0) {
            mask = 0;
        } else {
            mask = (1LL << val.bitfieldWidth) - 1;
        }

        std::string masked = newTemp();
        emit(masked + " = and " + storageTy + " " + shifted + ", " + std::to_string(mask));

        // Normalize to i32 as the expression value.
        std::string asI32;
        if (storageTy == "i32") {
            asI32 = masked;
        } else {
            asI32 = newTemp();
            if (storageTy == "i64") {
                emit(asI32 + " = trunc i64 " + masked + " to i32");
            } else {
                emit(asI32 + " = zext " + storageTy + " " + masked + " to i32");
            }
        }

        if (!val.bitfieldIsUnsigned && val.bitfieldWidth > 0 && val.bitfieldWidth < 32) {
            int sh = 32 - val.bitfieldWidth;
            std::string shl = newTemp();
            emit(shl + " = shl i32 " + asI32 + ", " + std::to_string(sh));
            std::string ashr = newTemp();
            emit(ashr + " = ashr i32 " + shl + ", " + std::to_string(sh));
            return IRValue(ashr, "i32", false, false);
        }

        return IRValue(asI32, "i32", false, false);
    }

    if (!val.isPointer || val.isConstant) {
        return val;
    }
    
    std::string temp = newTemp();
    std::string loadType = val.derefType();
    
    // Emit: %temp = load type, type* ptr
    emit(temp + " = load " + loadType + ", " + val.type + " " + val.name);
    
    IRValue result;
    result.name = temp;
    result.type = loadType;
    result.isPointer = (loadType.size() > 0 && loadType.back() == '*');
    result.isConstant = false;
    
    return result;
}

IRValue IRGenerator::storeValue(const IRValue& val, const IRValue& ptr) {
    if (ptr.isBitfieldRef) {
        // Read-modify-write of the integer storage unit.
        std::string storagePtr = ptr.name;
        std::string storageTy = ptr.bitfieldStorageType.empty() ? ptr.derefType() : ptr.bitfieldStorageType;

        std::string oldV = newTemp();
        emit(oldV + " = load " + storageTy + ", " + storageTy + "* " + storagePtr);

        // Cast RHS to i32 if needed.
        std::string rhsReg = val.name;
        std::string rhsTy = val.type;
        if (val.isPointer && !val.isConstant) {
            rhsReg = newTemp();
            rhsTy = val.derefType();
            emit(rhsReg + " = load " + rhsTy + ", " + val.type + " " + val.name);
        }
        if (rhsTy != "i32") {
            std::string tmp = newTemp();
            if (!rhsTy.empty() && rhsTy.back() == '*') {
                // Defensive: pointers to bitfields shouldn't happen.
                emit(tmp + " = ptrtoint " + rhsTy + " " + rhsReg + " to i32");
            } else {
                emit(tmp + " = trunc " + rhsTy + " " + rhsReg + " to i32");
            }
            rhsReg = tmp;
            rhsTy = "i32";
        }

        // Cast i32 -> storageTy.
        std::string rhsStorage = newTemp();
        if (storageTy == "i64") {
            emit(rhsStorage + " = zext i32 " + rhsReg + " to i64");
        } else if (storageTy == "i32") {
            rhsStorage = rhsReg;
        } else {
            emit(rhsStorage + " = trunc i32 " + rhsReg + " to " + storageTy);
        }

        long long lowMask = 0;
        if (ptr.bitfieldWidth >= 64) {
            lowMask = -1LL;
        } else if (ptr.bitfieldWidth <= 0) {
            lowMask = 0;
        } else {
            lowMask = (1LL << ptr.bitfieldWidth) - 1;
        }
        std::string masked = newTemp();
        emit(masked + " = and " + storageTy + " " + rhsStorage + ", " + std::to_string(lowMask));

        std::string shifted = masked;
        if (ptr.bitfieldOffset != 0) {
            shifted = newTemp();
            emit(shifted + " = shl " + storageTy + " " + masked + ", " + std::to_string(ptr.bitfieldOffset));
        }

        // Clear target bits then OR in new bits.
        long long fieldMask = 0;
        if (ptr.bitfieldWidth >= 64) {
            fieldMask = -1LL;
        } else if (ptr.bitfieldWidth <= 0) {
            fieldMask = 0;
        } else {
            fieldMask = ((1LL << ptr.bitfieldWidth) - 1) << ptr.bitfieldOffset;
        }
        std::string cleared = newTemp();
        emit(cleared + " = and " + storageTy + " " + oldV + ", " + std::to_string(~fieldMask));

        std::string newV = newTemp();
        emit(newV + " = or " + storageTy + " " + cleared + ", " + shifted);
        emit("store " + storageTy + " " + newV + ", " + storageTy + "* " + storagePtr);

        return val;
    }

    std::string valueType = val.type;
    std::string ptrType = ptr.type;

    // Ensure the stored value matches the pointee type.
    // Without this, assignments like `char c = getchar();` produce invalid IR
    // (`store i32, i8*`) and corrupt the stack at runtime.
    std::string destType = ptr.derefType();
    std::string srcReg = val.name;
    std::string srcType = valueType;

    if (srcType != destType) {
        auto intBits = [](const std::string& t) -> int {
            if (t == "i1") return 1;
            if (t == "i8") return 8;
            if (t == "i16") return 16;
            if (t == "i32") return 32;
            if (t == "i64") return 64;
            return 0;
        };

        int srcInt = intBits(srcType);
        int dstInt = intBits(destType);

        // Integer <-> integer
        if (srcInt > 0 && dstInt > 0) {
            std::string casted = newTemp();
            if (dstInt < srcInt) {
                emit(casted + " = trunc " + srcType + " " + srcReg + " to " + destType);
            } else if (dstInt > srcInt) {
                // Default to sign-extension; unsigned correctness is generally handled earlier
                // via usual arithmetic conversions.
                emit(casted + " = sext " + srcType + " " + srcReg + " to " + destType);
            } else {
                emit(casted + " = bitcast " + srcType + " " + srcReg + " to " + destType);
            }
            srcReg = casted;
            srcType = destType;
        }
        // Pointer <-> pointer
        else if (!srcType.empty() && srcType.back() == '*' && !destType.empty() && destType.back() == '*') {
            std::string casted = newTemp();
            emit(casted + " = bitcast " + srcType + " " + srcReg + " to " + destType);
            srcReg = casted;
            srcType = destType;
        }
        // Integer -> pointer
        else if (srcInt > 0 && !destType.empty() && destType.back() == '*') {
            std::string casted = newTemp();
            emit(casted + " = inttoptr " + srcType + " " + srcReg + " to " + destType);
            srcReg = casted;
            srcType = destType;
        }
        // Pointer -> integer
        else if (!srcType.empty() && srcType.back() == '*' && dstInt > 0) {
            std::string casted = newTemp();
            emit(casted + " = ptrtoint " + srcType + " " + srcReg + " to " + destType);
            srcReg = casted;
            srcType = destType;
        }
        // Float widening/narrowing
        else if ((srcType == "float" || srcType == "double") && (destType == "float" || destType == "double")) {
            std::string casted = newTemp();
            if (srcType == "float" && destType == "double") {
                emit(casted + " = fpext float " + srcReg + " to double");
            } else if (srcType == "double" && destType == "float") {
                emit(casted + " = fptrunc double " + srcReg + " to float");
            } else {
                emit(casted + " = bitcast " + srcType + " " + srcReg + " to " + destType);
            }
            srcReg = casted;
            srcType = destType;
        }
        // Otherwise: fall through and attempt the store as-is.
    }

    emit("store " + srcType + " " + srcReg + ", " + ptrType + " " + ptr.name);
    
    return val;
}

// ============================================================================
// Symbol Table Management
// ============================================================================

void IRGenerator::enterScope() {
    scopes_.push_back({});
}

void IRGenerator::exitScope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

IRSymbol* IRGenerator::lookupSymbol(const std::string& name) {
    // Search from innermost to outermost scope
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return &found->second;
        }
    }
    return nullptr;
}

void IRGenerator::defineSymbol(const std::string& name, const IRSymbol& sym) {
    if (!scopes_.empty()) {
        scopes_.back()[name] = sym;
    }
}

// ============================================================================
// Constant Expression Evaluation
// ============================================================================

bool IRGenerator::evaluateConstantExpr(AST::Expression* expr, long long& result) {
    if (!expr) return false;
    
    if (auto* lit = dynamic_cast<AST::IntegerLiteral*>(expr)) {
        result = lit->value;
        return true;
    }
    
    if (auto* lit = dynamic_cast<AST::CharLiteral*>(expr)) {
        result = lit->value;
        return true;
    }
    
    if (auto* id = dynamic_cast<AST::Identifier*>(expr)) {
        auto it = enumValues_.find(id->name);
        if (it != enumValues_.end()) {
            result = it->second;
            return true;
        }
        return false;
    }
    
    if (auto* unary = dynamic_cast<AST::UnaryExpr*>(expr)) {
        long long operand;
        if (!evaluateConstantExpr(unary->operand.get(), operand)) return false;
        
        switch (unary->op) {
            case AST::UnaryOp::Plus: result = operand; return true;
            case AST::UnaryOp::Negate: result = -operand; return true;
            case AST::UnaryOp::BitwiseNot: result = ~operand; return true;
            case AST::UnaryOp::LogicalNot: result = !operand ? 1 : 0; return true;
            default: return false;
        }
    }
    
    if (auto* bin = dynamic_cast<AST::BinaryExpr*>(expr)) {
        long long left, right;
        if (!evaluateConstantExpr(bin->left.get(), left)) return false;
        if (!evaluateConstantExpr(bin->right.get(), right)) return false;
        
        switch (bin->op) {
            case AST::BinaryOp::Add: result = left + right; return true;
            case AST::BinaryOp::Sub: result = left - right; return true;
            case AST::BinaryOp::Mul: result = left * right; return true;
            case AST::BinaryOp::Div:
                if (right == 0) return false;
                result = left / right;
                return true;
            case AST::BinaryOp::Mod:
                if (right == 0) return false;
                result = left % right;
                return true;
            case AST::BinaryOp::BitwiseAnd: result = left & right; return true;
            case AST::BinaryOp::BitwiseOr: result = left | right; return true;
            case AST::BinaryOp::BitwiseXor: result = left ^ right; return true;
            case AST::BinaryOp::LeftShift: result = left << right; return true;
            case AST::BinaryOp::RightShift: result = left >> right; return true;
            case AST::BinaryOp::Equal: result = (left == right) ? 1 : 0; return true;
            case AST::BinaryOp::NotEqual: result = (left != right) ? 1 : 0; return true;
            case AST::BinaryOp::Less: result = (left < right) ? 1 : 0; return true;
            case AST::BinaryOp::LessEqual: result = (left <= right) ? 1 : 0; return true;
            case AST::BinaryOp::Greater: result = (left > right) ? 1 : 0; return true;
            case AST::BinaryOp::GreaterEqual: result = (left >= right) ? 1 : 0; return true;
            case AST::BinaryOp::LogicalAnd: result = (left && right) ? 1 : 0; return true;
            case AST::BinaryOp::LogicalOr: result = (left || right) ? 1 : 0; return true;
            default: return false;
        }
    }
    
    if (auto* tern = dynamic_cast<AST::TernaryExpr*>(expr)) {
        long long cond;
        if (!evaluateConstantExpr(tern->condition.get(), cond)) return false;
        if (cond) {
            return evaluateConstantExpr(tern->thenExpr.get(), result);
        } else {
            return evaluateConstantExpr(tern->elseExpr.get(), result);
        }
    }
    
    if (auto* sizeofExpr = dynamic_cast<AST::SizeofExpr*>(expr)) {
        if (sizeofExpr->targetType) {
            result = getTypeSize(sizeofExpr->targetType.get());
            return true;
        }
        // sizeof(expr) - would need type inference
        return false;
    }
    
    return false;
}

bool IRGenerator::evaluateConstantFloatExpr(AST::Expression* expr, double& result) {
    if (!expr) return false;

    if (auto* lit = dynamic_cast<AST::FloatLiteral*>(expr)) {
        result = lit->value;
        return true;
    }

    if (auto* lit = dynamic_cast<AST::IntegerLiteral*>(expr)) {
        result = static_cast<double>(lit->value);
        return true;
    }

    if (auto* lit = dynamic_cast<AST::CharLiteral*>(expr)) {
        result = static_cast<double>(lit->value);
        return true;
    }

    if (auto* id = dynamic_cast<AST::Identifier*>(expr)) {
        auto it = enumValues_.find(id->name);
        if (it != enumValues_.end()) {
            result = static_cast<double>(it->second);
            return true;
        }
        return false;
    }

    if (auto* unary = dynamic_cast<AST::UnaryExpr*>(expr)) {
        double operand;
        if (!evaluateConstantFloatExpr(unary->operand.get(), operand)) {
            long long intOperand;
            if (!evaluateConstantExpr(unary->operand.get(), intOperand)) return false;
            operand = static_cast<double>(intOperand);
        }

        switch (unary->op) {
            case AST::UnaryOp::Plus:
                result = operand;
                return true;
            case AST::UnaryOp::Negate:
                result = -operand;
                return true;
            case AST::UnaryOp::LogicalNot:
                result = (operand == 0.0) ? 1.0 : 0.0;
                return true;
            default:
                return false;
        }
    }

    if (auto* bin = dynamic_cast<AST::BinaryExpr*>(expr)) {
        double left, right;
        if (!evaluateConstantFloatExpr(bin->left.get(), left)) {
            long long intLeft;
            if (!evaluateConstantExpr(bin->left.get(), intLeft)) return false;
            left = static_cast<double>(intLeft);
        }
        if (!evaluateConstantFloatExpr(bin->right.get(), right)) {
            long long intRight;
            if (!evaluateConstantExpr(bin->right.get(), intRight)) return false;
            right = static_cast<double>(intRight);
        }

        switch (bin->op) {
            case AST::BinaryOp::Add:
                result = left + right;
                return true;
            case AST::BinaryOp::Sub:
                result = left - right;
                return true;
            case AST::BinaryOp::Mul:
                result = left * right;
                return true;
            case AST::BinaryOp::Div:
                if (right == 0.0) return false;
                result = left / right;
                return true;
            case AST::BinaryOp::Equal:
                result = (left == right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::NotEqual:
                result = (left != right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::Less:
                result = (left < right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::LessEqual:
                result = (left <= right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::Greater:
                result = (left > right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::GreaterEqual:
                result = (left >= right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::LogicalAnd:
                result = (left != 0.0 && right != 0.0) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::LogicalOr:
                result = (left != 0.0 || right != 0.0) ? 1.0 : 0.0;
                return true;
            default:
                return false;
        }
    }

    if (auto* tern = dynamic_cast<AST::TernaryExpr*>(expr)) {
        double cond;
        if (!evaluateConstantFloatExpr(tern->condition.get(), cond)) {
            long long intCond;
            if (!evaluateConstantExpr(tern->condition.get(), intCond)) return false;
            cond = static_cast<double>(intCond);
        }
        if (cond != 0.0) {
            return evaluateConstantFloatExpr(tern->thenExpr.get(), result);
        }
        return evaluateConstantFloatExpr(tern->elseExpr.get(), result);
    }

    if (auto* sizeofExpr = dynamic_cast<AST::SizeofExpr*>(expr)) {
        if (sizeofExpr->targetType) {
            result = static_cast<double>(getTypeSize(sizeofExpr->targetType.get()));
            return true;
        }
        return false;
    }

    if (auto* castExpr = dynamic_cast<AST::CastExpr*>(expr)) {
        double operand;
        if (!evaluateConstantFloatExpr(castExpr->operand.get(), operand)) {
            long long intOperand;
            if (!evaluateConstantExpr(castExpr->operand.get(), intOperand)) return false;
            operand = static_cast<double>(intOperand);
        }

        AST::Type* target = stripQualifiers(castExpr->targetType.get());
        if (auto* prim = dynamic_cast<AST::PrimitiveType*>(target)) {
            switch (prim->kind) {
                case AST::PrimitiveKind::Float:
                    result = static_cast<double>(static_cast<float>(operand));
                    return true;
                case AST::PrimitiveKind::Double:
                    result = operand;
                    return true;
                case AST::PrimitiveKind::LongDouble:
                    result = operand;
                    return true;
                default:
                    break;
            }
        }
        return false;
    }

    return false;
}

} // namespace cc1
