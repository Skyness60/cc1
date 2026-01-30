#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits short-circuiting logical AND/OR using labels and phi nodes.
// FR: Genere les AND/OR logiques court-circuites via labels et phi.
bool IRGenerator::emitBinaryLogicalShortCircuit(AST::BinaryExpr& node) {
    if (node.op != AST::BinaryOp::LogicalAnd && node.op != AST::BinaryOp::LogicalOr) {
        return false;
    }

    std::string rhsLabel = newLabel("land.rhs");
    std::string endLabel = newLabel("land.end");

    
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

    
    emitLabel(rhsLabel);
    node.right->accept(*this);
    IRValue rhsLoaded = loadValue(lastValue_);
    std::string rhsReg = rhsLoaded.name;

    std::string rhsCmp = newTemp();
    emit(rhsCmp + " = icmp ne " + rhsLoaded.type + " " + rhsReg + ", 0");

    
    std::string rhsFromLabel = newLabel("land.rhs.from");
    emit("br label %" + rhsFromLabel);
    emitLabel(rhsFromLabel);
    emit("br label %" + endLabel);

    
    emitLabel(endLabel);
    std::string result = newTemp();
    if (node.op == AST::BinaryOp::LogicalAnd) {
        emit(result + " = phi i1 [ false, %" + lhsEvalLabel + " ], [ " + rhsCmp + ", %" + rhsFromLabel + " ]");
    } else {
        emit(result + " = phi i1 [ true, %" + lhsEvalLabel + " ], [ " + rhsCmp + ", %" + rhsFromLabel + " ]");
    }

    
    std::string extResult = newTemp();
    emit(extResult + " = zext i1 " + result + " to i32");

    lastValue_ = IRValue(extResult, "i32", false, false);
    return true;
}

} 
