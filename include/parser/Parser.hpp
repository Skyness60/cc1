#pragma once

#include <lexer/Token.hpp>
#include <parser/ast/AST.hpp>
#include <parser/ParseError.hpp>
#include <utils/SourceLocation.hpp>
#include <vector>
#include <string>
#include <set>
#include <map>

namespace cc1 {

/**
 * @brief Recursive descent parser for C89/ANSI C
 * 
 * Parses a stream of tokens into an Abstract Syntax Tree (AST).
 * Follows the C89 grammar with precedence-based expression parsing.
 */
class Parser {
public:
    /**
     * @brief Construct a parser
     * @param tokens The token stream from the lexer
     * @param filename Source filename for error reporting
     * @param source Source code for error context
     */
    Parser(const std::vector<Token>& tokens, 
           const std::string& filename,
           const std::string& source);
    
    /**
     * @brief Parse the entire translation unit
     * @return The AST root node
     * @throws ParseError on syntax errors
     */
    AST::Ptr<AST::TranslationUnit> parse();
    
    /**
     * @brief Check if any parse errors occurred
     * @return true if parsing encountered errors
     */
    bool hadError() const { return hadError_; }

private:
    // ========================================================================
    // Token Stream Management
    // ========================================================================
    
    const Token& current() const;
    const Token& peek(int offset = 1) const;
    const Token& previous() const;
    
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool checkAny(std::initializer_list<TokenType> types) const;
    
    const Token& advance();
    bool match(TokenType type);
    bool matchAny(std::initializer_list<TokenType> types);
    
    const Token& consume(TokenType type, const std::string& message);
    void synchronize();
    
    // ========================================================================
    // Error Handling
    // ========================================================================
    
    [[noreturn]] void error(const std::string& message);
    [[noreturn]] void errorAt(const Token& token, const std::string& message);
    [[noreturn]] void errorAtPosition(int line, int column, const std::string& message);
    
    // ========================================================================
    // Declaration Parsing
    // ========================================================================
    
    /**
     * @brief Declaration specifiers result
     * 
     * Aggregates storage class, type specifiers, and qualifiers
     * from a declaration specifier sequence.
     */
    struct DeclSpecifiers {
        AST::StorageClass storageClass = AST::StorageClass::None;
        AST::Ptr<AST::Type> type;
        bool isTypedef = false;
        bool isConst = false;
        bool isVolatile = false;
        int storageClassLine = 0;
        int storageClassColumn = 0;
    };
    
    /**
     * @brief Declarator parsing result
     * 
     * Contains the declared name and the complete type
     * after applying pointer, array, and function modifiers.
     */
    struct Declarator {
        std::string name;
        AST::Ptr<AST::Type> type;
        std::vector<AST::Ptr<AST::ParamDecl>> params;  // For function declarators
        bool isVariadic = false;
        int line = 0;
        int column = 0;
    };
    
    AST::Ptr<AST::Declaration> parseExternalDeclaration();
    AST::Ptr<AST::Declaration> parseDeclaration();
    std::vector<AST::Ptr<AST::Declaration>> parseDeclarations();
    
    DeclSpecifiers parseDeclarationSpecifiers();
    bool isDeclarationSpecifier() const;
    bool isTypeSpecifier() const;
    bool isStorageClassSpecifier() const;
    bool isTypeQualifier() const;
    
    Declarator parseDeclarator(const AST::Ptr<AST::Type>& baseType);
    Declarator parseDirectDeclarator(const AST::Ptr<AST::Type>& baseType);
    AST::Ptr<AST::Type> parsePointer(AST::Ptr<AST::Type> baseType);
    
    AST::Ptr<AST::FunctionDecl> parseFunctionDefinition(
        const DeclSpecifiers& specs, const Declarator& decl);
    std::vector<AST::Ptr<AST::ParamDecl>> parseParameterList(bool& isVariadic);
    AST::Ptr<AST::ParamDecl> parseParameterDeclaration();
    
    AST::Ptr<AST::Type> parseStructOrUnionSpecifier();
    AST::Ptr<AST::Type> parseEnumSpecifier();
    std::vector<AST::Ptr<AST::VarDecl>> parseStructMemberList();
    
    AST::Ptr<AST::Expression> parseInitializer();
    AST::Ptr<AST::InitializerList> parseInitializerList();
    
    // ========================================================================
    // Statement Parsing
    // ========================================================================
    
