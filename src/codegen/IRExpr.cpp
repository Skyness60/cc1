#include <codegen/IRGenerator.hpp>

#include <cctype>

namespace cc1 {

static inline std::string trimCopy(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(start, end - start);
}

static inline bool startsWith(const std::string& s, const std::string& prefix) {
    return s.rfind(prefix, 0) == 0;
}

static inline bool isAggregateLLVMType(const std::string& t) {
    return !t.empty() && (t[0] == '[' || t[0] == '{' || startsWith(t, "%struct.") || startsWith(t, "%union."));
}

static inline bool isUnsignedIntegralResolvedType(const AST::Expression& expr) {
    if (!expr.resolvedType) return false;
    // Only handle primitive unsigneds; everything else defaults to signed.
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(expr.resolvedType.get())) {
        switch (prim->kind) {
            case AST::PrimitiveKind::UnsignedChar:
            case AST::PrimitiveKind::UnsignedShort:
            case AST::PrimitiveKind::UnsignedInt:
            case AST::PrimitiveKind::UnsignedLong:
            case AST::PrimitiveKind::UnsignedLongLong:
                return true;
            default:
                return false;
        }
    }
    return false;
}

static inline bool shouldLoadCallArgument(const IRValue& v) {
    if (!v.isPointer || v.isConstant) return false;
    std::string dt = v.derefType();
    // Do not load aggregates (arrays/structs/unions). Keep pointer so it decays/passes by reference.
    if (isAggregateLLVMType(dt)) return false;
    return true;
}

static inline bool parseFunctionDeclParamTypes(const std::string& declLine,
                                               std::vector<std::string>& outParamTypes,
                                               bool& outIsVariadic) {
    outParamTypes.clear();
    outIsVariadic = false;
    auto l = declLine.find('(');
    auto r = declLine.rfind(')');
    if (l == std::string::npos || r == std::string::npos || r <= l) return false;
    std::string inside = declLine.substr(l + 1, r - l - 1);
    inside = trimCopy(inside);
    if (inside.empty()) return true;

    size_t pos = 0;
    while (pos < inside.size()) {
        size_t comma = inside.find(',', pos);
        std::string tok = (comma == std::string::npos) ? inside.substr(pos) : inside.substr(pos, comma - pos);
        tok = trimCopy(tok);
        if (tok == "...") {
            outIsVariadic = true;
        } else if (!tok.empty()) {
            outParamTypes.push_back(tok);
        }
        if (comma == std::string::npos) break;
        pos = comma + 1;
    }
    return true;
}

// ============================================================================
// Integer Literal
// ============================================================================

void IRGenerator::visit(AST::IntegerLiteral& node) {
    lastValue_ = IRValue(std::to_string(node.value), "i32", false, true);
}

// ============================================================================
// Float Literal
// ============================================================================

void IRGenerator::visit(AST::FloatLiteral& node) {
    // LLVM uses hex representation for float constants
    lastValue_ = IRValue(std::to_string(node.value), "double", false, true);
}

// ============================================================================
// Char Literal
// ============================================================================

void IRGenerator::visit(AST::CharLiteral& node) {
    lastValue_ = IRValue(std::to_string(static_cast<int>(node.value)), "i8", false, true);
}

// ============================================================================
// String Literal
// ============================================================================

void IRGenerator::visit(AST::StringLiteral& node) {
    std::string globalName = newGlobalString(node.value);
    size_t len = node.value.length() + 1;
    
    // Get pointer to first element
    std::string temp = newTemp();
    emit(temp + " = getelementptr inbounds [" + std::to_string(len) + " x i8], [" + 
         std::to_string(len) + " x i8]* " + globalName + ", i32 0, i32 0");
    
    lastValue_ = IRValue(temp, "i8*", false, false);
}

// ============================================================================
// Identifier
// ============================================================================

void IRGenerator::visit(AST::Identifier& node) {
    // Check for enum constant first
    auto enumIt = enumValues_.find(node.name);
    if (enumIt != enumValues_.end()) {
        lastValue_ = IRValue(std::to_string(enumIt->second), "i32", false, true);
        return;
    }
    
    // Look up in symbol table
    IRSymbol* sym = lookupSymbol(node.name);
    if (!sym) {
        hadError_ = true;
        lastValue_ = IRValue("0", "i32", false, true);
        return;
    }
    
    if (sym->isFunction) {
        lastValue_ = IRValue(sym->irName, sym->type + "*", false, false);
    } else {
        // Variable - return pointer to it (lvalue)
        lastValue_ = IRValue(sym->irName, sym->type + "*", true, false);
    }
}

// ============================================================================
// Binary Expression
// ============================================================================

