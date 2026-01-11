#include <parser/Parser.hpp>

namespace cc1 {

Parser::Declarator Parser::parseDeclarator(const AST::Ptr<AST::Type>& baseType) {
    AST::Ptr<AST::Type> type = parsePointer(baseType->clone());
    return parseDirectDeclarator(type);
}

Parser::Declarator Parser::parseDirectDeclarator(const AST::Ptr<AST::Type>& baseType) {
    Declarator decl;
    decl.type = baseType->clone();

    // Handle parenthesized declarator
    if (match(TokenType::LeftParen)) {
        // This could be a parenthesized declarator or a function
        // A parenthesized declarator starts with: *, identifier, or (
        // Function parameters start with: ), type-specifier, or void
        if (check(TokenType::Star) ||
            (!check(TokenType::RightParen) && !isTypeName() && check(TokenType::Identifier)) ||
            check(TokenType::LeftParen)) {
            // Parenthesized declarator - recursively parse
            decl = parseDeclarator(decl.type);
            consume(TokenType::RightParen, "expected ')' after declarator");

            // After the parenthesized declarator, we may have more suffixes
            // like function parameters or array dimensions
        } else {
            // Looks like function parameters - backtrack
            currentIndex_--;
        }
    }

    // Get the identifier name
    if (check(TokenType::Identifier)) {
        decl.name = current().value;
        decl.line = current().line;
        decl.column = current().column;
        advance();
    }

    // Parse array dimensions and function parameters
    while (true) {
        if (match(TokenType::LeftBracket)) {
            // Array declarator
            AST::Ptr<AST::Expression> size;
            if (!check(TokenType::RightBracket)) {
                size = parseConstantExpression();
            }
            consume(TokenType::RightBracket, "expected ']' after array dimension");
            decl.type = AST::make<AST::ArrayType>(std::move(decl.type),
                                                   std::move(size), 0, 0);
        } else if (match(TokenType::LeftParen)) {
            // Function declarator
            bool isVariadic = false;
            auto params = parseParameterList(isVariadic);
            consume(TokenType::RightParen, "expected ')' after parameters");

            decl.params = std::move(params);
            decl.isVariadic = isVariadic;

            // Create function type with parameter types
            std::vector<AST::Ptr<AST::Type>> paramTypes;
            for (const auto& param : decl.params) {
                if (param->type) {
                    paramTypes.push_back(param->type->clone());
                }
            }
            decl.type = AST::make<AST::FunctionType>(std::move(decl.type),
                                                      std::move(paramTypes),
                                                      isVariadic, 0, 0);
        } else {
            break;
        }
    }

    return decl;
}

AST::Ptr<AST::Type> Parser::parsePointer(AST::Ptr<AST::Type> baseType) {
    while (match(TokenType::Star)) {
        bool isConst = false;
        bool isVolatile = false;

        // Parse pointer qualifiers
        while (true) {
            if (match(TokenType::Const)) {
                isConst = true;
            } else if (match(TokenType::Volatile)) {
                isVolatile = true;
            } else {
                break;
            }
        }

        baseType = AST::make<AST::PointerType>(std::move(baseType), 0, 0);

        if (isConst || isVolatile) {
            baseType = AST::make<AST::QualifiedType>(
                std::move(baseType), isConst, isVolatile, 0, 0);
        }
    }
    return baseType;
}

} // namespace cc1
