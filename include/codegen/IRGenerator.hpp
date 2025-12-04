#pragma once

#include <parser/Visitor.hpp>
#include <string>
#include <sstream>

class IRGenerator : public Visitor {
    private:
        std::stringstream buffer_;

    public:
        void visit(BinaryOpExpr& node) override;
        void visit(ReturnStatement& node) override;
        void visit(AST::ProgramNode& node) override;

        std::string getIR() const { return buffer_.str(); }
};
