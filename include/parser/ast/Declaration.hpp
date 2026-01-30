#pragma once



#include <parser/ast/Node.hpp>
#include <parser/ast/Type.hpp>
#include <parser/ast/Expression.hpp>
#include <parser/ast/Statement.hpp>
#include <string>

namespace AST {


class Visitor;





// EN: Storage class specifiers for declarations.
// FR: Specifiers de classe de stockage pour declarations.
enum class StorageClass {
    None,
    Auto,
    Register,
    Static,
    Extern,
    Typedef
};





// EN: Base class for declarations with storage class.
// FR: Classe de base pour declarations avec classe de stockage.
class Declaration : public Node {
public:
    // EN: Storage class applied to this declaration.
    // FR: Classe de stockage appliquee a cette declaration.
    StorageClass storageClass = StorageClass::None;
    
    // EN: Virtual destructor for declarations.
    // FR: Destructeur virtuel pour declarations.
    ~Declaration() override = default;
    
protected:
    Declaration() = default;
    Declaration(int l, int c) : Node(l, c) {}
};





// EN: Variable declaration with name, type, and optional initializer.
// FR: Declaration de variable avec nom, type et initialiseur optionnel.
class VarDecl : public Declaration {
public:
    std::string name;
    Ptr<Type> type;
    Ptr<Expression> initializer;  
    
    // EN: Builds a variable declaration node.
    // FR: Construit un noeud de declaration de variable.
    VarDecl(const std::string& n, Ptr<Type> t, int l, int c)
        : Declaration(l, c), name(n), type(std::move(t)) {}
    
    // EN: Accepts a visitor for this variable declaration.
    // FR: Accepte un visiteur pour cette declaration de variable.
    void accept(Visitor& visitor) override;
};





// EN: Function parameter declaration.
// FR: Declaration de parametre de fonction.
class ParamDecl : public Declaration {
public:
    std::string name;  
    Ptr<Type> type;
    
    // EN: Builds a parameter declaration node.
    // FR: Construit un noeud de declaration de parametre.
    ParamDecl(const std::string& n, Ptr<Type> t, int l, int c)
        : Declaration(l, c), name(n), type(std::move(t)) {}
    
    // EN: Accepts a visitor for this parameter declaration.
    // FR: Accepte un visiteur pour cette declaration de parametre.
    void accept(Visitor& visitor) override;
};





// EN: Function declaration/definition with signature and optional body.
// FR: Declaration/definition de fonction avec signature et corps optionnel.
class FunctionDecl : public Declaration {
public:
    std::string name;
    Ptr<Type> returnType;
    PtrList<ParamDecl> parameters;
    Ptr<CompoundStmt> body;  
    bool isVariadic = false;
    bool isDefinition = false;
    
    // EN: Builds a function declaration node.
    // FR: Construit un noeud de declaration de fonction.
    FunctionDecl(const std::string& n, Ptr<Type> ret, int l, int c)
        : Declaration(l, c), name(n), returnType(std::move(ret)) {}
    
    // EN: Accepts a visitor for this function declaration.
    // FR: Accepte un visiteur pour cette declaration de fonction.
    void accept(Visitor& visitor) override;
};





// EN: Struct/union declaration with members.
// FR: Declaration de struct/union avec membres.
class StructDecl : public Declaration {
public:
    std::string name;  
    PtrList<VarDecl> members;
    
    
    Ptr<StructType> declaredType;
    bool isUnion = false;
    bool isForwardDecl = false;  
    
    // EN: Builds a struct/union declaration node.
    // FR: Construit un noeud de declaration struct/union.
    StructDecl(const std::string& n, bool u, int l, int c)
        : Declaration(l, c), name(n), isUnion(u) {}
    
    // EN: Accepts a visitor for this struct/union declaration.
    // FR: Accepte un visiteur pour cette declaration struct/union.
    void accept(Visitor& visitor) override;
};





// EN: Enumerator item with optional explicit value.
// FR: Enumerateur avec valeur optionnelle explicite.
struct EnumeratorDecl {
    std::string name;
    Ptr<Expression> value;  
    int line, column;
    int equalColumn = 0;  
    mutable long long computedValue = 0;  
    
    // EN: Builds an enumerator entry.
    // FR: Construit une entree d enumerateur.
    EnumeratorDecl(const std::string& n, int l, int c, int eq = 0)
        : name(n), line(l), column(c), equalColumn(eq), computedValue(0) {}
};

// EN: Enum declaration with list of enumerators.
// FR: Declaration d enum avec liste d enumerateurs.
class EnumDecl : public Declaration {
public:
    std::string name;  
    std::vector<EnumeratorDecl> enumerators;
    
    // EN: Builds an enum declaration node.
    // FR: Construit un noeud de declaration d enum.
    EnumDecl(const std::string& n, int l, int c)
        : Declaration(l, c), name(n) {}
    
    // EN: Accepts a visitor for this enum declaration.
    // FR: Accepte un visiteur pour cette declaration d enum.
    void accept(Visitor& visitor) override;
};





// EN: Typedef declaration mapping a name to a type.
// FR: Declaration typedef qui mappe un nom vers un type.
class TypedefDecl : public Declaration {
public:
    std::string name;
    Ptr<Type> underlyingType;
    
    // EN: Builds a typedef declaration node.
    // FR: Construit un noeud de declaration typedef.
    TypedefDecl(const std::string& n, Ptr<Type> t, int l, int c)
        : Declaration(l, c), name(n), underlyingType(std::move(t)) {}
    
    // EN: Accepts a visitor for this typedef declaration.
    // FR: Accepte un visiteur pour cette declaration typedef.
    void accept(Visitor& visitor) override;
};





// EN: Root node representing a translation unit (file).
// FR: Noeud racine representant une unite de traduction (fichier).
class TranslationUnit : public Node {
public:
    PtrList<Declaration> declarations;
    
    // EN: Builds an empty translation unit node.
    // FR: Construit un noeud d unite de traduction vide.
    TranslationUnit() = default;
    // EN: Builds a translation unit with a specific location.
    // FR: Construit une unite de traduction avec position specifique.
    TranslationUnit(int l, int c) : Node(l, c) {}
    
    // EN: Accepts a visitor for this translation unit.
    // FR: Accepte un visiteur pour cette unite de traduction.
    void accept(Visitor& visitor) override;
};

// TODO(cc1) EN: Track declaration attributes and alignment.
// FR: Suivre les attributs de declaration et l alignement.
} 
