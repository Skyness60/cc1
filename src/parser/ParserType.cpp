#include <parser/Parser.hpp>

namespace cc1 {





// EN: Parses a type name used in casts/sizeof.
// FR: Parse un type name utilise dans cast/sizeof.
AST::Ptr<AST::Type> Parser::parseTypeName() {
    DeclSpecifiers specs = parseDeclarationSpecifiers();
    
    if (!specs.type) {
        error("expected type name");
    }
    
    return parseAbstractDeclarator(std::move(specs.type));
}

// EN: Parses an abstract declarator (no identifier) for type-only constructs.
// FR: Parse un declarator abstrait (sans identifiant) pour types seuls.
AST::Ptr<AST::Type> Parser::parseAbstractDeclarator(AST::Ptr<AST::Type> baseType) {
    
    baseType = parsePointer(std::move(baseType));
    
    
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





// EN: Checks whether the current token can start a type name.
// FR: Verifie si le token courant peut demarrer un type name.
bool Parser::isTypeName() const {
    return isTypeSpecifier() || isTypeQualifier();
}

// EN: Checks whether the current token can start a declaration.
// FR: Verifie si le token courant peut demarrer une declaration.
bool Parser::isDeclarationStart() const {
    return isDeclarationSpecifier();
}

} 

// TODO(cc1) EN: Support abstract declarators with nested parentheses.
// FR: Supporter les declarators abstraits avec parentheses imbriquees.
