#include <codegen/IRGenerator.hpp>
#include <parser/AST.hpp>

void IRGenerator::visit(BinaryOpExpr& node)
{
    // Visit left child
    if (node.getLeft()) {
        node.getLeft()->accept(*this);
    }
    
    // Visit right child
    if (node.getRight()) {
        node.getRight()->accept(*this);
    }

    // Generate IR for binary op (placeholder)
    buffer_ << "  %temp = add i32 ...\n";
}

void IRGenerator::visit(ReturnStatement& node)
{
    // Visit expression
    if (node.getExpr()) {
        node.getExpr()->accept(*this);
    }

    // Generate IR for return (placeholder)
    buffer_ << "  ret i32 0\n";
}

void IRGenerator::visit(AST::ProgramNode&)
{
    buffer_ << "define i32 @main() {\n";
    // In a real implementation, ProgramNode would have a list of statements/functions
    // For now, we just assume it might have children to visit, but ProgramNode definition is empty in AST.hpp
    // So we just print the footer
    buffer_ << "}\n";
}
