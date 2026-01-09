#pragma once

#include <parser/ast/Node.hpp>
#include <parser/ast/Type.hpp>
#include <parser/ast/Expression.hpp>
#include <parser/ast/Statement.hpp>
#include <string>

namespace AST {

// Forward declarations
class Visitor;

// ============================================================================
// Storage Class Specifiers
// ============================================================================

enum class StorageClass {
    None,
    Auto,
    Register,
    Static,
    Extern,
    Typedef
};

// ============================================================================
// Declaration Base Class
// ============================================================================

class Declaration : public Node {
public:
    StorageClass storageClass = StorageClass::None;
    
    ~Declaration() override = default;
    
protected:
    Declaration() = default;
    Declaration(int l, int c) : Node(l, c) {}
};

// ============================================================================
// Variable Declaration
// ============================================================================

class VarDecl : public Declaration {
public:
    std::string name;
    Ptr<Type> type;
    Ptr<Expression> initializer;  // nullptr if not initialized
    
    VarDecl(const std::string& n, Ptr<Type> t, int l, int c)
        : Declaration(l, c), name(n), type(std::move(t)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Parameter Declaration (for function parameters)
// ============================================================================

class ParamDecl : public Declaration {
public:
    std::string name;  // Can be empty for unnamed parameters
    Ptr<Type> type;
    
    ParamDecl(const std::string& n, Ptr<Type> t, int l, int c)
        : Declaration(l, c), name(n), type(std::move(t)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Function Declaration/Definition
// ============================================================================

class FunctionDecl : public Declaration {
public:
    std::string name;
    Ptr<Type> returnType;
    PtrList<ParamDecl> parameters;
    Ptr<CompoundStmt> body;  // nullptr for declarations (prototypes)
    bool isVariadic = false;
    bool isDefinition = false;
    
    FunctionDecl(const std::string& n, Ptr<Type> ret, int l, int c)
        : Declaration(l, c), name(n), returnType(std::move(ret)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Struct/Union Declaration
// ============================================================================

class StructDecl : public Declaration {
public:
    std::string name;  // Empty for anonymous
    PtrList<VarDecl> members;
    // If this StructDecl originates from parsing a standalone struct/union definition
    // (e.g. "struct S { ... };"), keep the full StructType so we don't lose bitfield widths.
    Ptr<StructType> declaredType;
    bool isUnion = false;
    bool isForwardDecl = false;  // Just "struct S;"
    
    StructDecl(const std::string& n, bool u, int l, int c)
        : Declaration(l, c), name(n), isUnion(u) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Enum Declaration
// ============================================================================

struct EnumeratorDecl {
    std::string name;
    Ptr<Expression> value;  // nullptr if no explicit value
    int line, column;
    int equalColumn = 0;  // Position of '=' sign (if present)
    mutable long long computedValue = 0;  // Computed after semantic analysis
    
    EnumeratorDecl(const std::string& n, int l, int c, int eq = 0)
        : name(n), line(l), column(c), equalColumn(eq), computedValue(0) {}
};

class EnumDecl : public Declaration {
public:
    std::string name;  // Empty for anonymous
    std::vector<EnumeratorDecl> enumerators;
    
    EnumDecl(const std::string& n, int l, int c)
        : Declaration(l, c), name(n) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Typedef Declaration
// ============================================================================

class TypedefDecl : public Declaration {
public:
    std::string name;
    Ptr<Type> underlyingType;
    
    TypedefDecl(const std::string& n, Ptr<Type> t, int l, int c)
        : Declaration(l, c), name(n), underlyingType(std::move(t)) {}
    
    void accept(Visitor& visitor) override;
};

// ============================================================================
// Translation Unit (Root of AST)
// ============================================================================

class TranslationUnit : public Node {
public:
    PtrList<Declaration> declarations;
    
    TranslationUnit() = default;
    TranslationUnit(int l, int c) : Node(l, c) {}
    
    void accept(Visitor& visitor) override;
};

} // namespace AST