void IRGenerator::visit(AST::BinaryExpr& node) {
    // Handle assignment specially
    if (node.op == AST::BinaryOp::Assign) {
        // Evaluate RHS first
        node.right->accept(*this);
        IRValue rhsVal = loadValue(lastValue_);
        
        // Evaluate LHS to get address
        node.left->accept(*this);
        IRValue lhsVal = lastValue_;
        
        // Store (handles bitfields too)
        storeValue(rhsVal, lhsVal);

        // Result is the assigned value
        lastValue_ = rhsVal;
        return;
    }
    
    // Handle compound assignment
    if (node.op >= AST::BinaryOp::AddAssign && node.op <= AST::BinaryOp::RightShiftAssign) {
        // Get LHS address
        node.left->accept(*this);
        IRValue lhsPtr = lastValue_;

        // Load current LHS value (handles bitfields)
        IRValue lhsLoaded = loadValue(lhsPtr);
        std::string lhsReg = lhsLoaded.name;
        std::string lhsType = lhsLoaded.type;
        
        // Evaluate RHS
        node.right->accept(*this);
        IRValue rhsVal = loadValue(lastValue_);
        std::string rhsReg = rhsVal.name;

        // Keep RHS type compatible with LHS type.
        if (rhsVal.type != lhsType) {
            auto getIntSize = [](const std::string& t) -> int {
                if (t == "i8") return 8;
                if (t == "i16") return 16;
                if (t == "i32") return 32;
                if (t == "i64") return 64;
                return 0;
            };
            int rhsSize = getIntSize(rhsVal.type);
            int lhsSize = getIntSize(lhsType);
            if (rhsSize > 0 && lhsSize > 0 && rhsSize != lhsSize) {
                std::string promoted = newTemp();
                if (rhsSize > lhsSize) {
                    emit(promoted + " = trunc " + rhsVal.type + " " + rhsReg + " to " + lhsType);
                } else {
                    emit(promoted + " = sext " + rhsVal.type + " " + rhsReg + " to " + lhsType);
                }
                rhsReg = promoted;
            }
        }
        
        // Perform operation
        std::string resultReg = newTemp();
        std::string opName;
        switch (node.op) {
            case AST::BinaryOp::AddAssign: opName = "add"; break;
            case AST::BinaryOp::SubAssign: opName = "sub"; break;
            case AST::BinaryOp::MulAssign: opName = "mul"; break;
            case AST::BinaryOp::DivAssign: opName = "sdiv"; break;
            case AST::BinaryOp::ModAssign: opName = "srem"; break;
            case AST::BinaryOp::AndAssign: opName = "and"; break;
            case AST::BinaryOp::OrAssign: opName = "or"; break;
            case AST::BinaryOp::XorAssign: opName = "xor"; break;
            case AST::BinaryOp::LeftShiftAssign: opName = "shl"; break;
            case AST::BinaryOp::RightShiftAssign: opName = "ashr"; break;
            default: opName = "add"; break;
        }
        emit(resultReg + " = " + opName + " " + lhsType + " " + lhsReg + ", " + rhsReg);
        
        // Store back (handles bitfields)
        storeValue(IRValue(resultReg, lhsType, false, false), lhsPtr);
        
        lastValue_ = IRValue(resultReg, lhsType, false, false);
        return;
    }
    
    // Handle logical AND/OR (short-circuit evaluation)
    if (node.op == AST::BinaryOp::LogicalAnd || node.op == AST::BinaryOp::LogicalOr) {
        std::string rhsLabel = newLabel("land.rhs");
        std::string endLabel = newLabel("land.end");
        
        // Evaluate LHS
        node.left->accept(*this);
        IRValue lhsLoaded = loadValue(lastValue_);
        std::string lhsReg = lhsLoaded.name;
        
        std::string cmpReg = newTemp();
        emit(cmpReg + " = icmp ne " + lhsLoaded.type + " " + lhsReg + ", 0");
        
        std::string lhsEvalLabel = newLabel("land.lhs.eval");
        emit("br label %" + lhsEvalLabel);
        emitLabel(lhsEvalLabel);
        
        if (node.op == AST::BinaryOp::LogicalAnd) {
            emit("br i1 " + cmpReg + ", label %" + rhsLabel + ", label %" + endLabel);
        } else {
            emit("br i1 " + cmpReg + ", label %" + endLabel + ", label %" + rhsLabel);
        }
        
        // RHS block
        emitLabel(rhsLabel);
        node.right->accept(*this);
        IRValue rhsLoaded = loadValue(lastValue_);
        std::string rhsReg = rhsLoaded.name;
        
        std::string rhsCmp = newTemp();
        emit(rhsCmp + " = icmp ne " + rhsLoaded.type + " " + rhsReg + ", 0");
        
        // Merge point from RHS evaluation might be in a different block
        std::string rhsFromLabel = newLabel("land.rhs.from");
        emit("br label %" + rhsFromLabel);
        emitLabel(rhsFromLabel);
        emit("br label %" + endLabel);
        
        // End block with phi
        emitLabel(endLabel);
        std::string result = newTemp();
        if (node.op == AST::BinaryOp::LogicalAnd) {
            emit(result + " = phi i1 [ false, %" + lhsEvalLabel + " ], [ " + rhsCmp + ", %" + rhsFromLabel + " ]");
        } else {
            emit(result + " = phi i1 [ true, %" + lhsEvalLabel + " ], [ " + rhsCmp + ", %" + rhsFromLabel + " ]");
        }
        
        // Extend to i32
        std::string extResult = newTemp();
        emit(extResult + " = zext i1 " + result + " to i32");
        
        lastValue_ = IRValue(extResult, "i32", false, false);
        return;
    }
    
    // Regular binary operations
    node.left->accept(*this);
    IRValue lhsVal = loadValue(lastValue_);
    
    node.right->accept(*this);
    IRValue rhsVal = loadValue(lastValue_);
    
    std::string lhsReg = lhsVal.name;
    std::string lhsType = lhsVal.type;
    
    std::string rhsReg = rhsVal.name;
    std::string rhsType = rhsVal.type;
    
    // Promote integer types to common type for arithmetic operations
    auto getIntSize = [](const std::string& t) -> int {
        if (t == "i8") return 8;
        if (t == "i16") return 16;
        if (t == "i32") return 32;
        if (t == "i64") return 64;
        return 0;
    };
    
    int lhsSize = getIntSize(lhsType);
    int rhsSize = getIntSize(rhsType);
    
    // Promote to larger type if both are integers
    if (lhsSize > 0 && rhsSize > 0 && lhsSize != rhsSize) {
        if (lhsSize > rhsSize) {
            std::string promoted = newTemp();
            emit(promoted + " = sext " + rhsType + " " + rhsReg + " to " + lhsType);
            rhsReg = promoted;
            rhsType = lhsType;
        } else {
            std::string promoted = newTemp();
            emit(promoted + " = sext " + lhsType + " " + lhsReg + " to " + rhsType);
            lhsReg = promoted;
            lhsType = rhsType;
        }
    }
    
    // Helper to check if type is floating point
    auto isFloatType = [](const std::string& t) -> bool {
        return t == "float" || t == "double";
    };
    
    std::string result;
    
    switch (node.op) {
        case AST::BinaryOp::Add:
            result = newTemp();
            // Handle pointer arithmetic: ptr + int or int + ptr
            if (!lhsType.empty() && lhsType.back() == '*') {
                // Pointer + integer: use getelementptr
                std::string elemType = lhsType.substr(0, lhsType.size() - 1);
                emit(result + " = getelementptr inbounds " + elemType + ", " + lhsType + " " + lhsReg + ", i32 " + rhsReg);
            } else if (!rhsType.empty() && rhsType.back() == '*') {
                // Integer + pointer: use getelementptr (swap operands)
                std::string elemType = rhsType.substr(0, rhsType.size() - 1);
                emit(result + " = getelementptr inbounds " + elemType + ", " + rhsType + " " + rhsReg + ", i32 " + lhsReg);
            } else if (isFloatType(lhsType)) {
                emit(result + " = fadd " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(result + " = add " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            break;
        case AST::BinaryOp::Sub:
            result = newTemp();
            // Handle pointer arithmetic: ptr - int or ptr - ptr
            if (!lhsType.empty() && lhsType.back() == '*') {
                if (!rhsType.empty() && rhsType.back() == '*') {
                    // Pointer - pointer: compute difference in bytes, then divide by element size
                    std::string lhsInt = newTemp();
                    std::string rhsInt = newTemp();
                    emit(lhsInt + " = ptrtoint " + lhsType + " " + lhsReg + " to i32");
                    emit(rhsInt + " = ptrtoint " + rhsType + " " + rhsReg + " to i32");
                    emit(result + " = sub i32 " + lhsInt + ", " + rhsInt);
                    lhsType = "i32";  // Result is integer
                } else {
                    // Pointer - integer: use getelementptr with negated index
                    std::string elemType = lhsType.substr(0, lhsType.size() - 1);
                    std::string negIdx = newTemp();
                    emit(negIdx + " = sub i32 0, " + rhsReg);
                    emit(result + " = getelementptr inbounds " + elemType + ", " + lhsType + " " + lhsReg + ", i32 " + negIdx);
                }
            } else if (isFloatType(lhsType)) {
                emit(result + " = fsub " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(result + " = sub " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            break;
        case AST::BinaryOp::Mul:
            result = newTemp();
            if (isFloatType(lhsType)) {
                emit(result + " = fmul " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(result + " = mul " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            break;
        case AST::BinaryOp::Div:
            result = newTemp();
            if (isFloatType(lhsType)) {
                emit(result + " = fdiv " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(result + " = sdiv " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            break;
        case AST::BinaryOp::Mod:
            result = newTemp();
            if (isFloatType(lhsType)) {
                emit(result + " = frem " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(result + " = srem " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            break;
        case AST::BinaryOp::BitwiseAnd:
            result = newTemp();
            emit(result + " = and " + lhsType + " " + lhsReg + ", " + rhsReg);
            break;
        case AST::BinaryOp::BitwiseOr:
            result = newTemp();
            emit(result + " = or " + lhsType + " " + lhsReg + ", " + rhsReg);
            break;
        case AST::BinaryOp::BitwiseXor:
            result = newTemp();
            emit(result + " = xor " + lhsType + " " + lhsReg + ", " + rhsReg);
            break;
        case AST::BinaryOp::LeftShift:
            result = newTemp();
            emit(result + " = shl " + lhsType + " " + lhsReg + ", " + rhsReg);
            break;
        case AST::BinaryOp::RightShift:
            result = newTemp();
            emit(result + " = ashr " + lhsType + " " + lhsReg + ", " + rhsReg);
            break;
        case AST::BinaryOp::Equal: {
            std::string cmp = newTemp();
            // For pointer comparison with 0, use null
            std::string rhsValue = rhsReg;
            if (!lhsType.empty() && lhsType.back() == '*' && rhsVal.isConstant) {
                // Check if RHS is 0 (in any form)
                try {
                    if (std::stoll(rhsReg) == 0) {
                        rhsValue = "null";
                    }
                } catch (...) {}
            }
            emit(cmp + " = icmp eq " + lhsType + " " + lhsReg + ", " + rhsValue);
            result = newTemp();
            emit(result + " = zext i1 " + cmp + " to i32");
            lhsType = "i32";  // Result type is always i32
            break;
        }
        case AST::BinaryOp::NotEqual: {
            std::string cmp = newTemp();
            // For pointer comparison with 0, use null
            std::string rhsValue = rhsReg;
            if (!lhsType.empty() && lhsType.back() == '*' && rhsVal.isConstant) {
                try {
                    if (std::stoll(rhsReg) == 0) {
                        rhsValue = "null";
                    }
                } catch (...) {}
            }
            emit(cmp + " = icmp ne " + lhsType + " " + lhsReg + ", " + rhsValue);
            result = newTemp();
            emit(result + " = zext i1 " + cmp + " to i32");
            lhsType = "i32";  // Result type is always i32
            break;
        }
        case AST::BinaryOp::Less: {
            std::string cmp = newTemp();
            // For pointer comparisons, use unsigned comparison
            if (!lhsType.empty() && lhsType.back() == '*') {
                emit(cmp + " = icmp ult " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(cmp + " = icmp slt " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            result = newTemp();
            emit(result + " = zext i1 " + cmp + " to i32");
            lhsType = "i32";  // Result type is always i32
            break;
        }
        case AST::BinaryOp::LessEqual: {
            std::string cmp = newTemp();
            if (!lhsType.empty() && lhsType.back() == '*') {
                emit(cmp + " = icmp ule " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(cmp + " = icmp sle " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            result = newTemp();
            emit(result + " = zext i1 " + cmp + " to i32");
            lhsType = "i32";  // Result type is always i32
            break;
        }
        case AST::BinaryOp::Greater: {
            std::string cmp = newTemp();
            if (!lhsType.empty() && lhsType.back() == '*') {
                emit(cmp + " = icmp ugt " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(cmp + " = icmp sgt " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            result = newTemp();
            emit(result + " = zext i1 " + cmp + " to i32");
            lhsType = "i32";  // Result type is always i32
            break;
        }
        case AST::BinaryOp::GreaterEqual: {
            std::string cmp = newTemp();
            if (!lhsType.empty() && lhsType.back() == '*') {
                emit(cmp + " = icmp uge " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(cmp + " = icmp sge " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            result = newTemp();
            emit(result + " = zext i1 " + cmp + " to i32");
            lhsType = "i32";  // Result type is always i32
            break;
        }
        case AST::BinaryOp::Comma:
            // Comma operator: result is RHS
            lastValue_ = IRValue(rhsReg, rhsVal.derefType(), false, false);
            return;
        default:
            result = newTemp();
            emit(result + " = add " + lhsType + " " + lhsReg + ", " + rhsReg);
            break;
    }
    
    lastValue_ = IRValue(result, lhsType, false, false);
}

// ============================================================================
// Unary Expression
// ============================================================================

void IRGenerator::visit(AST::UnaryExpr& node) {
    // Handle pre/post increment/decrement
    if (node.op == AST::UnaryOp::PreIncrement || node.op == AST::UnaryOp::PreDecrement ||
        node.op == AST::UnaryOp::PostIncrement || node.op == AST::UnaryOp::PostDecrement) {
        
        node.operand->accept(*this);
        IRValue ptr = lastValue_;
        
        // Load current value
        std::string oldVal = newTemp();
        std::string valType = ptr.derefType();
        emit(oldVal + " = load " + valType + ", " + ptr.type + " " + ptr.name);
        
        // Compute new value - handle pointer increment/decrement
        std::string newVal = newTemp();
        if (!valType.empty() && valType.back() == '*') {
            // Pointer arithmetic
            std::string elemType = valType.substr(0, valType.size() - 1);
            if (node.op == AST::UnaryOp::PreIncrement || node.op == AST::UnaryOp::PostIncrement) {
                emit(newVal + " = getelementptr inbounds " + elemType + ", " + valType + " " + oldVal + ", i32 1");
            } else {
                emit(newVal + " = getelementptr inbounds " + elemType + ", " + valType + " " + oldVal + ", i32 -1");
            }
        } else {
            // Regular arithmetic
            if (node.op == AST::UnaryOp::PreIncrement || node.op == AST::UnaryOp::PostIncrement) {
                emit(newVal + " = add " + valType + " " + oldVal + ", 1");
            } else {
                emit(newVal + " = sub " + valType + " " + oldVal + ", 1");
            }
        }
        
        // Store new value
        emit("store " + valType + " " + newVal + ", " + ptr.type + " " + ptr.name);
        
        // Return old or new value
        if (node.op == AST::UnaryOp::PreIncrement || node.op == AST::UnaryOp::PreDecrement) {
            lastValue_ = IRValue(newVal, valType, false, false);
        } else {
            lastValue_ = IRValue(oldVal, valType, false, false);
        }
        return;
    }
    
    node.operand->accept(*this);
    IRValue operand = lastValue_;
    
    switch (node.op) {
        case AST::UnaryOp::Plus:
            // No-op
            if (operand.isPointer && !operand.isConstant) {
                std::string temp = newTemp();
                emit(temp + " = load " + operand.derefType() + ", " + operand.type + " " + operand.name);
                lastValue_ = IRValue(temp, operand.derefType(), false, false);
            }
            break;
            
        case AST::UnaryOp::Negate: {
            std::string valReg = operand.name;
            std::string valType = operand.type;
            if (operand.isPointer && !operand.isConstant) {
                valReg = newTemp();
                valType = operand.derefType();
                emit(valReg + " = load " + valType + ", " + operand.type + " " + operand.name);
            }
            std::string result = newTemp();
            // Use fneg for floating point, sub for integers
            if (valType == "float" || valType == "double") {
                emit(result + " = fneg " + valType + " " + valReg);
            } else {
                emit(result + " = sub " + valType + " 0, " + valReg);
            }
            lastValue_ = IRValue(result, valType, false, false);
            break;
        }
        
        case AST::UnaryOp::BitwiseNot: {
            std::string valReg = operand.name;
            std::string valType = operand.type;
            if (operand.isPointer && !operand.isConstant) {
                valReg = newTemp();
                valType = operand.derefType();
                emit(valReg + " = load " + valType + ", " + operand.type + " " + operand.name);
            }
            std::string result = newTemp();
            emit(result + " = xor " + valType + " " + valReg + ", -1");
            lastValue_ = IRValue(result, valType, false, false);
            break;
        }
        
        case AST::UnaryOp::LogicalNot: {
            std::string valReg = operand.name;
            std::string valType = operand.type;
            if (operand.isPointer && !operand.isConstant) {
                valReg = newTemp();
                valType = operand.derefType();
                emit(valReg + " = load " + valType + ", " + operand.type + " " + operand.name);
            }
            std::string cmp = newTemp();
            // For pointers, compare against null
            if (!valType.empty() && valType.back() == '*') {
                emit(cmp + " = icmp eq " + valType + " " + valReg + ", null");
            } else {
                emit(cmp + " = icmp eq " + valType + " " + valReg + ", 0");
            }
            std::string result = newTemp();
            emit(result + " = zext i1 " + cmp + " to i32");
            lastValue_ = IRValue(result, "i32", false, false);
            break;
        }
        
        case AST::UnaryOp::AddressOf:
            // Return the pointer itself
            if (operand.isPointer) {
                lastValue_ = IRValue(operand.name, operand.type, false, false);
            }
            break;
            
        case AST::UnaryOp::Dereference: {
            std::string ptrReg = operand.name;
            std::string ptrType = operand.type;
            if (operand.isPointer && !operand.isConstant) {
                ptrReg = newTemp();
                ptrType = operand.derefType();
                emit(ptrReg + " = load " + ptrType + ", " + operand.type + " " + operand.name);
            }
            // Return as pointer (lvalue)
            lastValue_ = IRValue(ptrReg, ptrType, true, false);
            break;
        }
        
        default:
            break;
    }
}

// ============================================================================
// Cast Expression
// ============================================================================

void IRGenerator::visit(AST::CastExpr& node) {
    node.operand->accept(*this);
    IRValue srcVal = loadValue(lastValue_);

    std::string srcReg = srcVal.name;
    std::string srcType = srcVal.type;
    
    std::string dstType = typeToLLVM(node.targetType.get());
    
    if (srcType == dstType) {
        lastValue_ = IRValue(srcReg, dstType, false, false);
        return;
    }
    
    std::string result = newTemp();
    
    // Determine cast type
    bool srcIsPtr = srcType.back() == '*';
    bool dstIsPtr = dstType.back() == '*';
    
    if (srcIsPtr && dstIsPtr) {
        emit(result + " = bitcast " + srcType + " " + srcReg + " to " + dstType);
    } else if (srcIsPtr && !dstIsPtr) {
        emit(result + " = ptrtoint " + srcType + " " + srcReg + " to " + dstType);
    } else if (!srcIsPtr && dstIsPtr) {
        emit(result + " = inttoptr " + srcType + " " + srcReg + " to " + dstType);
    } else {
        // Integer/float conversions
        int srcSize = 0, dstSize = 0;
        if (srcType == "i8") srcSize = 8;
        else if (srcType == "i16") srcSize = 16;
        else if (srcType == "i32") srcSize = 32;
        else if (srcType == "i64") srcSize = 64;
        
        if (dstType == "i8") dstSize = 8;
        else if (dstType == "i16") dstSize = 16;
        else if (dstType == "i32") dstSize = 32;
        else if (dstType == "i64") dstSize = 64;
        
        if (dstSize > srcSize) {
            emit(result + " = sext " + srcType + " " + srcReg + " to " + dstType);
        } else if (dstSize < srcSize) {
            emit(result + " = trunc " + srcType + " " + srcReg + " to " + dstType);
        } else {
            emit(result + " = bitcast " + srcType + " " + srcReg + " to " + dstType);
        }
    }
    
    lastValue_ = IRValue(result, dstType, false, false);
}

// ============================================================================
// Sizeof Expression
// ============================================================================

void IRGenerator::visit(AST::SizeofExpr& node) {
    int size = 0;
    
    if (node.targetType) {
        size = getTypeSize(node.targetType.get());
    } else if (node.operand) {
        // sizeof(expr)
        if (node.operand->resolvedType) {
            size = getTypeSize(node.operand->resolvedType.get());
        } else {
            // Fallback
            size = 4;
        }
    }
    
    lastValue_ = IRValue(std::to_string(size), "i32", false, true);
}

// ============================================================================
// Call Expression
// ============================================================================

void IRGenerator::visit(AST::CallExpr& node) {
    // Get function name
    std::string funcName;
    if (auto* id = dynamic_cast<AST::Identifier*>(node.callee.get())) {
        funcName = "@" + id->name;
    } else {
        // Function pointer
        node.callee->accept(*this);
        funcName = lastValue_.name;
    }
    
    // Try to recover function parameter types from the emitted declaration (if available)
    std::vector<std::string> paramTypes;
    bool isVariadic = false;
    std::string calleeIdName;
    if (auto* id = dynamic_cast<AST::Identifier*>(node.callee.get())) {
        calleeIdName = id->name;
        auto it = functionDeclarations_.find(id->name);
        if (it != functionDeclarations_.end()) {
            parseFunctionDeclParamTypes(it->second, paramTypes, isVariadic);
        }
    }
    if (calleeIdName == "printf") {
        isVariadic = true;
    }

    // Evaluate arguments with C semantics:
    // - arrays/structs are passed as pointers (do not aggregate-load)
    // - apply prototype pointer casts when needed
    // - apply default argument promotions for variadic calls
    std::vector<std::pair<std::string, std::string>> args;
    args.reserve(node.arguments.size());
    for (size_t i = 0; i < node.arguments.size(); ++i) {
        auto& arg = node.arguments[i];
        arg->accept(*this);
        IRValue argVal = lastValue_;

        std::string argReg = argVal.name;
        std::string argType = argVal.type;

        // Bitfield members are represented as lvalue refs; extract their rvalue now.
        if (argVal.isBitfieldRef) {
            IRValue loaded = loadValue(argVal);
            argReg = loaded.name;
            argType = loaded.type;
            argVal = loaded;
        }

        // Load lvalues for scalars/pointers, but NOT for aggregates.
        if (shouldLoadCallArgument(argVal)) {
            argReg = newTemp();
            argType = argVal.derefType();
            emit(argReg + " = load " + argType + ", " + argVal.type + " " + argVal.name);
        }

        // Prototype-based coercion for fixed parameters (mostly pointers)
        if (i < paramTypes.size()) {
            const std::string& paramTy = paramTypes[i];
            if (argType != paramTy) {
                // Pointer-to-pointer bitcasts (e.g. T* -> i8*)
                if (!argType.empty() && argType.back() == '*' && !paramTy.empty() && paramTy.back() == '*') {
                    std::string casted = newTemp();
                    emit(casted + " = bitcast " + argType + " " + argReg + " to " + paramTy);
                    argReg = casted;
                    argType = paramTy;
                } else {
                    auto intBits = [](const std::string& t) -> int {
                        if (t == "i1") return 1;
                        if (t == "i8") return 8;
                        if (t == "i16") return 16;
                        if (t == "i32") return 32;
                        if (t == "i64") return 64;
                        return 0;
                    };
                    int srcBits = intBits(argType);
                    int dstBits = intBits(paramTy);

                    // Integer <-> integer coercion to the parameter type.
                    if (srcBits > 0 && dstBits > 0) {
                        std::string casted = newTemp();
                        if (dstBits < srcBits) {
                            emit(casted + " = trunc " + argType + " " + argReg + " to " + paramTy);
                        } else if (dstBits > srcBits) {
                            bool isUnsigned = isUnsignedIntegralResolvedType(*arg);
                            emit(casted + " = " + std::string(isUnsigned ? "zext" : "sext") + " " + argType + " " + argReg + " to " + paramTy);
                        } else {
                            emit(casted + " = bitcast " + argType + " " + argReg + " to " + paramTy);
                        }
                        argReg = casted;
                        argType = paramTy;
                    }
                }
            }
        } else if (isVariadic) {
            // Default argument promotions for variadic args
            if (argType == "float") {
                std::string ext = newTemp();
                emit(ext + " = fpext float " + argReg + " to double");
                argReg = ext;
                argType = "double";
            } else if (argType == "i8" || argType == "i16") {
                std::string ext = newTemp();
                bool isUnsigned = isUnsignedIntegralResolvedType(*arg);
                emit(ext + " = " + std::string(isUnsigned ? "zext" : "sext") + " " + argType + " " + argReg + " to i32");
                argReg = ext;
                argType = "i32";
            }
        }

        args.push_back({argType, argReg});
    }
    
    // Look up function to get return type
    std::string returnType = "i32";  // Default
    IRSymbol* sym = nullptr;
    if (auto* id = dynamic_cast<AST::Identifier*>(node.callee.get())) {
        sym = lookupSymbol(id->name);
        if (sym && sym->isFunction) {
            returnType = sym->type;
        }
    }
    
    // Build call
    std::string result;
    if (returnType != "void") {
        result = newTemp();
    }
    
    std::stringstream call;
    if (returnType != "void") {
        call << result << " = ";
    }
    call << "call " << returnType << " " << funcName << "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) call << ", ";
        call << args[i].first << " " << args[i].second;
    }
    call << ")";
    
    emit(call.str());
    
    if (returnType != "void") {
        lastValue_ = IRValue(result, returnType, false, false);
    } else {
        lastValue_ = IRValue("", "void", false, false);
    }
}

// ============================================================================
// Member Expression
// ============================================================================

void IRGenerator::visit(AST::MemberExpr& node) {
    node.object->accept(*this);
    IRValue objVal = lastValue_;

    // For '->', the object expression should evaluate to a pointer value.
    // If it is a pointer lvalue (e.g. function parameter stored in an alloca, type T**), load it.
    if (node.isArrow && objVal.isPointer && !objVal.isConstant) {
        std::string dt = objVal.derefType();
        if (!dt.empty() && dt.back() == '*') {
            objVal = loadValue(objVal);
        }
    }
    
    DebugLogger::instance().logExpr("MemberExpr.object", objVal.type, objVal.name, objVal.isPointer, objVal.isConstant);
    
    // Determine the actual struct type
    std::string structType = objVal.type;
    std::string structLLVMType = structType;  // The type being indexed
    
    // For arrow operator (->): object is a pointer to struct
    // For dot operator (.):   object is the struct itself (or a local var alloca'd as pointer)
    
    if (node.isArrow) {
        // Arrow: object is a pointer to struct
        // structLLVMType is already the pointer, need to dereference it for getelementptr
        if (structType.back() == '*') {
            structLLVMType = structType.substr(0, structType.size() - 1);
        }
    } else {
        // Dot: object might be from alloca (pointer) or an aggregate
        // If it's a pointer from alloca, we need to index into the pointed-to type
        if (structType.back() == '*') {
            structLLVMType = structType.substr(0, structType.size() - 1);
        }
    }
    
    DebugLogger::instance().log("[MemberExpr] operator=" + std::string(node.isArrow ? "->" : ".") + 
                               " member=" + node.member + " structType=" + structType + 
                               " structLLVMType=" + structLLVMType);
    
    // Sanity check: if structLLVMType is not a struct or pointer, something's wrong
    // This can happen if previous operations returned a scalar value
    if (structLLVMType.find('{') != 0 && structLLVMType.find("%struct.") != 0) {
        // Scalar type - can't do member access
        DebugLogger::instance().log("[MemberExpr] ERROR: Not a struct type! " + structLLVMType);
        // Default to i32 pointer as fallback
        lastValue_ = IRValue(newTemp(), "i32*", true, false);
        return;
    }
    
    // Extract struct name if it's a named struct (%struct.name)
    std::string structName;
    if (structLLVMType.find("%struct.") == 0) {
        structName = structLLVMType.substr(8);  // Remove "%struct." prefix
    }
    
    // Get struct layout
    const StructLayout* layout = nullptr;
    if (!structName.empty()) {
        layout = getStructLayout(structName);
    }
    
    // Determine member index
    int memberIdx = 0;
    std::string memberType = "i32";
    
    if (layout) {
        auto it = layout->memberIndices.find(node.member);
        if (it != layout->memberIndices.end()) {
            memberIdx = it->second;
            auto typeIt = layout->memberTypes.find(node.member);
            if (typeIt != layout->memberTypes.end()) {
                memberType = typeIt->second;
            }
        }
    } else if (structLLVMType.find('{') == 0) {
        // Inline struct type - try to find member index
        int foundIdx = findMemberIndexInInlineStruct(structLLVMType, node.member);
        if (foundIdx >= 0) {
            memberIdx = foundIdx;
        }
        // Now extract the field type using the correct index
        memberType = extractFieldTypeFromInlineStruct(structLLVMType, memberIdx);
    }
    
    DebugLogger::instance().log("[MemberExpr] memberIdx=" + std::to_string(memberIdx) + 
                               " memberType=" + memberType);

    // Bitfield read: extract bits from packed storage (bitfields are not addressable)
    if (layout) {
        auto wIt = layout->bitfieldWidths.find(node.member);
        auto oIt = layout->bitfieldOffsets.find(node.member);
        if (wIt != layout->bitfieldWidths.end() && oIt != layout->bitfieldOffsets.end()) {
            int bitWidth = wIt->second;
            int bitOffset = oIt->second;

            std::string storagePtr = newTemp();
            emit(storagePtr + " = getelementptr inbounds " + structLLVMType + ", " +
                 objVal.type + " " + objVal.name + ", i32 0, i32 " + std::to_string(memberIdx));

            // Represent as a bitfield reference. Loads/stores are handled in loadValue/storeValue.
            IRValue ref(storagePtr, memberType + "*", true, false);
            ref.isBitfieldRef = true;
            ref.bitfieldStorageType = memberType;
            ref.bitfieldOffset = bitOffset;
            ref.bitfieldWidth = bitWidth;

            // Signedness from declared member type (preferred).
            bool isUnsigned = false;
            if (layout) {
                auto sIt = layout->bitfieldIsUnsigned.find(node.member);
                if (sIt != layout->bitfieldIsUnsigned.end()) {
                    isUnsigned = sIt->second;
                }
            }
            ref.bitfieldIsUnsigned = isUnsigned;

            lastValue_ = ref;
            return;
        }
    }
    
    // Generate getelementptr
    // Pattern: getelementptr inbounds <pointee_type>, <ptr_type> <ptr>, i32 0, i32 <member_idx>
    std::string memberPtr = newTemp();
    DebugLogger::instance().log("[MemberExpr] GEP: pointee=" + structLLVMType + 
                               " ptrType=" + objVal.type + " ptr=" + objVal.name);
    emit(memberPtr + " = getelementptr inbounds " + structLLVMType + ", " + 
         objVal.type + " " + objVal.name + ", i32 0, i32 " + std::to_string(memberIdx));
    
    DebugLogger::instance().logExpr("MemberExpr.result", memberType + "*", memberPtr, true, false);
    lastValue_ = IRValue(memberPtr, memberType + "*", true, false);
}

// ============================================================================
// Index Expression
// ============================================================================

void IRGenerator::visit(AST::IndexExpr& node) {
    node.array->accept(*this);
    IRValue arrVal = lastValue_;
    
    node.index->accept(*this);
    IRValue idxVal = lastValue_;
    
    DebugLogger::instance().logExpr("IndexExpr.array", arrVal.type, arrVal.name, arrVal.isPointer, arrVal.isConstant);
    DebugLogger::instance().logExpr("IndexExpr.index", idxVal.type, idxVal.name, idxVal.isPointer, idxVal.isConstant);
    
    // Get the array pointer
    std::string arrPtr = arrVal.name;
    std::string arrType = arrVal.type;
    
    // For arrays, if the pointer is in a register (isPointer), we use it directly
    // We don't load arrays - arrays always decay to pointers to their first element
    if (arrVal.isPointer && !arrVal.isConstant) {
        // Check if the dereferenced type is an array
        std::string derefType = arrVal.derefType();
        if (derefType.find('[') != std::string::npos) {
            // Array type - use the pointer directly without loading
            // The pointer is already in arrVal.name
            // arrType is already the correct pointer type
            DebugLogger::instance().log("[IndexExpr] Array type detected, using pointer directly: " + arrType);
        } else {
            // Non-array pointer - load the value
            arrPtr = newTemp();
            arrType = arrVal.derefType();
            DebugLogger::instance().log("[IndexExpr] Non-array pointer, loading value. arrType=" + arrType);
            emit(arrPtr + " = load " + arrType + ", " + arrVal.type + " " + arrVal.name);
        }
    }
    
    // Load index if needed
    std::string idxReg = idxVal.name;
    if (idxVal.isPointer && !idxVal.isConstant) {
        idxReg = newTemp();
        emit(idxReg + " = load " + idxVal.derefType() + ", " + idxVal.type + " " + idxVal.name);
    }
    
    // Compute element type from pointer type
    std::string elemType = arrType;
    if (elemType.back() == '*') {
        elemType = elemType.substr(0, elemType.size() - 1);
    }
    
    // Compute element pointer
    std::string elemPtr = newTemp();
    DebugLogger::instance().log("[IndexExpr] GEP: elemType=" + elemType + " arrType=" + arrType + " arrPtr=" + arrPtr);
    if (!elemType.empty() && elemType[0] == '[') {
        // Indexing into an array object: [N x T]* p; p[i] needs indices (0, i)
        emit(elemPtr + " = getelementptr inbounds " + elemType + ", " + arrType + " " + arrPtr + ", i32 0, " + idxVal.derefType() + " " + idxReg);
    } else {
        emit(elemPtr + " = getelementptr inbounds " + elemType + ", " + arrType + " " + arrPtr + ", " + idxVal.derefType() + " " + idxReg);
    }
    
    // For array types like [N x T], the element type is T, not [N x T]
    std::string resultType = elemType + "*";
    if (elemType[0] == '[') {
        // Array type like "[2 x i32]" - extract element type
        size_t xPos = elemType.find(" x ");
        if (xPos != std::string::npos) {
            // Type is between "x " and the closing "]"
            size_t typeStart = xPos + 3;  // After "x "
            size_t typeEnd = elemType.rfind(']');
            if (typeEnd > typeStart) {
                resultType = elemType.substr(typeStart, typeEnd - typeStart) + "*";
            }
        }
    }
    
    DebugLogger::instance().logExpr("IndexExpr.result", resultType, elemPtr, true, false);
    lastValue_ = IRValue(elemPtr, resultType, true, false);
}

// ============================================================================
// Ternary Expression
// ============================================================================

void IRGenerator::visit(AST::TernaryExpr& node) {
    std::string thenLabel = newLabel("cond.true");
    std::string elseLabel = newLabel("cond.false");
    std::string endLabel = newLabel("cond.end");
    
    // Evaluate condition
    node.condition->accept(*this);
    IRValue condLoaded = loadValue(lastValue_);
    std::string condReg = condLoaded.name;
    std::string condType = condLoaded.type;

    std::string cmpReg = newTemp();
    emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", 0");
    emit("br i1 " + cmpReg + ", label %" + thenLabel + ", label %" + elseLabel);
    
    // Then branch
    emitLabel(thenLabel);
    node.thenExpr->accept(*this);
    IRValue thenVal = loadValue(lastValue_);
    std::string thenReg = thenVal.name;
    std::string thenType = thenVal.type;
    std::string thenBlock = newLabel("then.from");
    emit("br label %" + thenBlock);
    emitLabel(thenBlock);
    emit("br label %" + endLabel);
    
    // Else branch
    emitLabel(elseLabel);
    node.elseExpr->accept(*this);
    IRValue elseVal = loadValue(lastValue_);
    std::string elseReg = elseVal.name;
    std::string elseBlock = newLabel("else.from");
    emit("br label %" + elseBlock);
    emitLabel(elseBlock);
    emit("br label %" + endLabel);
    
    // Merge with phi
    emitLabel(endLabel);
    std::string result = newTemp();
    emit(result + " = phi " + thenType + " [ " + thenReg + ", %" + thenBlock + " ], [ " + elseReg + ", %" + elseBlock + " ]");
    
    lastValue_ = IRValue(result, thenType, false, false);
}

// ============================================================================
// Initializer List
// ============================================================================

void IRGenerator::visit(AST::InitializerList& node) {
    // Initializer lists are handled in VarDecl
    // For now, just evaluate first element
    if (!node.initializers.empty()) {
        node.initializers[0]->accept(*this);
    } else {
        lastValue_ = IRValue("0", "i32", false, true);
    }
}

} // namespace cc1
