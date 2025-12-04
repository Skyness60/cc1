#pragma once

// Forward declarations of AST nodes
class BinaryOpExpr;
class ReturnStatement;
namespace AST {
    class ProgramNode;
}

class Visitor {
    public:
        virtual ~Visitor() = default;

        // Visit methods for each concrete AST node type
        virtual void visit(BinaryOpExpr& node) = 0;
        virtual void visit(ReturnStatement& node) = 0;
        virtual void visit(AST::ProgramNode& node) = 0;
};
