#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

static void setResolvedExprType(AST::Expression& expr, const AST::Ptr<AST::Type>& type) {
    expr.resolvedType = type ? type->clone() : nullptr;
}

void SemanticAnalyzer::visit(AST::IntegerLiteral& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, node.line, node.column);
    setResolvedExprType(node, exprTypes_[&node]);
}

void SemanticAnalyzer::visit(AST::FloatLiteral& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Double, node.line, node.column);
    setResolvedExprType(node, exprTypes_[&node]);
}

void SemanticAnalyzer::visit(AST::CharLiteral& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Char, node.line, node.column);
    setResolvedExprType(node, exprTypes_[&node]);
}

void SemanticAnalyzer::visit(AST::StringLiteral& node) {
    auto charType = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Char, node.line, node.column);
    auto size = AST::make<AST::IntegerLiteral>(static_cast<long long>(node.value.length() + 1), "", node.line, node.column);
    exprTypes_[&node] = AST::make<AST::ArrayType>(std::move(charType), std::move(size), node.line, node.column);
    setResolvedExprType(node, exprTypes_[&node]);
}

void SemanticAnalyzer::visit(AST::Identifier& node) {
    Symbol* sym = currentScope_->lookup(node.name);
    if (sym && sym->type) {
        exprTypes_[&node] = sym->type->clone();
        setResolvedExprType(node, exprTypes_[&node]);
    }
}

void SemanticAnalyzer::visit(AST::SizeofExpr& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::UnsignedLong, node.line, node.column);
    setResolvedExprType(node, exprTypes_[&node]);
}

void SemanticAnalyzer::visit(AST::InitializerList& node) {
    for (auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

} // namespace cc1
