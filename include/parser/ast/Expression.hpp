#pragma once



#include <parser/ast/Node.hpp>
#include <parser/ast/Type.hpp>
#include <lexer/Token.hpp>
#include <string>

namespace AST {


class Visitor;





// EN: Base class for all expression nodes, with type info resolved later.
// FR: Classe de base pour toutes les expressions, avec type resolu plus tard.
class Expression : public Node {
public:
    // EN: Semantic type resolved by later analysis.
    // FR: Type semantique resolu par l analyse.
    Ptr<Type> resolvedType;  
    // EN: Whether this expression is an lvalue.
    // FR: Indique si l expression est une lvalue.
    bool isLvalue = false;
    
    // EN: Virtual destructor for expressions.
    // FR: Destructeur virtuel pour expressions.
    ~Expression() override = default;
    
protected:
    Expression() = default;
    Expression(int l, int c) : Node(l, c) {}
};





// EN: Integer literal expression with parsed value and original text.
// FR: Litteral entier avec valeur et texte original.
class IntegerLiteral : public Expression {
public:
    long long value;
    std::string text;  
    
    // EN: Builds an integer literal node.
    // FR: Construit un noeud litteral entier.
    IntegerLiteral(long long v, const std::string& t, int l, int c)
        : Expression(l, c), value(v), text(t) {}
    
    // EN: Accepts a visitor for this literal.
    // FR: Accepte un visiteur pour ce litteral.
    void accept(Visitor& visitor) override;
};

// EN: Floating literal expression with parsed value and original text.
// FR: Litteral flottant avec valeur et texte original.
class FloatLiteral : public Expression {
public:
    double value;
    std::string text;
    bool isFloat = false;  
    
    // EN: Builds a float literal node.
    // FR: Construit un noeud litteral flottant.
    FloatLiteral(double v, const std::string& t, int l, int c)
        : Expression(l, c), value(v), text(t) {}
    
    // EN: Accepts a visitor for this literal.
    // FR: Accepte un visiteur pour ce litteral.
    void accept(Visitor& visitor) override;
};

// EN: Character literal expression with parsed value and original text.
// FR: Litteral caractere avec valeur et texte original.
class CharLiteral : public Expression {
public:
    int value;
    std::string text;
    
    // EN: Builds a char literal node.
    // FR: Construit un noeud litteral caractere.
    CharLiteral(int v, const std::string& t, int l, int c)
        : Expression(l, c), value(v), text(t) {}
    
    // EN: Accepts a visitor for this literal.
    // FR: Accepte un visiteur pour ce litteral.
    void accept(Visitor& visitor) override;
};

// EN: String literal expression with decoded value and original text.
// FR: Litteral chaine avec valeur decodee et texte original.
class StringLiteral : public Expression {
public:
    std::string value;  
    std::string text;   
    
    // EN: Builds a string literal node.
    // FR: Construit un noeud litteral chaine.
    StringLiteral(const std::string& v, const std::string& t, int l, int c)
        : Expression(l, c), value(v), text(t) {}
    
    // EN: Accepts a visitor for this literal.
    // FR: Accepte un visiteur pour ce litteral.
    void accept(Visitor& visitor) override;
};





// EN: Identifier expression referencing a named symbol.
// FR: Expression identifiant qui reference un symbole nomme.
class Identifier : public Expression {
public:
    std::string name;
    
    // EN: Builds an identifier node and marks it as lvalue.
    // FR: Construit un identifiant et le marque lvalue.
    Identifier(const std::string& n, int l, int c)
        : Expression(l, c), name(n) { isLvalue = true; }
    
    // EN: Accepts a visitor for this identifier.
    // FR: Accepte un visiteur pour cet identifiant.
    void accept(Visitor& visitor) override;
};





// EN: Binary operator kinds used in expressions.
// FR: Types d operateurs binaires utilises dans les expressions.
enum class BinaryOp {
    
    Add, Sub, Mul, Div, Mod,
    
    Equal, NotEqual, Less, LessEqual, Greater, GreaterEqual,
    
    LogicalAnd, LogicalOr,
    
    BitwiseAnd, BitwiseOr, BitwiseXor, LeftShift, RightShift,
    
    Assign,
    AddAssign, SubAssign, MulAssign, DivAssign, ModAssign,
    AndAssign, OrAssign, XorAssign, LeftShiftAssign, RightShiftAssign,
    
    Comma
};

// EN: Binary expression with operator and operands.
// FR: Expression binaire avec operateur et operands.
class BinaryExpr : public Expression {
public:
    BinaryOp op;
    Ptr<Expression> left;
    Ptr<Expression> right;
    
    // EN: Builds a binary expression node.
    // FR: Construit un noeud d expression binaire.
    BinaryExpr(BinaryOp o, Ptr<Expression> l, Ptr<Expression> r, int line, int col)
        : Expression(line, col), op(o), left(std::move(l)), right(std::move(r)) {}
    
    // EN: Accepts a visitor for this binary expression.
    // FR: Accepte un visiteur pour cette expression binaire.
    void accept(Visitor& visitor) override;
};





// EN: Unary operator kinds used in expressions.
// FR: Types d operateurs unaires utilises dans les expressions.
enum class UnaryOp {
    
    Plus, Negate, LogicalNot, BitwiseNot,
    PreIncrement, PreDecrement,
    AddressOf, Dereference,
    
    PostIncrement, PostDecrement
};

// EN: Unary expression with prefix/postfix operator.
// FR: Expression unaire avec operateur prefixe/postfixe.
class UnaryExpr : public Expression {
public:
    UnaryOp op;
    Ptr<Expression> operand;
    bool isPrefix;
    
