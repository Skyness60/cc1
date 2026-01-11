#pragma once

#include <semantics/SemanticAnalyzer.hpp>

#include <cstdint>

namespace cc1 {

struct SemanticAnalyzer::ConstExprIsConstVisitor : AST::DefaultVisitor {
    SemanticAnalyzer& sema;
    bool result = false;

    explicit ConstExprIsConstVisitor(SemanticAnalyzer& s);

    void visit(AST::IntegerLiteral&) override;
    void visit(AST::CharLiteral&) override;
    void visit(AST::FloatLiteral&) override;
    void visit(AST::SizeofExpr&) override;
    void visit(AST::Identifier& node) override;
    void visit(AST::CastExpr& node) override;
    void visit(AST::UnaryExpr& node) override;
    void visit(AST::BinaryExpr& node) override;
    void visit(AST::TernaryExpr& node) override;
};

struct SemanticAnalyzer::ConstExprEvalVisitor : AST::DefaultVisitor {
    SemanticAnalyzer& sema;
    long long& result;
    bool ok = false;

    explicit ConstExprEvalVisitor(SemanticAnalyzer& s, long long& out);

    int getPrimitiveSize(AST::PrimitiveKind kind) const;
    int getTypeAlign(AST::Type* type);
    long long getTypeSize(AST::Type* type);

    static uint64_t maskUnsigned(uint64_t value, int bits);
    int widthBitsForPrim(AST::PrimitiveKind kind) const;
    int inferIntWidthBits(AST::Expression* e);

    void visit(AST::IntegerLiteral& node) override;
    void visit(AST::CharLiteral& node) override;
    void visit(AST::SizeofExpr& node) override;
    void visit(AST::Identifier& node) override;
    void visit(AST::UnaryExpr& node) override;
    void visit(AST::BinaryExpr& node) override;
    void visit(AST::TernaryExpr& node) override;
    void visit(AST::CastExpr& node) override;
};

struct SemanticAnalyzer::ConstExprDivZeroVisitor : AST::DefaultVisitor {
    SemanticAnalyzer& sema;
    bool found = false;

    explicit ConstExprDivZeroVisitor(SemanticAnalyzer& s);

    void visit(AST::BinaryExpr& node) override;
    void visit(AST::UnaryExpr& node) override;
    void visit(AST::TernaryExpr& node) override;
};

} // namespace cc1
