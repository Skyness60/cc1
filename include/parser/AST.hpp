#pragma once

#include <memory>
#include <string>
#include <parser/Visitor.hpp>
#include <lexer/Token.hpp>

class ASTNode 
{
public:
    virtual void accept(Visitor& v) = 0;
    virtual ~ASTNode() = default;
};class Expression : public ASTNode 
{
    public:
        ~Expression() override = default;
};

class BinaryOpExpr : public Expression 
{
    private:
        std::unique_ptr<Expression> left_;
        std::unique_ptr<Expression> right_;
        TokenType op_;
    public:
        BinaryOpExpr(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, TokenType op)
            : left_(std::move(left)), right_(std::move(right)), op_(op) {}

        Expression* getLeft() const { return left_.get(); }
        Expression* getRight() const { return right_.get(); }
        TokenType getOp() const { return op_; }

        void accept(Visitor& v) override;
};

class Statement : public ASTNode
{
    public:
        ~Statement() override = default;
};

class ReturnStatement : public Statement {
    private:
        std::unique_ptr<Expression> expr_;
    public:
        ReturnStatement(std::unique_ptr<Expression> expr) : expr_(std::move(expr)) {}

        Expression* getExpr() const { return expr_.get(); }

        void accept(Visitor& v) override;
};

namespace AST {
    class ProgramNode : public ASTNode {
        public:
            void accept(Visitor& v) override;
            ~ProgramNode() override = default;
    };
}

