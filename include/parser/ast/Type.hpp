#pragma once



#include <parser/ast/Node.hpp>
#include <string>
#include <vector>

namespace AST {


class TypeVisitor;
class Expression;





// EN: Base type node with visitor support and utility methods.
// FR: Type de base avec support visiteur et utilitaires.
class Type {
public:
    // EN: Virtual destructor for polymorphic cleanup.
    // FR: Destructeur virtuel pour nettoyage polymorphique.
    virtual ~Type() = default;
    // EN: Accepts a type visitor.
    // FR: Accepte un visiteur de type.
    virtual void accept(TypeVisitor& visitor) const = 0;
    // EN: Structural equality check between types.
    // FR: Test d egalite structurelle entre types.
    virtual bool equals(const Type& other) const = 0;
    // EN: Returns a human-readable type string.
    // FR: Renvoie une chaine lisible du type.
    virtual std::string toString() const = 0;
    // EN: Clones the type node.
    // FR: Clone le noeud de type.
    virtual Ptr<Type> clone() const = 0;
    
    
    // EN: Source line for diagnostics.
    // FR: Ligne source pour diagnostics.
    int line = 0;
    // EN: Source column for diagnostics.
    // FR: Colonne source pour diagnostics.
    int column = 0;
    
protected:
    Type() = default;
    Type(int l, int c) : line(l), column(c) {}
};





// EN: Primitive type kinds.
// FR: Types primitifs.
enum class PrimitiveKind {
    Void,
    Char,
    SignedChar,
    UnsignedChar,
    Short,
    UnsignedShort,
    Int,
    UnsignedInt,
    Long,
    UnsignedLong,
    LongLong,
    UnsignedLongLong,
    Float,
    Double,
    LongDouble
};

// EN: Primitive type node (int, float, etc.).
// FR: Noeud de type primitif (int, float, etc.).
class PrimitiveType : public Type {
public:
    PrimitiveKind kind;
    
    // EN: Builds a primitive type node.
    // FR: Construit un type primitif.
    explicit PrimitiveType(PrimitiveKind k, int l = 0, int c = 0) 
        : Type(l, c), kind(k) {}
    
    // EN: Accepts a type visitor.
    // FR: Accepte un visiteur de type.
    void accept(TypeVisitor& visitor) const override;
    // EN: Compares primitive types.
    // FR: Compare les types primitifs.
    bool equals(const Type& other) const override;
    // EN: Returns a string for this primitive type.
    // FR: Renvoie la chaine du type primitif.
    std::string toString() const override;
    // EN: Clones this primitive type.
    // FR: Clone ce type primitif.
    Ptr<Type> clone() const override;
};





// EN: Pointer type node.
// FR: Noeud de type pointeur.
class PointerType : public Type {
public:
    Ptr<Type> pointee;
    
    // EN: Builds a pointer type to a pointee type.
    // FR: Construit un type pointeur vers un type cible.
    explicit PointerType(Ptr<Type> p, int l = 0, int c = 0) 
        : Type(l, c), pointee(std::move(p)) {}
    
    // EN: Accepts a type visitor.
    // FR: Accepte un visiteur de type.
    void accept(TypeVisitor& visitor) const override;
    // EN: Compares pointer types.
    // FR: Compare les types pointeurs.
    bool equals(const Type& other) const override;
    // EN: Returns a string for this pointer type.
    // FR: Renvoie la chaine de ce type pointeur.
    std::string toString() const override;
    // EN: Clones this pointer type.
    // FR: Clone ce type pointeur.
    Ptr<Type> clone() const override;
};





// EN: Qualified type node (const/volatile).
// FR: Noeud de type qualifie (const/volatile).
class QualifiedType : public Type {
public:
    Ptr<Type> baseType;
    bool isConst = false;
    bool isVolatile = false;
    
    // EN: Builds a qualified type around a base type.
    // FR: Construit un type qualifie autour d un type de base.
    QualifiedType(Ptr<Type> base, bool c, bool v, int l = 0, int col = 0)
        : Type(l, col), baseType(std::move(base)), isConst(c), isVolatile(v) {}
    
