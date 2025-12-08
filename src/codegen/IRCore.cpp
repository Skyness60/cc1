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

// ============================================================================
// Main Generation Entry Point
// ============================================================================

void IRGenerator::generate(AST::TranslationUnit& unit) {
    currentBuffer_ = &globalBuffer_;
    unit.accept(*this);
}

std::string IRGenerator::getIR() const {
    // Build metadata for PIE (Position Independent Executable)
    std::stringstream meta;
    meta << "!llvm.module.flags = !{!0, !1}\n";
    meta << "!0 = !{i32 8, !\"PIC Level\", i32 2}\n";
    meta << "!1 = !{i32 7, !\"PIE Level\", i32 2}\n";
    
    // Build function declarations (only for functions not defined)
    std::stringstream decls;
    for (const auto& pair : functionDeclarations_) {
        if (!definedFunctions_.count(pair.first)) {
            decls << pair.second;
        }
    }
    
    // Order: header, globals, string literals, function declarations, function definitions, metadata
    return headerBuffer_.str() + globalBuffer_.str() + stringBuffer_.str() + 
           decls.str() + funcDefBuffer_.str() + meta.str();
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
        *currentBuffer_ << "  " << ir << "\n";
    }
}

void IRGenerator::emitLabel(const std::string& label) {
    if (currentBuffer_) {
        *currentBuffer_ << label << ":\n";
    }
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
    if (!val.isPointer || val.isConstant) {
        return val;
    }
    
    std::string temp = newTemp();
    std::string loadType = val.derefType();
    
    // Emit: %temp = load type, type* ptr
    if (currentBuffer_) {
        *currentBuffer_ << "  " << temp << " = load " << loadType 
                        << ", " << val.type << " " << val.name << "\n";
    }
    
    IRValue result;
    result.name = temp;
    result.type = loadType;
    result.isPointer = (loadType.size() > 0 && loadType.back() == '*');
    result.isConstant = false;
    
    return result;
}

IRValue IRGenerator::storeValue(const IRValue& val, const IRValue& ptr) {
    std::string valueType = val.type;
    std::string ptrType = ptr.type;
    
    emit("store " + valueType + " " + val.name + ", " + ptrType + " " + ptr.name);
    
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

} // namespace cc1
