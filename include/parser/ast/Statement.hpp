#pragma once



#include <parser/ast/Node.hpp>
#include <parser/ast/Expression.hpp>
#include <string>

namespace AST {


class Visitor;
class Declaration;





// EN: Base class for statement nodes.
// FR: Classe de base pour les noeuds d instruction.
class Statement : public Node {
public:
    // EN: Virtual destructor for statements.
    // FR: Destructeur virtuel pour instructions.
    ~Statement() override = default;
    
protected:
    Statement() = default;
    Statement(int l, int c) : Node(l, c) {}
};





class CompoundStmt : public Statement {
public:
    PtrList<Declaration> declarations;  
    PtrList<Statement> statements;      
    
    // EN: Builds an empty compound statement (block).
    // FR: Construit un bloc vide.
    CompoundStmt(int l, int c) : Statement(l, c) {}
    
    // EN: Accepts a visitor for this block.
    // FR: Accepte un visiteur pour ce bloc.
    void accept(Visitor& visitor) override;
};





class ExpressionStmt : public Statement {
public:
    Ptr<Expression> expression;  
    
    // EN: Builds an expression statement (or empty if expr is null).
    // FR: Construit une instruction expression (ou vide si null).
    ExpressionStmt(Ptr<Expression> expr, int l, int c)
        : Statement(l, c), expression(std::move(expr)) {}
    
    // EN: Accepts a visitor for this expression statement.
    // FR: Accepte un visiteur pour cette instruction expression.
    void accept(Visitor& visitor) override;
};





class IfStmt : public Statement {
public:
    Ptr<Expression> condition;
    Ptr<Statement> thenBranch;
    Ptr<Statement> elseBranch;  
    
    // EN: Builds an if/else statement.
    // FR: Construit une instruction if/else.
    IfStmt(Ptr<Expression> cond, Ptr<Statement> then, 
           Ptr<Statement> els, int l, int c)
        : Statement(l, c), condition(std::move(cond)), 
          thenBranch(std::move(then)), elseBranch(std::move(els)) {}
    
    // EN: Accepts a visitor for this if statement.
    // FR: Accepte un visiteur pour ce if.
    void accept(Visitor& visitor) override;
};





class WhileStmt : public Statement {
public:
    Ptr<Expression> condition;
    Ptr<Statement> body;
    
    // EN: Builds a while loop statement.
    // FR: Construit une boucle while.
    WhileStmt(Ptr<Expression> cond, Ptr<Statement> b, int l, int c)
        : Statement(l, c), condition(std::move(cond)), body(std::move(b)) {}
    
    // EN: Accepts a visitor for this while loop.
    // FR: Accepte un visiteur pour cette boucle while.
    void accept(Visitor& visitor) override;
};





class DoWhileStmt : public Statement {
public:
    Ptr<Statement> body;
    Ptr<Expression> condition;
    
    // EN: Builds a do-while loop statement.
    // FR: Construit une boucle do-while.
    DoWhileStmt(Ptr<Statement> b, Ptr<Expression> cond, int l, int c)
        : Statement(l, c), body(std::move(b)), condition(std::move(cond)) {}
    
    // EN: Accepts a visitor for this do-while loop.
    // FR: Accepte un visiteur pour cette boucle do-while.
    void accept(Visitor& visitor) override;
};





class ForStmt : public Statement {
public:
    Ptr<Expression> init;       
    Ptr<Expression> condition;  
    Ptr<Expression> increment;  
    Ptr<Statement> body;
    
    // EN: Builds a for loop statement.
    // FR: Construit une boucle for.
    ForStmt(Ptr<Expression> i, Ptr<Expression> c, 
            Ptr<Expression> inc, Ptr<Statement> b, int l, int col)
        : Statement(l, col), init(std::move(i)), condition(std::move(c)),
          increment(std::move(inc)), body(std::move(b)) {}
    
