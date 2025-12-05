#pragma once

#include <parser/ast/Node.hpp>
#include <parser/ast/Type.hpp>
#include <lexer/Token.hpp>
#include <string>

namespace AST {

// Forward declarations
class Visitor;

// ============================================================================
// Expression Base Class
// ============================================================================

class Expression : public Node {
public:
    Ptr<Type> resolvedType;  // Set during semantic analysis
    bool isLvalue = false;
    
    ~Expression() override = default;
    
protected:
    Expression() = default;
    Expression(int l, int c) : Node(l, c) {}
};

// ============================================================================
// Literals
// ============================================================================

class IntegerLiteral : public Expression {
public:
    long long value;
    std::string text;  // Original text for suffix info (e.g., "42UL")
    
    IntegerLiteral(long long v, const std::string& t, int l, int c)
        : Expression(l, c), value(v), text(t) {}
    
    void accept(Visitor& visitor) override;
};

class FloatLiteral : public Expression {
public:
    double value;
    std::string text;
    bool isFloat = false;  // true for 'f' suffix
    
    FloatLiteral(double v, const std::string& t, int l, int c)
        : Expression(l, c), value(v), text(t) {}
    
    void accept(Visitor& visitor) override;
};

class CharLiteral : public Expression {
public:
    int value;
    std::string text;
    
    CharLiteral(int v, const std::string& t, int l, int c)
        : Expression(l, c), value(v), text(t) {}
    
    void accept(Visitor& visitor) override;
};

class StringLiteral : public Expression {
public:
    std::string value;  // Decoded string
    std::string text;   // Original text with quotes
    
    StringLiteral(const std::string& v, const std::string& t, int l, int c)
        : Expression(l, c), value(v), text(t) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Identifier
// ============================================================================

class Identifier : public Expression {
public:
    std::string name;
    
    Identifier(const std::string& n, int l, int c)
        : Expression(l, c), name(n) { isLvalue = true; }
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Binary Expression
// ============================================================================

enum class BinaryOp {
    // Arithmetic
    Add, Sub, Mul, Div, Mod,
    // Comparison
    Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual,
    // Logical
    LogicalAnd, LogicalOr,
    // Bitwise
    BitwiseAnd, BitwiseOr, BitwiseXor, LeftShift, RightShift,
    // Assignment
    Assign,
    AddAssign, SubAssign, MulAssign, DivAssign, ModAssign,
    AndAssign, OrAssign, XorAssign, LeftShiftAssign, RightShiftAssign,
    // Comma
    Comma
};

class BinaryExpr : public Expression {
public:
    BinaryOp op;
    Ptr<Expression> left;
    Ptr<Expression> right;
    
    BinaryExpr(BinaryOp o, Ptr<Expression> l, Ptr<Expression> r, int line, int col)
        : Expression(line, col), op(o), left(std::move(l)), right(std::move(r)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Unary Expression
// ============================================================================

enum class UnaryOp {
    // Prefix
    Plus, Negate, LogicalNot, BitwiseNot,
    PreIncrement, PreDecrement,
    AddressOf, Dereference,
    // Postfix
    PostIncrement, PostDecrement
};

class UnaryExpr : public Expression {
public:
    UnaryOp op;
    Ptr<Expression> operand;
    bool isPrefix;
    
    UnaryExpr(UnaryOp o, Ptr<Expression> expr, bool prefix, int l, int c)
        : Expression(l, c), op(o), operand(std::move(expr)), isPrefix(prefix) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Cast Expression
// ============================================================================

class CastExpr : public Expression {
public:
    Ptr<Type> targetType;
    Ptr<Expression> operand;
    
    CastExpr(Ptr<Type> type, Ptr<Expression> expr, int l, int c)
        : Expression(l, c), targetType(std::move(type)), operand(std::move(expr)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Sizeof Expression
// ============================================================================

class SizeofExpr : public Expression {
public:
    Ptr<Type> targetType;      // sizeof(type)
    Ptr<Expression> operand;   // sizeof expr
    
    // Only one should be set
    SizeofExpr(Ptr<Type> type, int l, int c)
        : Expression(l, c), targetType(std::move(type)) {}
    
    SizeofExpr(Ptr<Expression> expr, int l, int c)
        : Expression(l, c), operand(std::move(expr)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Call Expression
// ============================================================================

class CallExpr : public Expression {
public:
    Ptr<Expression> callee;
    PtrList<Expression> arguments;
    
    CallExpr(Ptr<Expression> func, int l, int c)
        : Expression(l, c), callee(std::move(func)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Member Access Expression (. and ->)
// ============================================================================

class MemberExpr : public Expression {
public:
    Ptr<Expression> object;
    std::string member;
    bool isArrow;  // true for ->, false for .
    
    MemberExpr(Ptr<Expression> obj, const std::string& mem, bool arrow, int l, int c)
        : Expression(l, c), object(std::move(obj)), member(mem), isArrow(arrow) {
        isLvalue = true;
    }
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Array Subscript Expression
// ============================================================================

class IndexExpr : public Expression {
public:
    Ptr<Expression> array;
    Ptr<Expression> index;
    
    IndexExpr(Ptr<Expression> arr, Ptr<Expression> idx, int l, int c)
        : Expression(l, c), array(std::move(arr)), index(std::move(idx)) {
        isLvalue = true;
    }
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Conditional Expression (ternary)
// ============================================================================

class TernaryExpr : public Expression {
public:
    Ptr<Expression> condition;
    Ptr<Expression> thenExpr;
    Ptr<Expression> elseExpr;
    
    TernaryExpr(Ptr<Expression> cond, Ptr<Expression> then, 
                Ptr<Expression> els, int l, int c)
        : Expression(l, c), condition(std::move(cond)), 
          thenExpr(std::move(then)), elseExpr(std::move(els)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Initializer List Expression
// ============================================================================

class InitializerList : public Expression {
public:
    PtrList<Expression> initializers;
    
    InitializerList(int l, int c) : Expression(l, c) {}
    
    void accept(Visitor& visitor) override;
};

} // namespace AST
