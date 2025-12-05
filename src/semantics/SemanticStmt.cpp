#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// ============================================================================
// Statement Visitors
// ============================================================================

void SemanticAnalyzer::visit(AST::CompoundStmt& node) {
    // Don't create new scope if we just entered a function (already done)
    bool needsScope = currentFunction_ == nullptr || !currentScope_->isFunction;
    
    if (needsScope) {
        enterScope(false);
    }
    
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
    
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
    
    if (needsScope) {
        exitScope();
    }
}

void SemanticAnalyzer::visit(AST::ExpressionStmt& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

void SemanticAnalyzer::visit(AST::IfStmt& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.thenBranch) node.thenBranch->accept(*this);
    if (node.elseBranch) node.elseBranch->accept(*this);
}

void SemanticAnalyzer::visit(AST::WhileStmt& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SemanticAnalyzer::visit(AST::DoWhileStmt& node) {
    if (node.body) node.body->accept(*this);
    if (node.condition) node.condition->accept(*this);
}

void SemanticAnalyzer::visit(AST::ForStmt& node) {
    if (node.init) node.init->accept(*this);
    if (node.condition) node.condition->accept(*this);
    if (node.increment) node.increment->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SemanticAnalyzer::visit(AST::SwitchStmt& node) {
    if (node.expression) node.expression->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SemanticAnalyzer::visit(AST::CaseStmt& node) {
    if (node.value) node.value->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SemanticAnalyzer::visit(AST::DefaultStmt& node) {
    if (node.body) node.body->accept(*this);
}

void SemanticAnalyzer::visit(AST::ReturnStmt& node) {
    if (node.value) node.value->accept(*this);
}

void SemanticAnalyzer::visit(AST::LabelStmt& node) {
    if (node.body) node.body->accept(*this);
}

} // namespace cc1
