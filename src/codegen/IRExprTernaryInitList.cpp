#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Ternary Expression
// ============================================================================

void IRGenerator::visit(AST::TernaryExpr& node) {
    // Evaluate condition
    node.condition->accept(*this);
    IRValue condVal = loadValue(lastValue_);

    std::string thenLabel = newLabel("ternary.then");
    std::string elseLabel = newLabel("ternary.else");
    std::string endLabel = newLabel("ternary.end");

    std::string cmpReg = newTemp();
    emit(cmpReg + " = icmp ne " + condVal.type + " " + condVal.name + ", 0");
    emit("br i1 " + cmpReg + ", label %" + thenLabel + ", label %" + elseLabel);

    // Then block
    emitLabel(thenLabel);
    node.thenExpr->accept(*this);
    IRValue thenVal = loadValue(lastValue_);
    std::string thenFrom = newLabel("ternary.then.from");
    emit("br label %" + thenFrom);
    emitLabel(thenFrom);
    emit("br label %" + endLabel);

    // Else block
    emitLabel(elseLabel);
    node.elseExpr->accept(*this);
    IRValue elseVal = loadValue(lastValue_);
    std::string elseFrom = newLabel("ternary.else.from");
    emit("br label %" + elseFrom);
    emitLabel(elseFrom);
    emit("br label %" + endLabel);

    // End block
    emitLabel(endLabel);

    // Use then type as result type (best-effort)
    std::string resultType = thenVal.type;
    std::string phiReg = newTemp();
    emit(phiReg + " = phi " + resultType + " [ " + thenVal.name + ", %" + thenFrom + " ], [ " + elseVal.name + ", %" + elseFrom + " ]");

    lastValue_ = IRValue(phiReg, resultType, false, false);
}

// ============================================================================
// Initializer List (as expression)
// ============================================================================

void IRGenerator::visit(AST::InitializerList& node) {
    // Treat as zero for expression context; handled specially in decl init.
    (void)node;
    lastValue_ = IRValue("0", "i32", false, true);
}

} // namespace cc1
