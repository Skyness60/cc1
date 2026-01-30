#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for simple assignment, storing rhs into lhs.
// FR: Genere l IR pour une assignation simple, store rhs dans lhs.
bool IRGenerator::emitBinaryAssign(AST::BinaryExpr& node) {
    if (node.op != AST::BinaryOp::Assign) return false;

    
    node.right->accept(*this);
    IRValue rhsVal = loadValue(lastValue_);

    
    node.left->accept(*this);
    IRValue lhsVal = lastValue_;

    
    storeValue(rhsVal, lhsVal);

    
    lastValue_ = rhsVal;
    return true;
}

} 
