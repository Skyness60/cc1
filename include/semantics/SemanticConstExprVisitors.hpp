#pragma once



#include <semantics/SemanticAnalyzer.hpp>

#include <cstdint>

namespace cc1 {

// EN: Visitor that checks whether an expression is a constant expression.
// FR: Visiteur qui verifie si une expression est constante.
struct SemanticAnalyzer::ConstExprIsConstVisitor : AST::DefaultVisitor {
    SemanticAnalyzer& sema;
    bool result = false;

    // EN: Initializes the visitor with a semantic analyzer reference.
    // FR: Initialise le visiteur avec une reference a l analyseur.
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

// EN: Visitor that evaluates constant expressions to an integer value.
// FR: Visiteur qui evalue les expressions constantes en valeur entiere.
struct SemanticAnalyzer::ConstExprEvalVisitor : AST::DefaultVisitor {
    SemanticAnalyzer& sema;
    long long& result;
    bool ok = false;

    // EN: Initializes the evaluator with output storage.
    // FR: Initialise l evaluateur avec un stockage de sortie.
    explicit ConstExprEvalVisitor(SemanticAnalyzer& s, long long& out);

    // EN: Returns primitive size in bytes for a kind.
    // FR: Renvoie la taille en octets pour un type primitif.
    int getPrimitiveSize(AST::PrimitiveKind kind) const;
    // EN: Returns alignment in bytes for a type.
    // FR: Renvoie l alignement en octets pour un type.
    int getTypeAlign(AST::Type* type);
    // EN: Returns size in bytes for a type.
    // FR: Renvoie la taille en octets pour un type.
    long long getTypeSize(AST::Type* type);

    // EN: Masks a value to a bit width.
    // FR: Masque une valeur a une largeur de bits.
    static uint64_t maskUnsigned(uint64_t value, int bits);
    // EN: Returns bit width for a primitive kind.
    // FR: Renvoie la largeur de bits pour un type primitif.
    int widthBitsForPrim(AST::PrimitiveKind kind) const;
    // EN: Infers integer width from expression type.
    // FR: Infere la largeur d entier depuis le type d expression.
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

// EN: Visitor that detects division by zero in constant expressions.
// FR: Visiteur qui detecte la division par zero dans les constantes.
struct SemanticAnalyzer::ConstExprDivZeroVisitor : AST::DefaultVisitor {
    SemanticAnalyzer& sema;
    bool found = false;

    // EN: Initializes the visitor with a semantic analyzer reference.
    // FR: Initialise le visiteur avec une reference a l analyseur.
    explicit ConstExprDivZeroVisitor(SemanticAnalyzer& s);

    void visit(AST::BinaryExpr& node) override;
    void visit(AST::UnaryExpr& node) override;
    void visit(AST::TernaryExpr& node) override;
};

} 

// TODO(cc1) EN: Extend const-eval to handle floating constants if needed.
// FR: Etendre l evaluation constante pour flottants si besoin.
