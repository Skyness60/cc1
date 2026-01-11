#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// While Statement
// ============================================================================

void IRGenerator::visit(AST::WhileStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    std::string condLabel = newLabel("while.cond");
    std::string bodyLabel = newLabel("while.body");
    std::string endLabel = newLabel("while.end");

    // Push break/continue targets
    breakLabels_.push(endLabel);
    continueLabels_.push(condLabel);

    // Branch to condition
    emit("br label %" + condLabel);

    // Condition block
    emitLabel(condLabel);
    node.condition->accept(*this);
    IRValue condVal = lastValue_;

    // Convert to rvalue (also extracts bitfields).
    IRValue condLoaded = loadValue(condVal);
    std::string condReg = condLoaded.name;
    std::string condType = condLoaded.type;

    std::string cmpReg = newTemp();
    if (condType.back() == '*') {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", null");
    } else {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", 0");
    }
    emit("br i1 " + cmpReg + ", label %" + bodyLabel + ", label %" + endLabel);

    // Body block
    emitLabel(bodyLabel);
    node.body->accept(*this);
    emit("br label %" + condLabel);

    // End block
    emitLabel(endLabel);

    // Pop break/continue targets
    breakLabels_.pop();
    continueLabels_.pop();
}

// ============================================================================
// Do-While Statement
// ============================================================================

void IRGenerator::visit(AST::DoWhileStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    std::string bodyLabel = newLabel("do.body");
    std::string condLabel = newLabel("do.cond");
    std::string endLabel = newLabel("do.end");

    // Push break/continue targets
    breakLabels_.push(endLabel);
    continueLabels_.push(condLabel);

    // Branch to body
    emit("br label %" + bodyLabel);

    // Body block
    emitLabel(bodyLabel);
    node.body->accept(*this);
    emit("br label %" + condLabel);

    // Condition block
    emitLabel(condLabel);
    node.condition->accept(*this);
    IRValue condVal = lastValue_;

    // Convert to rvalue (also extracts bitfields).
    IRValue condLoaded = loadValue(condVal);
    std::string condReg = condLoaded.name;
    std::string condType = condLoaded.type;

    std::string cmpReg = newTemp();
    if (condType.back() == '*') {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", null");
    } else {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", 0");
    }
    emit("br i1 " + cmpReg + ", label %" + bodyLabel + ", label %" + endLabel);

    // End block
    emitLabel(endLabel);

    // Pop break/continue targets
    breakLabels_.pop();
    continueLabels_.pop();
}

// ============================================================================
// For Statement
// ============================================================================

void IRGenerator::visit(AST::ForStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    std::string condLabel = newLabel("for.cond");
    std::string bodyLabel = newLabel("for.body");
    std::string incLabel = newLabel("for.inc");
    std::string endLabel = newLabel("for.end");

    enterScope();

    // Init
    if (node.init) {
        node.init->accept(*this);
    }
    emit("br label %" + condLabel);

    // Push break/continue targets
    breakLabels_.push(endLabel);
    continueLabels_.push(incLabel);

    // Condition
    emitLabel(condLabel);
    if (node.condition) {
        node.condition->accept(*this);
        IRValue condVal = lastValue_;

        // Convert to rvalue (also extracts bitfields).
        IRValue condLoaded = loadValue(condVal);
        std::string condReg = condLoaded.name;
        std::string condType = condLoaded.type;

        std::string cmpReg = newTemp();
        if (condType.back() == '*') {
            emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", null");
        } else {
            emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", 0");
        }
        emit("br i1 " + cmpReg + ", label %" + bodyLabel + ", label %" + endLabel);
    } else {
        emit("br label %" + bodyLabel);
    }

    // Body
    emitLabel(bodyLabel);
    if (node.body) {
        node.body->accept(*this);
    }
    emit("br label %" + incLabel);

    // Increment
    emitLabel(incLabel);
    if (node.increment) {
        node.increment->accept(*this);
    }
    emit("br label %" + condLabel);

    // End
    emitLabel(endLabel);

    // Pop break/continue targets
    breakLabels_.pop();
    continueLabels_.pop();

    exitScope();
}

} // namespace cc1
