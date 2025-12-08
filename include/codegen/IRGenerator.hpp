#pragma once

#include <parser/ast/Visitor.hpp>
#include <parser/ast/Type.hpp>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <stack>

namespace cc1 {

/**
 * @brief Represents an LLVM IR value (register or constant)
 */
struct IRValue {
    std::string name;       // Register name (e.g., "%1") or constant value
    std::string type;       // LLVM type (e.g., "i32", "i32*")
    bool isPointer = false; // True if this is a pointer/address
    bool isConstant = false;
    
    IRValue() = default;
    IRValue(const std::string& n, const std::string& t, bool ptr = false, bool cst = false)
        : name(n), type(t), isPointer(ptr), isConstant(cst) {}
    
    // Get the type when dereferenced (removes one level of pointer)
    std::string derefType() const {
        if (type.size() > 1 && type.back() == '*') {
            return type.substr(0, type.size() - 1);
        }
        return type;
    }
};

/**
 * @brief Symbol information for IR generation
 */
struct IRSymbol {
    std::string name;           // Original C name
    std::string irName;         // LLVM IR name (pointer to alloca)
    std::string type;           // LLVM type of the value
    bool isGlobal = false;
    bool isFunction = false;
    bool isParameter = false;
    int paramIndex = -1;
};

/**
 * @brief LLVM IR Generator using the Visitor pattern
 * 
 * Generates LLVM IR for i386 (32-bit) target following System V ABI.
 * 
 * Responsibilities divided into separate files (SRP):
 * - IRCore.cpp: Constructor, main emit functions, module structure
 * - IRTypes.cpp: Type mapping from C to LLVM IR
 * - IRDecl.cpp: Declaration visitors (variables, functions, structs)
 * - IRStmt.cpp: Statement visitors (if, while, for, switch, etc.)
 * - IRExpr.cpp: Expression visitors (arithmetic, logical, calls, etc.)
 */
class IRGenerator : public AST::DefaultVisitor {
public:
    IRGenerator(bool is64bit = false);
    
    /// Generate IR for the AST
    void generate(AST::TranslationUnit& unit);
    
    /// Get the generated LLVM IR
    std::string getIR() const;
    
    /// Check if generation had errors
    bool hadError() const { return hadError_; }

    // ========================================================================
    // Declaration Visitors
    // ========================================================================
    void visit(AST::TranslationUnit& node) override;
    void visit(AST::VarDecl& node) override;
    void visit(AST::FunctionDecl& node) override;
    void visit(AST::ParamDecl& node) override;
    void visit(AST::StructDecl& node) override;
    void visit(AST::EnumDecl& node) override;
    void visit(AST::TypedefDecl& node) override;
    
    // ========================================================================
    // Statement Visitors
    // ========================================================================
    void visit(AST::CompoundStmt& node) override;
    void visit(AST::ExpressionStmt& node) override;
    void visit(AST::IfStmt& node) override;
    void visit(AST::WhileStmt& node) override;
    void visit(AST::DoWhileStmt& node) override;
    void visit(AST::ForStmt& node) override;
    void visit(AST::SwitchStmt& node) override;
    void visit(AST::CaseStmt& node) override;
    void visit(AST::DefaultStmt& node) override;
    void visit(AST::BreakStmt& node) override;
    void visit(AST::ContinueStmt& node) override;
    void visit(AST::ReturnStmt& node) override;
    void visit(AST::GotoStmt& node) override;
    void visit(AST::LabelStmt& node) override;
    
    // ========================================================================
    // Expression Visitors
    // ========================================================================
    void visit(AST::IntegerLiteral& node) override;
    void visit(AST::FloatLiteral& node) override;
    void visit(AST::CharLiteral& node) override;
    void visit(AST::StringLiteral& node) override;
    void visit(AST::Identifier& node) override;
    void visit(AST::BinaryExpr& node) override;
    void visit(AST::UnaryExpr& node) override;
    void visit(AST::CastExpr& node) override;
    void visit(AST::SizeofExpr& node) override;
    void visit(AST::CallExpr& node) override;
    void visit(AST::MemberExpr& node) override;
    void visit(AST::IndexExpr& node) override;
    void visit(AST::TernaryExpr& node) override;
    void visit(AST::InitializerList& node) override;

    // ========================================================================
    // Type Utilities (IRTypes.cpp)
    // ========================================================================
    std::string typeToLLVM(AST::Type* type);
    std::string primitiveToLLVM(AST::PrimitiveKind kind);
    int getTypeSize(AST::Type* type);
    int getTypeAlign(AST::Type* type);
    int getPrimitiveSize(AST::PrimitiveKind kind);
    std::string getDefaultValue(AST::Type* type);
    AST::Type* stripQualifiers(AST::Type* type);