    // EN: Accepts a type visitor.
    // FR: Accepte un visiteur de type.
    void accept(TypeVisitor& visitor) const override;
    // EN: Compares qualified types.
    // FR: Compare les types qualifies.
    bool equals(const Type& other) const override;
    // EN: Returns a string for this qualified type.
    // FR: Renvoie la chaine de ce type qualifie.
    std::string toString() const override;
    // EN: Clones this qualified type.
    // FR: Clone ce type qualifie.
    Ptr<Type> clone() const override;
};





// EN: Array type node with element type and size info.
// FR: Noeud de type tableau avec element et taille.
class ArrayType : public Type {
public:
    Ptr<Type> elementType;
    Ptr<Expression> sizeExpr;  
    long long size = -1;  
    
    // EN: Builds an array type with a size expression.
    // FR: Construit un type tableau avec expression de taille.
    ArrayType(Ptr<Type> elem, Ptr<Expression> sizeExpr, int l = 0, int c = 0) 
        : Type(l, c), elementType(std::move(elem)), sizeExpr(std::move(sizeExpr)) {}
    
    // EN: Builds an array type with a resolved size.
    // FR: Construit un type tableau avec taille resolue.
    ArrayType(Ptr<Type> elem, long long sz, int l = 0, int c = 0)
        : Type(l, c), elementType(std::move(elem)), size(sz) {}
    
    // EN: Accepts a type visitor.
    // FR: Accepte un visiteur de type.
    void accept(TypeVisitor& visitor) const override;
    // EN: Compares array types.
    // FR: Compare les types tableau.
    bool equals(const Type& other) const override;
    // EN: Returns a string for this array type.
    // FR: Renvoie la chaine de ce type tableau.
    std::string toString() const override;
    // EN: Clones this array type.
    // FR: Clone ce type tableau.
    Ptr<Type> clone() const override;
};





// EN: Function type node with parameter types and variadic flag.
// FR: Noeud de type fonction avec params et flag variadique.
class FunctionType : public Type {
public:
    Ptr<Type> returnType;
    std::vector<Ptr<Type>> parameterTypes;
    bool isVariadic = false;
    
    // EN: Builds a function type with parameters and variadic info.
    // FR: Construit un type fonction avec params et info variadique.
    FunctionType(Ptr<Type> ret, std::vector<Ptr<Type>> params, bool variadic,
                 int l = 0, int c = 0)
        : Type(l, c), returnType(std::move(ret)), 
          parameterTypes(std::move(params)), isVariadic(variadic) {}
    
    // EN: Accepts a type visitor.
    // FR: Accepte un visiteur de type.
    void accept(TypeVisitor& visitor) const override;
    // EN: Compares function types.
    // FR: Compare les types fonction.
    bool equals(const Type& other) const override;
    // EN: Returns a string for this function type.
    // FR: Renvoie la chaine de ce type fonction.
    std::string toString() const override;
    // EN: Clones this function type.
    // FR: Clone ce type fonction.
    Ptr<Type> clone() const override;
};






// EN: Struct/union member description, optionally a bitfield.
// FR: Membre de struct/union, optionnellement bitfield.
struct StructMember {
    std::string name;
    Ptr<Type> type;
    int line = 0;
    int column = 0;
    int bitWidth = -1;  
    
    
    // EN: Builds a regular member entry.
    // FR: Construit une entree de membre standard.
    StructMember(const std::string& n, Ptr<Type> t, int l = 0, int c = 0)
        : name(n), type(std::move(t)), line(l), column(c), bitWidth(-1) {}
    
    
    // EN: Builds a bitfield member entry.
    // FR: Construit une entree de membre bitfield.
    StructMember(const std::string& n, Ptr<Type> t, int l, int c, int bw)
        : name(n), type(std::move(t)), line(l), column(c), bitWidth(bw) {}
        
    // EN: Checks if this member is a bitfield.
    // FR: Verifie si ce membre est un bitfield.
    bool isBitfield() const { return bitWidth >= 0; }
};

// EN: Struct/union type node with member list.
// FR: Noeud de type struct/union avec liste de membres.
class StructType : public Type {
public:
    std::string name;  
    std::vector<StructMember> members;
    bool isUnion = false;
    bool isComplete = false;  
    
    // EN: Builds a struct/union type with name and union flag.
    // FR: Construit un type struct/union avec nom et flag union.
    StructType(const std::string& n, bool u, int l = 0, int c = 0) 
        : Type(l, c), name(n), isUnion(u) {}
    
