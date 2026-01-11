#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// If Statement
// ============================================================================

void IRGenerator::visit(AST::IfStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    std::string thenLabel = newLabel("if.then");
    std::string elseLabel = node.elseBranch ? newLabel("if.else") : "";
    std::string endLabel = newLabel("if.end");

    // Evaluate condition
    node.condition->accept(*this);
    IRValue condVal = lastValue_;

    // Convert to rvalue (also extracts bitfields).
    IRValue condLoaded = loadValue(condVal);
    std::string condReg = condLoaded.name;
    std::string condType = condLoaded.type;

    // Compare to zero (C: non-zero is true)
    // For pointers, compare against null
    std::string cmpReg = newTemp();
    if (condType.back() == '*') {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", null");
    } else {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", 0");
    }

    // Branch
    if (node.elseBranch) {
        emit("br i1 " + cmpReg + ", label %" + thenLabel + ", label %" + elseLabel);
    } else {
        emit("br i1 " + cmpReg + ", label %" + thenLabel + ", label %" + endLabel);
    }

    // Then block
    emitLabel(thenLabel);
    node.thenBranch->accept(*this);
    emit("br label %" + endLabel);

    // Else block
    if (node.elseBranch) {
        emitLabel(elseLabel);
        node.elseBranch->accept(*this);
        emit("br label %" + endLabel);
    }

    // End label
    emitLabel(endLabel);
}

} // namespace cc1