    // ========================================================================
    // IR Emission Helpers (IRCore.cpp)
    // ========================================================================
    std::string newTemp();          // Generate new temporary register
    std::string newLabel(const std::string& prefix = "L");
    std::string newGlobalString(const std::string& str);
    
    void emit(const std::string& ir);
    void emitLabel(const std::string& label);
    void emitComment(const std::string& comment);
    void emitGlobal(const std::string& ir);
    
    // ========================================================================
    // Value Management
    // ========================================================================
    IRValue loadValue(const IRValue& val);  // Load if pointer
    IRValue storeValue(const IRValue& val, const IRValue& ptr);
    
    // ========================================================================
    // Symbol Table
    // ========================================================================
    void enterScope();
    void exitScope();
    IRSymbol* lookupSymbol(const std::string& name);
    void defineSymbol(const std::string& name, const IRSymbol& sym);
    
    // ========================================================================
    // Type Resolution
    // ========================================================================
    AST::Type* resolveTypedef(const std::string& name);
    
    // ========================================================================
    // Struct Layout
    // ========================================================================
    struct StructLayout {
        std::string llvmType;
        std::map<std::string, int> memberIndices;  // name -> struct index
        std::map<std::string, std::string> memberTypes;  // name -> LLVM type
        int totalSize = 0;
        int alignment = 1;
    };
    StructLayout computeStructLayout(AST::StructDecl* decl);
    StructLayout computeStructLayout(AST::StructType* type);
    const StructLayout* getStructLayout(const std::string& name);
    
    // Extract the Nth field type from an inline LLVM struct type string
    // E.g., from "{ float, [2 x i32], { i32 } }" get field 1 as "[2 x i32]"
    std::string extractFieldTypeFromInlineStruct(const std::string& inlineStructType, int fieldIndex);

    
    // ========================================================================
    // Constant Expression Evaluation
    // ========================================================================
    bool evaluateConstantExpr(AST::Expression* expr, long long& result);

private:
    // ========================================================================
    // State
    // ========================================================================
    std::stringstream headerBuffer_;    // Target triple, data layout
    std::stringstream globalBuffer_;    // Global declarations (vars, struct types)
    std::stringstream stringBuffer_;    // String literals (emitted before functions)
    std::stringstream functionBuffer_;  // Current function body
    std::stringstream declBuffer_;      // Function declarations
    std::stringstream funcDefBuffer_;   // Function definitions
    std::stringstream metadataBuffer_;  // Module metadata (PIE, PIC flags, etc.)
    std::stringstream* currentBuffer_ = nullptr;
    
    int tempCounter_ = 0;
    int labelCounter_ = 0;
    int stringCounter_ = 0;
    int staticVarCounter_ = 0;  // Counter for unique static variable names
    
    bool hadError_ = false;
    bool is64bit_ = false;
    
    // Current function context
    AST::FunctionDecl* currentFunction_ = nullptr;
    std::string currentFunctionReturnType_;
    std::string returnLabel_;
    std::string returnValuePtr_;
    bool inGlobalScope_ = true;
    
    // Control flow
    std::stack<std::string> breakLabels_;
    std::stack<std::string> continueLabels_;
    
    // Switch statement support
    struct SwitchContext {
        std::string switchVar;
        std::string switchType;
        std::string defaultLabel;
        std::string endLabel;
        std::vector<std::pair<long long, std::string>> cases;
    };
    std::stack<SwitchContext> switchStack_;
    
    // Symbol tables (scoped)
    std::vector<std::map<std::string, IRSymbol>> scopes_;
    
    // Struct layouts cache
    std::map<std::string, StructLayout> structLayouts_;
    
    // Typedef cache (maps typedef name to its underlying type)
    std::map<std::string, AST::Type*> typedefMap_;
    
    // Enum values
    std::map<std::string, long long> enumValues_;
    
    // Expression result (set by expression visitors)
    IRValue lastValue_;
    
    // Label management for goto
    std::map<std::string, std::string> gotoLabels_;
    std::vector<std::string> pendingGotos_;  // gotos before label defined
    
    // Track which functions/globals have been declared to avoid duplicates
    std::set<std::string> declaredFunctions_;
    std::set<std::string> definedFunctions_;  // Track definitions separately
    std::set<std::string> declaredGlobals_;
    std::map<std::string, std::string> functionDeclarations_;  // name -> declaration line
};

} // namespace cc1