    AST::Ptr<AST::Statement> parseStatement();
    AST::Ptr<AST::CompoundStmt> parseCompoundStatement();
    AST::Ptr<AST::Statement> parseIfStatement();
    AST::Ptr<AST::Statement> parseWhileStatement();
    AST::Ptr<AST::Statement> parseDoWhileStatement();
    AST::Ptr<AST::Statement> parseForStatement();
    AST::Ptr<AST::Statement> parseSwitchStatement();
    AST::Ptr<AST::Statement> parseCaseStatement();
    AST::Ptr<AST::Statement> parseDefaultStatement();
    AST::Ptr<AST::Statement> parseReturnStatement();
    AST::Ptr<AST::Statement> parseGotoStatement();
    AST::Ptr<AST::Statement> parseBreakStatement();
    AST::Ptr<AST::Statement> parseContinueStatement();
    AST::Ptr<AST::Statement> parseLabeledStatement();
    AST::Ptr<AST::Statement> parseExpressionStatement();
    
    // ========================================================================
    // Expression Parsing (Precedence Climbing)
    // ========================================================================
    
    AST::Ptr<AST::Expression> parseExpression();
    AST::Ptr<AST::Expression> parseAssignmentExpression();
    AST::Ptr<AST::Expression> parseConditionalExpression();
    AST::Ptr<AST::Expression> parseLogicalOrExpression();
    AST::Ptr<AST::Expression> parseLogicalAndExpression();
    AST::Ptr<AST::Expression> parseBitwiseOrExpression();
    AST::Ptr<AST::Expression> parseBitwiseXorExpression();
    AST::Ptr<AST::Expression> parseBitwiseAndExpression();
    AST::Ptr<AST::Expression> parseEqualityExpression();
    AST::Ptr<AST::Expression> parseRelationalExpression();
    AST::Ptr<AST::Expression> parseShiftExpression();
    AST::Ptr<AST::Expression> parseAdditiveExpression();
    AST::Ptr<AST::Expression> parseMultiplicativeExpression();
    AST::Ptr<AST::Expression> parseCastExpression();
    AST::Ptr<AST::Expression> parseUnaryExpression();
    AST::Ptr<AST::Expression> parsePostfixExpression();
    AST::Ptr<AST::Expression> parsePrimaryExpression();
    AST::Ptr<AST::Expression> parsePrimaryIntegerLiteral();
    AST::Ptr<AST::Expression> parsePrimaryFloatLiteral();
    AST::Ptr<AST::Expression> parsePrimaryStringLiteral();
    AST::Ptr<AST::Expression> parsePrimaryCharLiteral();
    AST::Ptr<AST::Expression> parseSizeofExpression();
    AST::Ptr<AST::Expression> parseConstantExpression();
    
    // ========================================================================
    // Type Parsing
    // ========================================================================
    
    AST::Ptr<AST::Type> parseTypeName();
    AST::Ptr<AST::Type> parseAbstractDeclarator(AST::Ptr<AST::Type> baseType);
    
    // ========================================================================
    // Utility Methods
    // ========================================================================
    
    bool isTypeName() const;
    bool isDeclarationStart() const;
    
    AST::BinaryOp tokenToBinaryOp(TokenType type) const;
    AST::UnaryOp tokenToUnaryOp(TokenType type) const;
    AST::UnaryOp tokenToPostfixOp(TokenType type) const;
    
    std::string getSourceLine(int line) const;
    SourceLocation makeLocation(const Token& tok) const;
    
    // ========================================================================
    // Parser State
    // ========================================================================
    
    const std::vector<Token>& tokens_;
    std::string filename_;
    std::string source_;
    size_t currentIndex_ = 0;
    bool hadError_ = false;
    
    /// Typedef names seen so far (needed to distinguish types from identifiers)
    std::set<std::string> typedefNames_;
    
    /// Global identifiers declared with their types (for redefinition detection)
    std::map<std::string, std::string> globalIdentifiers_;
    
    /// Functions that have a body/definition (not just declaration)
    std::set<std::string> definedFunctions_;
    
    /// Additional declarations from multi-variable declarations (e.g., int a, b, c;)
    std::vector<AST::Ptr<AST::Declaration>> additionalDeclarations_;
    
    /// Context tracking for semantic validation during parsing
    int switchDepth_ = 0;     // Track nesting level inside switch statements
    int functionDepth_ = 0;   // Track nesting level inside function definitions
};

} // namespace cc1
