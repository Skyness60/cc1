#pragma once

#include <parser/Visitor.hpp>

class SemanticAnalyzer : public Visitor {
    public:
        void visit(BinaryOpExpr& node) override;
        void visit(ReturnStatement& node) override;
        void visit(AST::ProgramNode& node) override;
};