    // EN: Accepts a visitor for this for loop.
    // FR: Accepte un visiteur pour cette boucle for.
    void accept(Visitor& visitor) override;
};





class SwitchStmt : public Statement {
public:
    Ptr<Expression> expression;
    Ptr<Statement> body;
    
    // EN: Builds a switch statement.
    // FR: Construit une instruction switch.
    SwitchStmt(Ptr<Expression> expr, Ptr<Statement> b, int l, int c)
        : Statement(l, c), expression(std::move(expr)), body(std::move(b)) {}
    
    // EN: Accepts a visitor for this switch statement.
    // FR: Accepte un visiteur pour ce switch.
    void accept(Visitor& visitor) override;
};





class CaseStmt : public Statement {
public:
    Ptr<Expression> value;
    Ptr<Statement> body;    
    
    // EN: Builds a case label statement.
    // FR: Construit un label case.
    CaseStmt(Ptr<Expression> val, Ptr<Statement> b, int l, int c)
        : Statement(l, c), value(std::move(val)), body(std::move(b)) {}
    
    // EN: Accepts a visitor for this case statement.
    // FR: Accepte un visiteur pour ce case.
    void accept(Visitor& visitor) override;
};





class DefaultStmt : public Statement {
public:
    Ptr<Statement> body;
    
    // EN: Builds a default label statement.
    // FR: Construit un label default.
    DefaultStmt(Ptr<Statement> b, int l, int c)
        : Statement(l, c), body(std::move(b)) {}
    
    // EN: Accepts a visitor for this default statement.
    // FR: Accepte un visiteur pour ce default.
    void accept(Visitor& visitor) override;
};





class BreakStmt : public Statement {
public:
    // EN: Builds a break statement.
    // FR: Construit une instruction break.
    BreakStmt(int l, int c) : Statement(l, c) {}
    
    // EN: Accepts a visitor for this break statement.
    // FR: Accepte un visiteur pour ce break.
    void accept(Visitor& visitor) override;
};





class ContinueStmt : public Statement {
public:
    // EN: Builds a continue statement.
    // FR: Construit une instruction continue.
    ContinueStmt(int l, int c) : Statement(l, c) {}
    
    // EN: Accepts a visitor for this continue statement.
    // FR: Accepte un visiteur pour ce continue.
    void accept(Visitor& visitor) override;
};





class ReturnStmt : public Statement {
public:
    Ptr<Expression> value;  
    
    // EN: Builds a return statement with an optional value.
    // FR: Construit un return avec valeur optionnelle.
    ReturnStmt(Ptr<Expression> val, int l, int c)
        : Statement(l, c), value(std::move(val)) {}
    
    // EN: Accepts a visitor for this return statement.
    // FR: Accepte un visiteur pour ce return.
    void accept(Visitor& visitor) override;
};





class GotoStmt : public Statement {
public:
    std::string label;
    
    // EN: Builds a goto statement with a target label.
    // FR: Construit un goto avec label cible.
    GotoStmt(const std::string& lbl, int l, int c)
        : Statement(l, c), label(lbl) {}
    
    // EN: Accepts a visitor for this goto statement.
    // FR: Accepte un visiteur pour ce goto.
    void accept(Visitor& visitor) override;
};





class LabelStmt : public Statement {
public:
    std::string label;
    Ptr<Statement> body;
    
    // EN: Builds a labeled statement.
    // FR: Construit une instruction etiquetee.
    LabelStmt(const std::string& lbl, Ptr<Statement> b, int l, int c)
        : Statement(l, c), label(lbl), body(std::move(b)) {}
    
    // EN: Accepts a visitor for this labeled statement.
    // FR: Accepte un visiteur pour cette instruction etiquetee.
    void accept(Visitor& visitor) override;
};

// TODO(cc1) EN: Add support for switch case ranges if language mode allows.
// FR: Ajouter le support des ranges de case si le mode de langage le permet.
} 
