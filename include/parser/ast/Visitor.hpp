#pragma once



#include <parser/ast/Expression.hpp>
#include <parser/ast/Statement.hpp>
#include <parser/ast/Declaration.hpp>

namespace AST {





// EN: Visitor interface for all AST node types.
// FR: Interface visiteur pour tous les types de noeuds AST.
class Visitor {
public:
    // EN: Virtual destructor for polymorphic cleanup.
    // FR: Destructeur virtuel pour nettoyage polymorphique.
    virtual ~Visitor() = default;
    
    
    // EN: Visits integer literal nodes.
    // FR: Visite les noeuds litteral entier.
    virtual void visit(IntegerLiteral& node) = 0;
    // EN: Visits float literal nodes.
    // FR: Visite les noeuds litteral flottant.
    virtual void visit(FloatLiteral& node) = 0;
    // EN: Visits char literal nodes.
    // FR: Visite les noeuds litteral caractere.
    virtual void visit(CharLiteral& node) = 0;
    // EN: Visits string literal nodes.
    // FR: Visite les noeuds litteral chaine.
    virtual void visit(StringLiteral& node) = 0;
    // EN: Visits identifier nodes.
    // FR: Visite les noeuds identifiant.
    virtual void visit(Identifier& node) = 0;
    // EN: Visits binary expression nodes.
    // FR: Visite les noeuds d expression binaire.
    virtual void visit(BinaryExpr& node) = 0;
    // EN: Visits unary expression nodes.
    // FR: Visite les noeuds d expression unaire.
    virtual void visit(UnaryExpr& node) = 0;
    // EN: Visits cast expression nodes.
    // FR: Visite les noeuds d expression de cast.
    virtual void visit(CastExpr& node) = 0;
    // EN: Visits sizeof expression nodes.
    // FR: Visite les noeuds d expression sizeof.
    virtual void visit(SizeofExpr& node) = 0;
    // EN: Visits call expression nodes.
    // FR: Visite les noeuds d appel.
    virtual void visit(CallExpr& node) = 0;
    // EN: Visits member access expression nodes.
    // FR: Visite les noeuds d acces membre.
    virtual void visit(MemberExpr& node) = 0;
    // EN: Visits index expression nodes.
    // FR: Visite les noeuds d indexation.
    virtual void visit(IndexExpr& node) = 0;
    // EN: Visits ternary expression nodes.
    // FR: Visite les noeuds d expression ternaire.
    virtual void visit(TernaryExpr& node) = 0;
    // EN: Visits initializer list nodes.
    // FR: Visite les noeuds de liste d initialisation.
    virtual void visit(InitializerList& node) = 0;
    
    
    // EN: Visits compound statement nodes.
    // FR: Visite les noeuds d instruction composee.
    virtual void visit(CompoundStmt& node) = 0;
    // EN: Visits expression statement nodes.
    // FR: Visite les noeuds d instruction expression.
    virtual void visit(ExpressionStmt& node) = 0;
    // EN: Visits if statement nodes.
    // FR: Visite les noeuds d instruction if.
    virtual void visit(IfStmt& node) = 0;
    // EN: Visits while statement nodes.
    // FR: Visite les noeuds d instruction while.
    virtual void visit(WhileStmt& node) = 0;
    // EN: Visits do-while statement nodes.
    // FR: Visite les noeuds d instruction do-while.
    virtual void visit(DoWhileStmt& node) = 0;
    // EN: Visits for statement nodes.
    // FR: Visite les noeuds d instruction for.
    virtual void visit(ForStmt& node) = 0;
    // EN: Visits switch statement nodes.
    // FR: Visite les noeuds d instruction switch.
    virtual void visit(SwitchStmt& node) = 0;
    // EN: Visits case statement nodes.
    // FR: Visite les noeuds d instruction case.
    virtual void visit(CaseStmt& node) = 0;
    // EN: Visits default statement nodes.
    // FR: Visite les noeuds d instruction default.
    virtual void visit(DefaultStmt& node) = 0;
    // EN: Visits break statement nodes.
    // FR: Visite les noeuds d instruction break.
    virtual void visit(BreakStmt& node) = 0;
    // EN: Visits continue statement nodes.
    // FR: Visite les noeuds d instruction continue.
    virtual void visit(ContinueStmt& node) = 0;
    // EN: Visits return statement nodes.
    // FR: Visite les noeuds d instruction return.
    virtual void visit(ReturnStmt& node) = 0;
    // EN: Visits goto statement nodes.
    // FR: Visite les noeuds d instruction goto.
    virtual void visit(GotoStmt& node) = 0;
    // EN: Visits label statement nodes.
    // FR: Visite les noeuds d instruction etiquetee.
    virtual void visit(LabelStmt& node) = 0;
    
    
    // EN: Visits variable declaration nodes.
    // FR: Visite les noeuds de declaration de variable.
    virtual void visit(VarDecl& node) = 0;
    // EN: Visits parameter declaration nodes.
    // FR: Visite les noeuds de declaration de parametre.
    virtual void visit(ParamDecl& node) = 0;
    // EN: Visits function declaration nodes.
    // FR: Visite les noeuds de declaration de fonction.
    virtual void visit(FunctionDecl& node) = 0;
    // EN: Visits struct/union declaration nodes.
    // FR: Visite les noeuds de declaration struct/union.
    virtual void visit(StructDecl& node) = 0;
    // EN: Visits enum declaration nodes.
    // FR: Visite les noeuds de declaration enum.
    virtual void visit(EnumDecl& node) = 0;
    // EN: Visits typedef declaration nodes.
    // FR: Visite les noeuds de declaration typedef.
    virtual void visit(TypedefDecl& node) = 0;
    // EN: Visits translation unit nodes.
    // FR: Visite les noeuds d unite de traduction.
    virtual void visit(TranslationUnit& node) = 0;
};





// EN: Default visitor with no-op implementations for all nodes.
// FR: Visiteur par defaut avec implementations vides.
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

// TODO(cc1) EN: Add const-visit overloads for immutable traversal.
// FR: Ajouter des surcharges const pour parcours immuable.
} 
