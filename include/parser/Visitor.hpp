#pragma once




class BinaryOpExpr;
class ReturnStatement;
namespace AST {
    class ProgramNode;
}

// EN: Visitor interface for the minimal AST node set in this header.
// FR: Interface visiteur pour l ensemble minimal de noeuds AST ici.
class Visitor {
    public:
        // EN: Virtual destructor for polymorphic cleanup.
        // FR: Destructeur virtuel pour nettoyage polymorphique.
        virtual ~Visitor() = default;

        
        // EN: Visits a binary operation expression node.
        // FR: Visite un noeud d expression binaire.
        virtual void visit(BinaryOpExpr& node) = 0;
        // EN: Visits a return statement node.
        // FR: Visite un noeud d instruction return.
        virtual void visit(ReturnStatement& node) = 0;
        // EN: Visits the program root node.
        // FR: Visite le noeud racine du programme.
        virtual void visit(AST::ProgramNode& node) = 0;
};

// TODO(cc1) EN: Unify this visitor with parser/ast/Visitor for a single API.
// FR: Unifier ce visiteur avec parser/ast/Visitor pour une seule API.
