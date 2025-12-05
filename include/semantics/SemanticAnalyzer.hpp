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

/**
 * @brief Symbol information for semantic analysis
 */
struct Symbol {
    std::string name;
    AST::Type* type = nullptr;  // Non-owning pointer to type
    bool isConst = false;
    bool isFunction = false;
    bool isTypedef = false;
    bool isEnumConstant = false;
    long long enumValue = 0;  // Value for enum constants
    int line = 0;
    int column = 0;
    
    // For struct/union/enum tag namespace
    enum class TagKind { None, Struct, Union, Enum };
    TagKind tagKind = TagKind::None;
    AST::StructDecl* structDecl = nullptr;  // For storing struct definition (non-owning)
};

/**
 * @brief Scope for symbol table
 */
class Scope {
public:
    std::map<std::string, Symbol> symbols;
    std::map<std::string, Symbol> tags;  // struct/union/enum tags
    std::vector<AST::Ptr<AST::Type>> ownedTypes;  // Storage for types
    Scope* parent = nullptr;
    bool isFunction = false;
    
    Symbol* lookup(const std::string& name);
    Symbol* lookupTag(const std::string& name);
    Symbol* lookupLocal(const std::string& name);
    void define(const std::string& name, const Symbol& sym, AST::Ptr<AST::Type> type = nullptr);
    void defineTag(const std::string& name, const Symbol& sym);
};

/**
 * @brief Semantic Analyzer using the Visitor pattern
 * 
 * Performs type checking and semantic validation:
 * - Variable has incomplete type 'void'
 * - Cannot assign to const variable
 * - Incompatible pointer/integer conversions
 * - Wrong argument types in function calls
 * - Tag type mismatch (struct vs enum with same name)
 * - Bit-field with non-integral type
 * - Non-constant array size at file scope
 * - Division by zero in constant expressions
 */
class SemanticAnalyzer : public AST::DefaultVisitor {
public:
    SemanticAnalyzer(const std::string& filename, const std::string& source);
    
    /// Analyze the AST
    void analyze(AST::TranslationUnit& unit);
    
    /// Check if analysis encountered errors
    bool hadError() const { return hadError_; }
    
    // Declarations
    void visit(AST::TranslationUnit& node) override;
    void visit(AST::VarDecl& node) override;
    void visit(AST::FunctionDecl& node) override;
    void visit(AST::ParamDecl& node) override;
    void visit(AST::StructDecl& node) override;
    void visit(AST::EnumDecl& node) override;
    void visit(AST::TypedefDecl& node) override;
    
    // Statements
    void visit(AST::CompoundStmt& node) override;
    void visit(AST::ExpressionStmt& node) override;
    void visit(AST::IfStmt& node) override;
    void visit(AST::WhileStmt& node) override;
    void visit(AST::DoWhileStmt& node) override;
    void visit(AST::ForStmt& node) override;
    void visit(AST::SwitchStmt& node) override;
    void visit(AST::CaseStmt& node) override;
    void visit(AST::DefaultStmt& node) override;
    void visit(AST::ReturnStmt& node) override;
    void visit(AST::LabelStmt& node) override;
    
    // Expressions
    void visit(AST::IntegerLiteral& node) override;
    void visit(AST::FloatLiteral& node) override;
    void visit(AST::CharLiteral& node) override;
    void visit(AST::StringLiteral& node) override;
    void visit(AST::Identifier& node) override;
    void visit(AST::BinaryExpr& node) override;
    void visit(AST::UnaryExpr& node) override;
    void visit(AST::CastExpr& node) override;
    void visit(AST::CallExpr& node) override;
    void visit(AST::MemberExpr& node) override;
    void visit(AST::IndexExpr& node) override;
    void visit(AST::TernaryExpr& node) override;
    void visit(AST::SizeofExpr& node) override;
    void visit(AST::InitializerList& node) override;
    
private:
    // Error reporting
    void error(int line, int column, const std::string& message);
    void warning(int line, int column, const std::string& message);
    std::string getSourceLine(int line) const;
    
    // Scope management
    void enterScope(bool isFunction = false);
    void exitScope();
    
    // Type utilities
    AST::Ptr<AST::Type> getExprType(AST::Expression* expr);
    bool isVoidType(AST::Type* type);
    bool isIntegerType(AST::Type* type);
    bool isFloatType(AST::Type* type);
    bool isPointerType(AST::Type* type);
    bool isArrayType(AST::Type* type);
    bool isStructType(AST::Type* type);
    bool isConstQualified(AST::Type* type);
    bool isScalarType(AST::Type* type);
    bool typesCompatible(AST::Type* left, AST::Type* right);
    bool canAssign(AST::Type* target, AST::Type* source, bool isInit = false);
    AST::Type* stripQualifiers(AST::Type* type);
    std::string typeToString(AST::Type* type);
    
    // Constant expression evaluation
    bool isConstantExpr(AST::Expression* expr);
    bool evaluateConstantExpr(AST::Expression* expr, long long& result);
    bool hasDivisionByZero(AST::Expression* expr);
    
    // Enum processing
    void processEnumType(AST::EnumType* enumType);
    
    // State
    std::string filename_;
    std::string source_;
    bool hadError_ = false;
    
    // Scope stack
    std::vector<std::unique_ptr<Scope>> scopes_;
    Scope* currentScope_ = nullptr;
    
    // Context
    AST::FunctionDecl* currentFunction_ = nullptr;
    bool inGlobalScope_ = true;
    
    // Expression type tracking (for type inference)
    std::map<AST::Expression*, AST::Ptr<AST::Type>> exprTypes_;
};

} // namespace cc1
