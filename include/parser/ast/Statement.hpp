#pragma once

#include <parser/ast/Node.hpp>
#include <parser/ast/Expression.hpp>
#include <string>

namespace AST {

// Forward declarations
class Visitor;
class Declaration;

// ============================================================================
// Statement Base Class
// ============================================================================

class Statement : public Node {
public:
    ~Statement() override = default;
    
protected:
    Statement() = default;
    Statement(int l, int c) : Node(l, c) {}
};

// ============================================================================
// Compound Statement (Block)
// ============================================================================

class CompoundStmt : public Statement {
public:
    PtrList<Declaration> declarations;  // C89: declarations first
    PtrList<Statement> statements;      // Then statements
    
    CompoundStmt(int l, int c) : Statement(l, c) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Expression Statement
// ============================================================================

class ExpressionStmt : public Statement {
public:
    Ptr<Expression> expression;  // nullptr for empty statement ';'
    
    ExpressionStmt(Ptr<Expression> expr, int l, int c)
        : Statement(l, c), expression(std::move(expr)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// If Statement
// ============================================================================

class IfStmt : public Statement {
public:
    Ptr<Expression> condition;
    Ptr<Statement> thenBranch;
    Ptr<Statement> elseBranch;  // nullptr if no else
    
    IfStmt(Ptr<Expression> cond, Ptr<Statement> then, 
           Ptr<Statement> els, int l, int c)
        : Statement(l, c), condition(std::move(cond)), 
          thenBranch(std::move(then)), elseBranch(std::move(els)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// While Statement
// ============================================================================

class WhileStmt : public Statement {
public:
    Ptr<Expression> condition;
    Ptr<Statement> body;
    
    WhileStmt(Ptr<Expression> cond, Ptr<Statement> b, int l, int c)
        : Statement(l, c), condition(std::move(cond)), body(std::move(b)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Do-While Statement
// ============================================================================

class DoWhileStmt : public Statement {
public:
    Ptr<Statement> body;
    Ptr<Expression> condition;
    
    DoWhileStmt(Ptr<Statement> b, Ptr<Expression> cond, int l, int c)
        : Statement(l, c), body(std::move(b)), condition(std::move(cond)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// For Statement
// ============================================================================

class ForStmt : public Statement {
public:
    Ptr<Expression> init;       // nullptr if omitted (C89: no declarations in init)
    Ptr<Expression> condition;  // nullptr if omitted
    Ptr<Expression> increment;  // nullptr if omitted
    Ptr<Statement> body;
    
    ForStmt(Ptr<Expression> i, Ptr<Expression> c, 
            Ptr<Expression> inc, Ptr<Statement> b, int l, int col)
        : Statement(l, col), init(std::move(i)), condition(std::move(c)),
          increment(std::move(inc)), body(std::move(b)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Switch Statement
// ============================================================================

class SwitchStmt : public Statement {
public:
    Ptr<Expression> expression;
    Ptr<Statement> body;
    
    SwitchStmt(Ptr<Expression> expr, Ptr<Statement> b, int l, int c)
        : Statement(l, c), expression(std::move(expr)), body(std::move(b)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Case Statement
// ============================================================================

class CaseStmt : public Statement {
public:
    Ptr<Expression> value;
    Ptr<Statement> body;    // The statement following the case
    
    CaseStmt(Ptr<Expression> val, Ptr<Statement> b, int l, int c)
        : Statement(l, c), value(std::move(val)), body(std::move(b)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Default Statement
// ============================================================================

class DefaultStmt : public Statement {
public:
    Ptr<Statement> body;
    
    DefaultStmt(Ptr<Statement> b, int l, int c)
        : Statement(l, c), body(std::move(b)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Break Statement
// ============================================================================

class BreakStmt : public Statement {
public:
    BreakStmt(int l, int c) : Statement(l, c) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Continue Statement
// ============================================================================

class ContinueStmt : public Statement {
public:
    ContinueStmt(int l, int c) : Statement(l, c) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Return Statement
// ============================================================================

class ReturnStmt : public Statement {
public:
    Ptr<Expression> value;  // nullptr for return without value
    
    ReturnStmt(Ptr<Expression> val, int l, int c)
        : Statement(l, c), value(std::move(val)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Goto Statement
// ============================================================================

class GotoStmt : public Statement {
public:
    std::string label;
    
    GotoStmt(const std::string& lbl, int l, int c)
        : Statement(l, c), label(lbl) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Label Statement
// ============================================================================

class LabelStmt : public Statement {
public:
    std::string label;
    Ptr<Statement> body;
    
    LabelStmt(const std::string& lbl, Ptr<Statement> b, int l, int c)
        : Statement(l, c), label(lbl), body(std::move(b)) {}
    
    void accept(Visitor& visitor) override;
};

} // namespace AST
