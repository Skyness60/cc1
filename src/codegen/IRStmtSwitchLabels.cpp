#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Case Statement
// ============================================================================

void IRGenerator::visit(AST::CaseStmt& node) {
    if (switchStack_.empty()) {
        // Error: case outside switch
        hadError_ = true;
        return;
    }

    SwitchContext& ctx = switchStack_.top();
    auto it = ctx.labelForNode.find(&node);
    if (it == ctx.labelForNode.end()) {
        hadError_ = true;
        return;
    }
    const std::string& caseLabel = it->second;

    // Fallthrough to this labeled statement.
    if (!blockTerminated_) {
        emit("br label %" + caseLabel);
    }
    emitLabel(caseLabel);

    if (node.body) {
        node.body->accept(*this);
    }
}

// ============================================================================
// Default Statement
// ============================================================================

void IRGenerator::visit(AST::DefaultStmt& node) {
    if (switchStack_.empty()) {
        hadError_ = true;
        return;
    }

    SwitchContext& ctx = switchStack_.top();
    auto it = ctx.labelForNode.find(&node);
    if (it == ctx.labelForNode.end()) {
        hadError_ = true;
        return;
    }
    const std::string& defaultLabel = it->second;

    if (!blockTerminated_) {
        emit("br label %" + defaultLabel);
    }
    emitLabel(defaultLabel);

    if (node.body) {
        node.body->accept(*this);
    }
}

} // namespace cc1
