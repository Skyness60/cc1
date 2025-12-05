#pragma once

#include <parser/ast/Expression.hpp>
#include <parser/ast/Statement.hpp>
#include <parser/ast/Declaration.hpp>

namespace AST {

// ============================================================================
// AST Visitor Interface
// ============================================================================

class Visitor {
public:
    virtual ~Visitor() = default;
    
    // Expressions
    virtual void visit(IntegerLiteral& node) = 0;
    virtual void visit(FloatLiteral& node) = 0;
    virtual void visit(CharLiteral& node) = 0;
    virtual void visit(StringLiteral& node) = 0;
    virtual void visit(Identifier& node) = 0;
    virtual void visit(BinaryExpr& node) = 0;
    virtual void visit(UnaryExpr& node) = 0;
    virtual void visit(CastExpr& node) = 0;
    virtual void visit(SizeofExpr& node) = 0;
    virtual void visit(CallExpr& node) = 0;
    virtual void visit(MemberExpr& node) = 0;
    virtual void visit(IndexExpr& node) = 0;
    virtual void visit(TernaryExpr& node) = 0;
    virtual void visit(InitializerList& node) = 0;
    
    // Statements
    virtual void visit(CompoundStmt& node) = 0;
    virtual void visit(ExpressionStmt& node) = 0;
    virtual void visit(IfStmt& node) = 0;
    virtual void visit(WhileStmt& node) = 0;
    virtual void visit(DoWhileStmt& node) = 0;
    virtual void visit(ForStmt& node) = 0;
    virtual void visit(SwitchStmt& node) = 0;
    virtual void visit(CaseStmt& node) = 0;
    virtual void visit(DefaultStmt& node) = 0;
    virtual void visit(BreakStmt& node) = 0;
    virtual void visit(ContinueStmt& node) = 0;
    virtual void visit(ReturnStmt& node) = 0;
    virtual void visit(GotoStmt& node) = 0;
    virtual void visit(LabelStmt& node) = 0;
    
    // Declarations
    virtual void visit(VarDecl& node) = 0;
    virtual void visit(ParamDecl& node) = 0;
    virtual void visit(FunctionDecl& node) = 0;
    virtual void visit(StructDecl& node) = 0;
    virtual void visit(EnumDecl& node) = 0;
    virtual void visit(TypedefDecl& node) = 0;
    virtual void visit(TranslationUnit& node) = 0;
};

// ============================================================================
// Default Visitor (does nothing, for convenience)
// ============================================================================

class DefaultVisitor : public Visitor {
public:
    void visit(IntegerLiteral&) override {}
    void visit(FloatLiteral&) override {}
    void visit(CharLiteral&) override {}
    void visit(StringLiteral&) override {}
    void visit(Identifier&) override {}
    void visit(BinaryExpr&) override {}
    void visit(UnaryExpr&) override {}
    void visit(CastExpr&) override {}
    void visit(SizeofExpr&) override {}
    void visit(CallExpr&) override {}
    void visit(MemberExpr&) override {}
    void visit(IndexExpr&) override {}
    void visit(TernaryExpr&) override {}
    void visit(InitializerList&) override {}
    
    void visit(CompoundStmt&) override {}
    void visit(ExpressionStmt&) override {}
    void visit(IfStmt&) override {}
    void visit(WhileStmt&) override {}
    void visit(DoWhileStmt&) override {}
    void visit(ForStmt&) override {}
    void visit(SwitchStmt&) override {}
    void visit(CaseStmt&) override {}
    void visit(DefaultStmt&) override {}
    void visit(BreakStmt&) override {}
    void visit(ContinueStmt&) override {}
    void visit(ReturnStmt&) override {}
    void visit(GotoStmt&) override {}
    void visit(LabelStmt&) override {}
    
    void visit(VarDecl&) override {}
    void visit(ParamDecl&) override {}
    void visit(FunctionDecl&) override {}
    void visit(StructDecl&) override {}
    void visit(EnumDecl&) override {}
    void visit(TypedefDecl&) override {}
    void visit(TranslationUnit&) override {}
};

} // namespace AST
