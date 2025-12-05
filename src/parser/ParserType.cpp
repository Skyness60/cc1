#include <parser/Parser.hpp>

namespace cc1 {

// ============================================================================
// Type Parsing
// ============================================================================

AST::Ptr<AST::Type> Parser::parseTypeName() {
    DeclSpecifiers specs = parseDeclarationSpecifiers();
    
    if (!specs.type) {
        error("expected type name");
    }
    
    return parseAbstractDeclarator(std::move(specs.type));
}

AST::Ptr<AST::Type> Parser::parseAbstractDeclarator(AST::Ptr<AST::Type> baseType) {
    // Parse pointer part
    baseType = parsePointer(std::move(baseType));
    
    // Parse direct abstract declarator (arrays, functions)
    while (true) {
        if (match(TokenType::LeftBracket)) {
            AST::Ptr<AST::Expression> size;
            if (!check(TokenType::RightBracket)) {
                size = parseConstantExpression();
            }
            consume(TokenType::RightBracket, "expected ']'");
            baseType = AST::make<AST::ArrayType>(std::move(baseType),
                                                  std::move(size), 0, 0);
        } else if (match(TokenType::LeftParen)) {
            // Function type
            bool isVariadic = false;
            auto params = parseParameterList(isVariadic);
            consume(TokenType::RightParen, "expected ')'");
            
            std::vector<AST::Ptr<AST::Type>> paramTypes;
            baseType = AST::make<AST::FunctionType>(std::move(baseType),
                                                     std::move(paramTypes),
                                                     isVariadic, 0, 0);
        } else {
            break;
        }
    }
    
    return baseType;
}

// ============================================================================
// Type Utility Methods
// ============================================================================

bool Parser::isTypeName() const {
    return isTypeSpecifier() || isTypeQualifier();
}

bool Parser::isDeclarationStart() const {
    return isDeclarationSpecifier();
}

} // namespace cc1
