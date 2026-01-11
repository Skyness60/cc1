#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Compound Statement
// ============================================================================

void IRGenerator::visit(AST::CompoundStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    enterScope();

    // Process declarations
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }

    // Process statements
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }

    exitScope();
}

// ============================================================================
// Expression Statement
// ============================================================================

void IRGenerator::visit(AST::ExpressionStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    if (node.expression) {
        node.expression->accept(*this);
        // Result is discarded
    }
}

} // namespace cc1
