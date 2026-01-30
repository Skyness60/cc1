#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// EN: Analyzes a compound statement, creating a scope if needed.
// FR: Analyse un bloc compose, en creant un scope si besoin.
void SemanticAnalyzer::visit(AST::CompoundStmt& node) {
    
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

// EN: Analyzes an expression statement.
// FR: Analyse une instruction d expression.
void SemanticAnalyzer::visit(AST::ExpressionStmt& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

// EN: Analyzes an if/else statement.
// FR: Analyse une instruction if/else.
void SemanticAnalyzer::visit(AST::IfStmt& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.thenBranch) node.thenBranch->accept(*this);
    if (node.elseBranch) node.elseBranch->accept(*this);
}

// EN: Analyzes a while loop.
// FR: Analyse une boucle while.
void SemanticAnalyzer::visit(AST::WhileStmt& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

// EN: Analyzes a do-while loop.
// FR: Analyse une boucle do-while.
void SemanticAnalyzer::visit(AST::DoWhileStmt& node) {
    if (node.body) node.body->accept(*this);
    if (node.condition) node.condition->accept(*this);
}

// EN: Analyzes a for loop, visiting init/cond/increment/body.
// FR: Analyse une boucle for, en visitant init/cond/incr/corps.
void SemanticAnalyzer::visit(AST::ForStmt& node) {
    if (node.init) node.init->accept(*this);
    if (node.condition) node.condition->accept(*this);
    if (node.increment) node.increment->accept(*this);
    if (node.body) node.body->accept(*this);
}

// EN: Analyzes a switch statement.
// FR: Analyse une instruction switch.
void SemanticAnalyzer::visit(AST::SwitchStmt& node) {
    if (node.expression) node.expression->accept(*this);
    if (node.body) node.body->accept(*this);
}

// EN: Analyzes a case label and its statement body.
// FR: Analyse un label case et son corps.
void SemanticAnalyzer::visit(AST::CaseStmt& node) {
    if (node.value) node.value->accept(*this);
    if (node.body) node.body->accept(*this);
}

// EN: Analyzes a default label and its body.
// FR: Analyse un label default et son corps.
void SemanticAnalyzer::visit(AST::DefaultStmt& node) {
    if (node.body) node.body->accept(*this);
}

// EN: Analyzes a return statement expression.
// FR: Analyse l expression d un return.
void SemanticAnalyzer::visit(AST::ReturnStmt& node) {
    if (node.value) node.value->accept(*this);
}

// EN: Analyzes a labeled statement body.
// FR: Analyse une instruction avec label.
void SemanticAnalyzer::visit(AST::LabelStmt& node) {
    if (node.body) node.body->accept(*this);
}

} 
