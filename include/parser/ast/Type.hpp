#pragma once

#include <parser/ast/Node.hpp>
#include <string>
#include <vector>

namespace AST {

// Forward declarations
class TypeVisitor;
class Expression;

// ============================================================================
// Type System
// ============================================================================

class Type {
public:
    virtual ~Type() = default;
    virtual void accept(TypeVisitor& visitor) const = 0;
    virtual bool equals(const Type& other) const = 0;
    virtual std::string toString() const = 0;
    virtual Ptr<Type> clone() const = 0;
    
    // Source location (optional, for error messages)
    int line = 0;
    int column = 0;
    
protected:
    Type() = default;
    Type(int l, int c) : line(l), column(c) {}
};

// ============================================================================
// Primitive Types
// ============================================================================

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

class PrimitiveType : public Type {
public:
    PrimitiveKind kind;
    
    explicit PrimitiveType(PrimitiveKind k, int l = 0, int c = 0) 
        : Type(l, c), kind(k) {}
    
    void accept(TypeVisitor& visitor) const override;
    bool equals(const Type& other) const override;
    std::string toString() const override;
    Ptr<Type> clone() const override;
};

// ============================================================================
// Pointer Type
// ============================================================================

class PointerType : public Type {
public:
    Ptr<Type> pointee;
    
    explicit PointerType(Ptr<Type> p, int l = 0, int c = 0) 
        : Type(l, c), pointee(std::move(p)) {}
    
    void accept(TypeVisitor& visitor) const override;
    bool equals(const Type& other) const override;
    std::string toString() const override;
    Ptr<Type> clone() const override;
};

// ============================================================================
// Qualified Type (const, volatile)
// ============================================================================

class QualifiedType : public Type {
public:
    Ptr<Type> baseType;
    bool isConst = false;
    bool isVolatile = false;
    
    QualifiedType(Ptr<Type> base, bool c, bool v, int l = 0, int col = 0)
        : Type(l, col), baseType(std::move(base)), isConst(c), isVolatile(v) {}
    
    void accept(TypeVisitor& visitor) const override;
    bool equals(const Type& other) const override;
    std::string toString() const override;
    Ptr<Type> clone() const override;
};

// ============================================================================
// Array Type
// ============================================================================

class ArrayType : public Type {
public:
    Ptr<Type> elementType;
    Ptr<Expression> sizeExpr;  // nullptr means unspecified size
    long long size = -1;  // Computed size, -1 means not computed
    
    ArrayType(Ptr<Type> elem, Ptr<Expression> sizeExpr, int l = 0, int c = 0) 
        : Type(l, c), elementType(std::move(elem)), sizeExpr(std::move(sizeExpr)) {}
    
    ArrayType(Ptr<Type> elem, long long sz, int l = 0, int c = 0)
        : Type(l, c), elementType(std::move(elem)), size(sz) {}
    
    void accept(TypeVisitor& visitor) const override;
    bool equals(const Type& other) const override;
    std::string toString() const override;
    Ptr<Type> clone() const override;
};

// ============================================================================
// Function Type
// ============================================================================

class FunctionType : public Type {
public:
    Ptr<Type> returnType;
    std::vector<Ptr<Type>> parameterTypes;
    bool isVariadic = false;
    
    FunctionType(Ptr<Type> ret, std::vector<Ptr<Type>> params, bool variadic,
                 int l = 0, int c = 0)
        : Type(l, c), returnType(std::move(ret)), 
          parameterTypes(std::move(params)), isVariadic(variadic) {}
    
    void accept(TypeVisitor& visitor) const override;
    bool equals(const Type& other) const override;
    std::string toString() const override;
    Ptr<Type> clone() const override;
};

// ============================================================================
// Struct/Union Type
// ============================================================================

/// Struct member for type representation
struct StructMember {
    std::string name;
    Ptr<Type> type;
    int line = 0;
    int column = 0;
    
    StructMember(const std::string& n, Ptr<Type> t, int l = 0, int c = 0)
        : name(n), type(std::move(t)), line(l), column(c) {}
};

class StructType : public Type {
public:
    std::string name;  // empty for anonymous structs
    std::vector<StructMember> members;
    bool isUnion = false;
    bool isComplete = false;  // false for forward declarations
    
    StructType(const std::string& n, bool u, int l = 0, int c = 0) 
        : Type(l, c), name(n), isUnion(u) {}
    
    void accept(TypeVisitor& visitor) const override;
    bool equals(const Type& other) const override;
    std::string toString() const override;
    Ptr<Type> clone() const override;
};

// ============================================================================
// Enum Type
// ============================================================================

struct Enumerator {
    std::string name;
    Ptr<Expression> value;  // nullptr if no explicit value
    int line = 0, column = 0;  // Position of the enumerator name
    int equalColumn = 0;  // Position of the '=' sign (if present)
    mutable long long computedValue = 0;  // Computed after semantic analysis
    
    Enumerator(const std::string& n, Ptr<Expression> v, int l = 0, int c = 0, int eq = 0)
        : name(n), value(std::move(v)), line(l), column(c), equalColumn(eq), computedValue(0) {}
};

class EnumType : public Type {
public:
    std::string name;  // empty for anonymous enums
    std::vector<Enumerator> enumerators;
    
    explicit EnumType(const std::string& n, int l = 0, int c = 0) 
        : Type(l, c), name(n) {}
    
    void accept(TypeVisitor& visitor) const override;
    bool equals(const Type& other) const override;
    std::string toString() const override;
    Ptr<Type> clone() const override;
};

// ============================================================================
// Typedef Type (reference to a typedef name)
// ============================================================================

class TypedefType : public Type {
public:
    std::string name;
    Ptr<Type> underlyingType;  // resolved type (set during semantic analysis)
    
    TypedefType(const std::string& n, int l = 0, int c = 0)
        : Type(l, c), name(n) {}
    
    void accept(TypeVisitor& visitor) const override;
    bool equals(const Type& other) const override;
    std::string toString() const override;
    Ptr<Type> clone() const override;
};

// ============================================================================
// Type Visitor Interface
// ============================================================================

class TypeVisitor {
public:
    virtual ~TypeVisitor() = default;
    
    virtual void visit(const PrimitiveType& type) = 0;
    virtual void visit(const PointerType& type) = 0;
    virtual void visit(const QualifiedType& type) = 0;
    virtual void visit(const ArrayType& type) = 0;
    virtual void visit(const FunctionType& type) = 0;
    virtual void visit(const StructType& type) = 0;
    virtual void visit(const EnumType& type) = 0;
    virtual void visit(const TypedefType& type) = 0;
};

} // namespace AST
