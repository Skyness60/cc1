#include <codegen/IRGenerator.hpp>

namespace cc1 {

bool IRGenerator::emitBinaryAssign(AST::BinaryExpr& node) {
    if (node.op != AST::BinaryOp::Assign) return false;

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
    return true;
}

} // namespace cc1
