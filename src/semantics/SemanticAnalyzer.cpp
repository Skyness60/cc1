#include <semantics/SemanticAnalyzer.hpp>
#include <parser/AST.hpp>

void SemanticAnalyzer::visit(BinaryOpExpr& node)
{
    if (node.getLeft()) {
        node.getLeft()->accept(*this);
    }
    if (node.getRight()) {
        node.getRight()->accept(*this);
    }
    // Check types compatibility here
}

void SemanticAnalyzer::visit(ReturnStatement& node)
{
    if (node.getExpr()) {
        node.getExpr()->accept(*this);
    }
    // Check return type matches function signature here
}

void SemanticAnalyzer::visit(AST::ProgramNode& node)
{
    // Visit all global declarations and functions
}
