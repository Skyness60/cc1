#pragma once



#include <memory>
#include <parser/Visitor.hpp>
#include <lexer/Token.hpp>

// EN: Base class for all AST nodes with visitor support.
// FR: Classe de base pour tous les noeuds AST avec support visiteur.
class ASTNode 
{
public:
    // EN: Accepts a visitor to process this node.
    // FR: Accepte un visiteur pour traiter ce noeud.
    virtual void accept(Visitor& v) = 0;
    // EN: Virtual destructor for polymorphic cleanup.
    // FR: Destructeur virtuel pour nettoyage polymorphique.
    virtual ~ASTNode() = default;
};class Expression : public ASTNode 
{
    public:
        // EN: Virtual destructor for expression nodes.
        // FR: Destructeur virtuel pour noeuds d expression.
        ~Expression() override = default;
};

// EN: Binary operation expression (left op right).
// FR: Expression binaire (gauche op droite).
class BinaryOpExpr : public Expression 
{
    private:
        std::unique_ptr<Expression> left_;
        std::unique_ptr<Expression> right_;
        TokenType op_;
    public:
        // EN: Constructs a binary expression with owned operands and operator.
        // FR: Construit une expression binaire avec operands possedes et operateur.
        BinaryOpExpr(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, TokenType op)
            : left_(std::move(left)), right_(std::move(right)), op_(op) {}

        // EN: Returns the left operand expression.
        // FR: Renvoie l operande gauche.
        Expression* getLeft() const { return left_.get(); }
        // EN: Returns the right operand expression.
        // FR: Renvoie l operande droite.
        Expression* getRight() const { return right_.get(); }
        // EN: Returns the operator token type.
        // FR: Renvoie le type de token operateur.
        TokenType getOp() const { return op_; }

        // EN: Accepts a visitor for this binary expression.
        // FR: Accepte un visiteur pour cette expression binaire.
        void accept(Visitor& v) override;
};

// EN: Base class for statement nodes.
// FR: Classe de base pour noeuds d instruction.
class Statement : public ASTNode
{
    public:
        // EN: Virtual destructor for statement nodes.
        // FR: Destructeur virtuel pour noeuds d instruction.
        ~Statement() override = default;
};

// EN: Return statement with an optional expression.
// FR: Instruction return avec expression optionnelle.
class ReturnStatement : public Statement {
    private:
        std::unique_ptr<Expression> expr_;
    public:
        // EN: Constructs a return statement with an owned expression.
        // FR: Construit un return avec expression possedee.
        ReturnStatement(std::unique_ptr<Expression> expr) : expr_(std::move(expr)) {}

        // EN: Returns the return expression, if any.
        // FR: Renvoie l expression de retour, si presente.
        Expression* getExpr() const { return expr_.get(); }

        // EN: Accepts a visitor for this return statement.
        // FR: Accepte un visiteur pour ce return.
        void accept(Visitor& v) override;
};

namespace AST {
    // EN: Root program node for a translation unit.
    // FR: Noeud racine du programme pour une unite de traduction.
    class ProgramNode : public ASTNode {
        public:
            // EN: Accepts a visitor for the program node.
            // FR: Accepte un visiteur pour le noeud programme.
            void accept(Visitor& v) override;
            // EN: Virtual destructor for cleanup.
            // FR: Destructeur virtuel pour nettoyage.
            ~ProgramNode() override = default;
    };
}

// TODO(cc1) EN: Normalize formatting and split AST classes into dedicated headers.
// FR: Normaliser le formatage et separer les classes AST dans des headers dedies.
