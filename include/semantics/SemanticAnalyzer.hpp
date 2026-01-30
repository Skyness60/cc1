#pragma once

#include <parser/ast/Visitor.hpp>
#include <parser/ast/Type.hpp>
#include <parser/ParseError.hpp>
#include <utils/SourceLocation.hpp>
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace cc1 {




// EN: Represents a symbol entry with type and semantic metadata.
// FR: Represente une entree de symbole avec type et metadonnees semantiques.
struct Symbol {
    std::string name;
    AST::Type* type = nullptr;  
    bool isConst = false;
    bool isFunction = false;
    bool isTypedef = false;
    bool isEnumConstant = false;
    long long enumValue = 0;  
    int line = 0;
    int column = 0;
    
    
    // EN: Tag kind for struct/union/enum namespaces.
    // FR: Type de tag pour namespaces struct/union/enum.
    enum class TagKind { None, Struct, Union, Enum };
    TagKind tagKind = TagKind::None;
    AST::StructDecl* structDecl = nullptr;  
};


// EN: Represents a lexical scope with symbols, tags, and owned types.
// FR: Represente un scope lexical avec symboles, tags et types possedes.
class Scope {
public:
    std::map<std::string, Symbol> symbols;
    std::map<std::string, Symbol> tags;  
    std::vector<AST::Ptr<AST::Type>> ownedTypes;  
    Scope* parent = nullptr;
    bool isFunction = false;
    
    // EN: Looks up a symbol by name through parent scopes.
    // FR: Cherche un symbole par nom dans les scopes parents.
    Symbol* lookup(const std::string& name);
    // EN: Looks up a tag symbol by name through parent scopes.
    // FR: Cherche un tag par nom dans les scopes parents.
    Symbol* lookupTag(const std::string& name);
    // EN: Looks up a symbol only in the current scope.
    // FR: Cherche un symbole seulement dans le scope courant.
    Symbol* lookupLocal(const std::string& name);
    // EN: Defines a symbol and optionally stores an owned type.
    // FR: Definit un symbole et stocke un type possede optionnel.
    void define(const std::string& name, const Symbol& sym, AST::Ptr<AST::Type> type = nullptr);
    // EN: Defines a tag symbol (struct/union/enum).
    // FR: Definit un tag (struct/union/enum).
    void defineTag(const std::string& name, const Symbol& sym);
};


// EN: Walks the AST to resolve types, check semantics, and report errors.
// FR: Parcourt l AST pour resoudre types, verifier semantique et reporter erreurs.
class SemanticAnalyzer : public AST::DefaultVisitor {
public:
    // EN: Initializes analyzer with source context and target bitness.
    // FR: Initialise l analyseur avec contexte source et bitness cible.
    SemanticAnalyzer(const std::string& filename, const std::string& source, bool is64bit = false);
    
    
    // EN: Entry point for semantic analysis of a translation unit.
    // FR: Point d entree pour l analyse semantique d une unite de traduction.
    void analyze(AST::TranslationUnit& unit);
    
    
    // EN: Reports whether any semantic errors were encountered.
    // FR: Indique si des erreurs semantiques ont ete rencontrees.
    bool hadError() const { return hadError_; }
    
    
    // EN: Visits the translation unit and establishes global scope.
    // FR: Visite l unite de traduction et etabli le scope global.
    void visit(AST::TranslationUnit& node) override;
    // EN: Visits variable declarations and registers symbols.
    // FR: Visite les declarations de variables et enregistre les symboles.
    void visit(AST::VarDecl& node) override;
    // EN: Visits function declarations/definitions.
    // FR: Visite les declarations/definitions de fonctions.
    void visit(AST::FunctionDecl& node) override;
    // EN: Visits parameter declarations.
    // FR: Visite les declarations de parametres.
    void visit(AST::ParamDecl& node) override;
    // EN: Visits struct/union declarations.
    // FR: Visite les declarations struct/union.
    void visit(AST::StructDecl& node) override;
    // EN: Visits enum declarations.
    // FR: Visite les declarations enum.
    void visit(AST::EnumDecl& node) override;
    // EN: Visits typedef declarations.
    // FR: Visite les declarations typedef.
    void visit(AST::TypedefDecl& node) override;
    
    
    // EN: Visits compound statements and manages scope.
    // FR: Visite les blocs composes et gere le scope.
    void visit(AST::CompoundStmt& node) override;
    // EN: Visits expression statements.
    // FR: Visite les instructions expression.
    void visit(AST::ExpressionStmt& node) override;
    // EN: Visits if statements.
    // FR: Visite les instructions if.
    void visit(AST::IfStmt& node) override;
    // EN: Visits while statements.
    // FR: Visite les instructions while.
    void visit(AST::WhileStmt& node) override;
    // EN: Visits do-while statements.
    // FR: Visite les instructions do-while.
    void visit(AST::DoWhileStmt& node) override;
    // EN: Visits for statements.
    // FR: Visite les instructions for.
    void visit(AST::ForStmt& node) override;
    // EN: Visits switch statements.
    // FR: Visite les instructions switch.
    void visit(AST::SwitchStmt& node) override;
    // EN: Visits case statements.
    // FR: Visite les instructions case.
    void visit(AST::CaseStmt& node) override;
    // EN: Visits default statements.
    // FR: Visite les instructions default.
    void visit(AST::DefaultStmt& node) override;
    // EN: Visits return statements.
    // FR: Visite les instructions return.
    void visit(AST::ReturnStmt& node) override;
    // EN: Visits labeled statements.
    // FR: Visite les instructions etiquetees.
    void visit(AST::LabelStmt& node) override;
    
    
    // EN: Visits integer literals to assign type.
    // FR: Visite les litteraux entiers pour assigner le type.
    void visit(AST::IntegerLiteral& node) override;
    // EN: Visits float literals to assign type.
    // FR: Visite les litteraux flottants pour assigner le type.
    void visit(AST::FloatLiteral& node) override;
    // EN: Visits char literals to assign type.
    // FR: Visite les litteraux caractere pour assigner le type.
    void visit(AST::CharLiteral& node) override;
    // EN: Visits string literals to assign type.
    // FR: Visite les litteraux chaine pour assigner le type.
    void visit(AST::StringLiteral& node) override;
    // EN: Visits identifiers to resolve symbols.
    // FR: Visite les identifiants pour resoudre les symboles.
    void visit(AST::Identifier& node) override;
    // EN: Visits binary expressions to type-check operands.
    // FR: Visite les expressions binaires pour typer les operands.
    void visit(AST::BinaryExpr& node) override;
    // EN: Visits unary expressions to type-check operands.
    // FR: Visite les expressions unaires pour typer les operands.
    void visit(AST::UnaryExpr& node) override;
    // EN: Visits cast expressions to check conversion validity.
    // FR: Visite les casts pour verifier la conversion.
    void visit(AST::CastExpr& node) override;
    // EN: Visits call expressions to validate callee/arguments.
    // FR: Visite les appels pour valider callee/arguments.
    void visit(AST::CallExpr& node) override;
    // EN: Visits member expressions to resolve struct/union members.
    // FR: Visite les acces membres pour resoudre struct/union.
    void visit(AST::MemberExpr& node) override;
    // EN: Visits index expressions to validate array/pointer indexing.
    // FR: Visite les indexations pour valider tableau/pointeur.
    void visit(AST::IndexExpr& node) override;
    // EN: Visits ternary expressions for type compatibility.
    // FR: Visite les ternaires pour compatibilite de type.
    void visit(AST::TernaryExpr& node) override;
    // EN: Visits sizeof expressions to compute size type.
    // FR: Visite sizeof pour calculer type de taille.
    void visit(AST::SizeofExpr& node) override;
    // EN: Visits initializer lists to validate aggregate init.
    // FR: Visite les listes d init pour valider init d aggregate.
    void visit(AST::InitializerList& node) override;
    
private:
    
    // EN: Emits an error diagnostic and marks analysis as failed.
    // FR: Emet un diagnostic d erreur et marque l echec.
    void error(int line, int column, const std::string& message);
    // EN: Emits a warning diagnostic.
    // FR: Emet un diagnostic d avertissement.
    void warning(int line, int column, const std::string& message);
    // EN: Returns a source line for diagnostics.
    // FR: Renvoie une ligne source pour diagnostics.
    std::string getSourceLine(int line) const;
    
    
    // EN: Pushes a new scope, marking function scope if needed.
    // FR: Empile un nouveau scope, marque function scope si besoin.
    void enterScope(bool isFunction = false);
    // EN: Pops the current scope.
    // FR: Depile le scope courant.
    void exitScope();
    
    
    // EN: Retrieves or computes the type of an expression.
    // FR: Recupere ou calcule le type d une expression.
    AST::Ptr<AST::Type> getExprType(AST::Expression* expr);
    // EN: Type predicate helpers used throughout semantic checks.
    // FR: Helpers de predicates de type utilises partout.
    bool isVoidType(AST::Type* type);
    bool isIntegerType(AST::Type* type);
    bool isFloatType(AST::Type* type);
    bool isPointerType(AST::Type* type);
    bool isArrayType(AST::Type* type);
    bool isStructType(AST::Type* type);
    bool isConstQualified(AST::Type* type);
    bool isScalarType(AST::Type* type);
    // EN: Compatibility/assignment checks for types.
    // FR: Verifications de compatibilite/affectation de types.
    bool typesCompatible(AST::Type* left, AST::Type* right);
    bool canAssign(AST::Type* target, AST::Type* source, bool isInit = false);
    // EN: Removes qualifiers to compare underlying types.
    // FR: Retire les qualifiers pour comparer les types sous-jacents.
    AST::Type* stripQualifiers(AST::Type* type);
    // EN: Returns a string for diagnostics.
    // FR: Renvoie une chaine pour diagnostics.
    std::string typeToString(AST::Type* type);
    
    
    // EN: Constant expression helpers for enums/cases.
    // FR: Helpers d expressions constantes pour enums/cases.
    bool isConstantExpr(AST::Expression* expr);
    bool evaluateConstantExpr(AST::Expression* expr, long long& result);
    bool hasDivisionByZero(AST::Expression* expr);

    struct ConstExprIsConstVisitor;
    struct ConstExprEvalVisitor;
    struct ConstExprDivZeroVisitor;
    
    
    // EN: Processes enum type to assign values and register constants.
    // FR: Traite un enum pour assigner les valeurs et enregistrer constantes.
    void processEnumType(AST::EnumType* enumType);
    
    
    std::string filename_;
    std::string source_;
    bool hadError_ = false;
    bool is64bit_ = false;
    
    
    std::vector<std::unique_ptr<Scope>> scopes_;
    Scope* currentScope_ = nullptr;
    
    
    AST::FunctionDecl* currentFunction_ = nullptr;
    bool inGlobalScope_ = true;
    
    
    std::map<AST::Expression*, AST::Ptr<AST::Type>> exprTypes_;
};

// TODO(cc1) EN: Add support for diagnostic categories and severity levels.
// FR: Ajouter un support pour categories et niveaux de diagnostic.
} 
