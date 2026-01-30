#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// EN: Stores a resolved type on the expression for later passes.
// FR: Stocke le type resolu sur l expression pour les passes suivantes.
static void setResolvedExprType(AST::Expression& expr, const AST::Ptr<AST::Type>& type) {
    expr.resolvedType = type ? type->clone() : nullptr;
}

// EN: Assigns integer literal type (default int).
// FR: Assigne le type d un literal entier (int par defaut).
void SemanticAnalyzer::visit(AST::IntegerLiteral& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, node.line, node.column);
    setResolvedExprType(node, exprTypes_[&node]);
}

// EN: Assigns floating literal type (default double).
// FR: Assigne le type d un literal flottant (double par defaut).
void SemanticAnalyzer::visit(AST::FloatLiteral& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Double, node.line, node.column);
    setResolvedExprType(node, exprTypes_[&node]);
}

// EN: Assigns character literal type (char).
// FR: Assigne le type d un literal caractere (char).
void SemanticAnalyzer::visit(AST::CharLiteral& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Char, node.line, node.column);
    setResolvedExprType(node, exprTypes_[&node]);
}

// EN: Assigns string literal type as char array with length + 1.
// FR: Assigne le type de string literal en tableau de char taille+1.
void SemanticAnalyzer::visit(AST::StringLiteral& node) {
    auto charType = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Char, node.line, node.column);
    auto size = AST::make<AST::IntegerLiteral>(static_cast<long long>(node.value.length() + 1), "", node.line, node.column);
    exprTypes_[&node] = AST::make<AST::ArrayType>(std::move(charType), std::move(size), node.line, node.column);
    setResolvedExprType(node, exprTypes_[&node]);
}

// EN: Resolves identifier type from the current scope symbol table.
// FR: Resout le type d un identifiant via la table de symboles.
void SemanticAnalyzer::visit(AST::Identifier& node) {
    Symbol* sym = currentScope_->lookup(node.name);
    if (sym && sym->type) {
        exprTypes_[&node] = sym->type->clone();
        setResolvedExprType(node, exprTypes_[&node]);
    }
}

// EN: Assigns sizeof expression type (unsigned long).
// FR: Assigne le type d un sizeof (unsigned long).
void SemanticAnalyzer::visit(AST::SizeofExpr& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::UnsignedLong, node.line, node.column);
    setResolvedExprType(node, exprTypes_[&node]);
}

// EN: Visits initializer list elements to resolve their types.
// FR: Visite les elements d une liste d initialisation.
void SemanticAnalyzer::visit(AST::InitializerList& node) {
    for (auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

} 