    // EN: Builds a unary expression node.
    // FR: Construit un noeud d expression unaire.
    UnaryExpr(UnaryOp o, Ptr<Expression> expr, bool prefix, int l, int c)
        : Expression(l, c), op(o), operand(std::move(expr)), isPrefix(prefix) {}
    
    // EN: Accepts a visitor for this unary expression.
    // FR: Accepte un visiteur pour cette expression unaire.
    void accept(Visitor& visitor) override;
};





// EN: Cast expression to a target type.
// FR: Expression de cast vers un type cible.
class CastExpr : public Expression {
public:
    Ptr<Type> targetType;
    Ptr<Expression> operand;
    
    // EN: Builds a cast expression node.
    // FR: Construit un noeud d expression de cast.
    CastExpr(Ptr<Type> type, Ptr<Expression> expr, int l, int c)
        : Expression(l, c), targetType(std::move(type)), operand(std::move(expr)) {}
    
    // EN: Accepts a visitor for this cast expression.
    // FR: Accepte un visiteur pour cette expression de cast.
    void accept(Visitor& visitor) override;
};





// EN: sizeof expression on a type or expression.
// FR: Expression sizeof sur un type ou une expression.
class SizeofExpr : public Expression {
public:
    Ptr<Type> targetType;      
    Ptr<Expression> operand;   
    
    
    // EN: Builds a sizeof(type) expression.
    // FR: Construit un sizeof(type).
    SizeofExpr(Ptr<Type> type, int l, int c)
        : Expression(l, c), targetType(std::move(type)) {}
    
    // EN: Builds a sizeof(expr) expression.
    // FR: Construit un sizeof(expr).
    SizeofExpr(Ptr<Expression> expr, int l, int c)
        : Expression(l, c), operand(std::move(expr)) {}
    
    // EN: Accepts a visitor for this sizeof expression.
    // FR: Accepte un visiteur pour ce sizeof.
    void accept(Visitor& visitor) override;
};





// EN: Function call expression.
// FR: Expression d appel de fonction.
class CallExpr : public Expression {
public:
    Ptr<Expression> callee;
    PtrList<Expression> arguments;
    
    // EN: Builds a call expression with a callee.
    // FR: Construit un appel avec callee.
    CallExpr(Ptr<Expression> func, int l, int c)
        : Expression(l, c), callee(std::move(func)) {}
    
    // EN: Accepts a visitor for this call expression.
    // FR: Accepte un visiteur pour cet appel.
    void accept(Visitor& visitor) override;
};





// EN: Member access expression (obj.member or obj->member).
// FR: Expression d acces membre (obj.member ou obj->member).
class MemberExpr : public Expression {
public:
    Ptr<Expression> object;
    std::string member;
    bool isArrow;  
    
    // EN: Builds a member access expression and marks it as lvalue.
    // FR: Construit un acces membre et le marque lvalue.
    MemberExpr(Ptr<Expression> obj, const std::string& mem, bool arrow, int l, int c)
        : Expression(l, c), object(std::move(obj)), member(mem), isArrow(arrow) {
        isLvalue = true;
    }
    
    // EN: Accepts a visitor for this member expression.
    // FR: Accepte un visiteur pour cette expression membre.
    void accept(Visitor& visitor) override;
};





// EN: Array indexing expression (array[index]).
// FR: Expression d indexation (array[index]).
class IndexExpr : public Expression {
public:
    Ptr<Expression> array;
    Ptr<Expression> index;
    
    // EN: Builds an index expression and marks it as lvalue.
    // FR: Construit une indexation et la marque lvalue.
    IndexExpr(Ptr<Expression> arr, Ptr<Expression> idx, int l, int c)
        : Expression(l, c), array(std::move(arr)), index(std::move(idx)) {
        isLvalue = true;
    }
    
    // EN: Accepts a visitor for this index expression.
    // FR: Accepte un visiteur pour cette expression d index.
    void accept(Visitor& visitor) override;
};





// EN: Ternary conditional expression (cond ? then : else).
// FR: Expression conditionnelle ternaire (cond ? then : else).
class TernaryExpr : public Expression {
public:
    Ptr<Expression> condition;
    Ptr<Expression> thenExpr;
    Ptr<Expression> elseExpr;
    
    // EN: Builds a ternary expression node.
    // FR: Construit un noeud d expression ternaire.
    TernaryExpr(Ptr<Expression> cond, Ptr<Expression> then, 
                Ptr<Expression> els, int l, int c)
        : Expression(l, c), condition(std::move(cond)), 
          thenExpr(std::move(then)), elseExpr(std::move(els)) {}
    
    // EN: Accepts a visitor for this ternary expression.
    // FR: Accepte un visiteur pour cette expression ternaire.
    void accept(Visitor& visitor) override;
};





// EN: Brace-enclosed initializer list expression.
// FR: Liste d initialisation entre accolades.
class InitializerList : public Expression {
public:
    PtrList<Expression> initializers;
    
    // EN: Builds an empty initializer list node.
    // FR: Construit une liste d initialisation vide.
    InitializerList(int l, int c) : Expression(l, c) {}
    
    // EN: Accepts a visitor for this initializer list.
    // FR: Accepte un visiteur pour cette liste d initialisation.
    void accept(Visitor& visitor) override;
};

// TODO(cc1) EN: Add missing expression nodes (compound literals, designators).
// FR: Ajouter les noeuds d expression manquants (litteraux composes, designateurs).
} 
