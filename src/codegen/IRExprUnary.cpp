#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for unary operators, including ++/-- and address/deref.
// FR: Genere l IR pour les unaires, incluant ++/-- et adresse/deref.
void IRGenerator::visit(AST::UnaryExpr& node) {
    
    if (node.op == AST::UnaryOp::PreIncrement || node.op == AST::UnaryOp::PreDecrement ||
        node.op == AST::UnaryOp::PostIncrement || node.op == AST::UnaryOp::PostDecrement) {

        node.operand->accept(*this);
        IRValue ptr = lastValue_;

        
        std::string oldVal = newTemp();
        std::string valType = ptr.derefType();
        emit(oldVal + " = load " + valType + ", " + ptr.type + " " + ptr.name);

        
        std::string newVal = newTemp();
        if (!valType.empty() && valType.back() == '*') {
            
            std::string elemType = valType.substr(0, valType.size() - 1);
            std::string idxType = is64bit_ ? "i64" : "i32";
            if (node.op == AST::UnaryOp::PreIncrement || node.op == AST::UnaryOp::PostIncrement) {
                emit(newVal + " = getelementptr inbounds " + elemType + ", " + valType + " " + oldVal + ", " + idxType + " 1");
            } else {
                emit(newVal + " = getelementptr inbounds " + elemType + ", " + valType + " " + oldVal + ", " + idxType + " -1");
            }
        } else {
            
            if (node.op == AST::UnaryOp::PreIncrement || node.op == AST::UnaryOp::PostIncrement) {
                emit(newVal + " = add " + valType + " " + oldVal + ", 1");
            } else {
                emit(newVal + " = sub " + valType + " " + oldVal + ", 1");
            }
        }

        
        emit("store " + valType + " " + newVal + ", " + ptr.type + " " + ptr.name);

        
        if (node.op == AST::UnaryOp::PreIncrement || node.op == AST::UnaryOp::PreDecrement) {
            lastValue_ = IRValue(newVal, valType, false, false);
        } else {
            lastValue_ = IRValue(oldVal, valType, false, false);
        }
        return;
    }

    
    node.operand->accept(*this);
    IRValue operandVal = loadValue(lastValue_);

    std::string operandReg = operandVal.name;
    std::string operandType = operandVal.type;

    std::string result;

    switch (node.op) {
        case AST::UnaryOp::Negate:
            result = newTemp();
            if (operandType == "float" || operandType == "double") {
                emit(result + " = fsub " + operandType + " 0.0, " + operandReg);
            } else {
                emit(result + " = sub " + operandType + " 0, " + operandReg);
            }
            break;
        case AST::UnaryOp::LogicalNot: {
            std::string cmp = newTemp();
            if (!operandType.empty() && operandType.back() == '*') {
                emit(cmp + " = icmp eq " + operandType + " " + operandReg + ", null");
            } else {
                emit(cmp + " = icmp eq " + operandType + " " + operandReg + ", 0");
            }
            result = newTemp();
            emit(result + " = zext i1 " + cmp + " to i32");
            operandType = "i32";
            break;
        }
        case AST::UnaryOp::BitwiseNot:
            result = newTemp();
            emit(result + " = xor " + operandType + " " + operandReg + ", -1");
            break;
        case AST::UnaryOp::AddressOf:
            
            {
                IRValue v = lastValue_;
                v.isPointer = false;
                lastValue_ = v;
            }
            return;
        case AST::UnaryOp::Dereference: {
            // Dereferencing a pointer makes it addressable (isPointer = true).
            // However, function pointers are special: dereferencing a function pointer
            // in C is a no-op (f, *f, **f are all equivalent for functions).
            // We detect function pointer types by checking if the type contains '('
            // (function signature) and ends with '*'.
            if (!operandType.empty() && operandType.back() == '*') {
                // Check if this is a function pointer type (contains function signature)
                std::string derefType = operandType.substr(0, operandType.size() - 1);
                bool isFunctionPointer = (derefType.find('(') != std::string::npos);
                
                if (isFunctionPointer) {
                    // For function pointers, dereferencing is a no-op; the value is already
                    // the function pointer and should not be marked for another load.
                    lastValue_ = IRValue(operandReg, operandType, false, false);
                } else {
                    // Normal pointer dereference - mark as addressable
                    lastValue_ = IRValue(operandReg, operandType, true, false);
                }
            } else {
                lastValue_ = operandVal;
            }
            return;
        }
        default:
            result = operandReg;
            break;
    }

    lastValue_ = IRValue(result, operandType, false, false);
}

} 
