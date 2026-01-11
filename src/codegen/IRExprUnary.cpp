#include <codegen/IRGenerator.hpp>

namespace cc1 {

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

    // For other unary operations, evaluate operand
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
            emit(cmp + " = icmp eq " + operandType + " " + operandReg + ", 0");
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
            // Address-of returns pointer to operand
            lastValue_ = lastValue_;
            return;
        case AST::UnaryOp::Dereference: {
            // Dereference returns lvalue/pointer to the value
            // If operand already is a pointer value, mark it as lvalue
            IRValue v = lastValue_;
            v.isPointer = true;
            lastValue_ = v;
            return;
        }
        default:
            result = operandReg;
            break;
    }

    lastValue_ = IRValue(result, operandType, false, false);
}

} // namespace cc1
