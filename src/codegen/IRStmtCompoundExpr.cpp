#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for a compound statement with its own scope.
// FR: Genere l IR pour un bloc compose avec scope.
void IRGenerator::visit(AST::CompoundStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    enterScope();

    
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }

    
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }

    exitScope();
}

// EN: Emits IR for an expression statement.
// FR: Genere l IR pour une instruction d expression.
void IRGenerator::visit(AST::ExpressionStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    if (node.expression) {
        node.expression->accept(*this);
        
    }
}

} 