    // EN: Accepts a type visitor.
    // FR: Accepte un visiteur de type.
    void accept(TypeVisitor& visitor) const override;
    // EN: Compares struct/union types.
    // FR: Compare les types struct/union.
    bool equals(const Type& other) const override;
    // EN: Returns a string for this struct/union type.
    // FR: Renvoie la chaine de ce type struct/union.
    std::string toString() const override;
    // EN: Clones this struct/union type.
    // FR: Clone ce type struct/union.
    Ptr<Type> clone() const override;
};





// EN: Enum enumerator with optional explicit value.
// FR: Enumerateur d enum avec valeur optionnelle explicite.
struct Enumerator {
    std::string name;
    Ptr<Expression> value;  
    int line = 0, column = 0;  
    int equalColumn = 0;  
    mutable long long computedValue = 0;  
    
    // EN: Builds an enumerator entry.
    // FR: Construit une entree d enumerateur.
    Enumerator(const std::string& n, Ptr<Expression> v, int l = 0, int c = 0, int eq = 0)
        : name(n), value(std::move(v)), line(l), column(c), equalColumn(eq), computedValue(0) {}
};

// EN: Enum type node with enumerator list.
// FR: Noeud de type enum avec liste d enumerateurs.
class EnumType : public Type {
public:
    std::string name;  
    std::vector<Enumerator> enumerators;
    
    // EN: Builds an enum type with a name.
    // FR: Construit un type enum avec nom.
    explicit EnumType(const std::string& n, int l = 0, int c = 0) 
        : Type(l, c), name(n) {}
    
    // EN: Accepts a type visitor.
    // FR: Accepte un visiteur de type.
    void accept(TypeVisitor& visitor) const override;
    // EN: Compares enum types.
    // FR: Compare les types enum.
    bool equals(const Type& other) const override;
    // EN: Returns a string for this enum type.
    // FR: Renvoie la chaine de ce type enum.
    std::string toString() const override;
    // EN: Clones this enum type.
    // FR: Clone ce type enum.
    Ptr<Type> clone() const override;
};





// EN: Typedef type node pointing to an underlying type.
// FR: Noeud de type typedef pointant vers un type sous-jacent.
class TypedefType : public Type {
public:
    std::string name;
    Ptr<Type> underlyingType;  
    
    // EN: Builds a typedef type with a name.
    // FR: Construit un type typedef avec nom.
    TypedefType(const std::string& n, int l = 0, int c = 0)
        : Type(l, c), name(n) {}
    
    // EN: Accepts a type visitor.
    // FR: Accepte un visiteur de type.
    void accept(TypeVisitor& visitor) const override;
    // EN: Compares typedef types.
    // FR: Compare les types typedef.
    bool equals(const Type& other) const override;
    // EN: Returns a string for this typedef type.
    // FR: Renvoie la chaine de ce type typedef.
    std::string toString() const override;
    // EN: Clones this typedef type.
    // FR: Clone ce type typedef.
    Ptr<Type> clone() const override;
};





// EN: Visitor interface for type nodes.
// FR: Interface visiteur pour noeuds de type.
class TypeVisitor {
public:
    // EN: Virtual destructor for polymorphic cleanup.
    // FR: Destructeur virtuel pour nettoyage polymorphique.
    virtual ~TypeVisitor() = default;
    
    // EN: Visits a primitive type node.
    // FR: Visite un type primitif.
    virtual void visit(const PrimitiveType& type) = 0;
    // EN: Visits a pointer type node.
    // FR: Visite un type pointeur.
    virtual void visit(const PointerType& type) = 0;
    // EN: Visits a qualified type node.
    // FR: Visite un type qualifie.
    virtual void visit(const QualifiedType& type) = 0;
    // EN: Visits an array type node.
    // FR: Visite un type tableau.
    virtual void visit(const ArrayType& type) = 0;
    // EN: Visits a function type node.
    // FR: Visite un type fonction.
    virtual void visit(const FunctionType& type) = 0;
    // EN: Visits a struct/union type node.
    // FR: Visite un type struct/union.
    virtual void visit(const StructType& type) = 0;
    // EN: Visits an enum type node.
    // FR: Visite un type enum.
    virtual void visit(const EnumType& type) = 0;
    // EN: Visits a typedef type node.
    // FR: Visite un type typedef.
    virtual void visit(const TypedefType& type) = 0;
};

// TODO(cc1) EN: Add support for atomic and vector types if needed.
// FR: Ajouter le support des types atomiques et vectoriels si besoin.
} 
