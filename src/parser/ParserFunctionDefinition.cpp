#include <parser/Parser.hpp>

namespace cc1 {

AST::Ptr<AST::FunctionDecl> Parser::parseFunctionDefinition(
    const DeclSpecifiers& specs, const Declarator& decl) {

    // Check for nested function definition (not allowed in C89)
    if (functionDepth_ > 0) {
        error("function definition is not allowed here");
    }

    auto func = AST::make<AST::FunctionDecl>(decl.name, nullptr,
                                              decl.line, decl.column);

    // Extract return type from function type if available
    if (auto* funcType = dynamic_cast<AST::FunctionType*>(decl.type.get())) {
        func->returnType = std::move(funcType->returnType);
    } else {
        // decl.type is the return type itself
        // This happens when there's no function type wrapper
        func->returnType = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, 0, 0);
    }

    func->parameters = std::vector<AST::Ptr<AST::ParamDecl>>();
    for (auto& p : const_cast<Declarator&>(decl).params) {
        func->parameters.push_back(std::move(p));
    }
    func->isVariadic = decl.isVariadic;
    func->storageClass = specs.storageClass;
    func->isDefinition = true;

    // Parse function body with incremented depth
    functionDepth_++;
    func->body = parseCompoundStatement();
    functionDepth_--;

    return func;
}

std::vector<AST::Ptr<AST::ParamDecl>> Parser::parseParameterList(bool& isVariadic) {
    std::vector<AST::Ptr<AST::ParamDecl>> params;
    isVariadic = false;

    if (check(TokenType::RightParen)) {
        return params; // Empty parameter list
    }

    // Check for void parameter
    if (check(TokenType::Void) && peek().type == TokenType::RightParen) {
        advance(); // consume void
        return params;
    }

    do {
        if (match(TokenType::Ellipsis)) {
            isVariadic = true;
            break;
        }

        auto param = parseParameterDeclaration();
        if (param) {
            params.push_back(std::move(param));
        }
    } while (match(TokenType::Comma));

    return params;
}

AST::Ptr<AST::ParamDecl> Parser::parseParameterDeclaration() {
    DeclSpecifiers specs = parseDeclarationSpecifiers();

    if (!specs.type) {
        error("expected type in parameter declaration");
    }

    // Parameter can have abstract declarator (no name) or concrete declarator (with name)
    std::string name;
    int line = current().line;
    int col = current().column;

    // Handle pointer prefix
    AST::Ptr<AST::Type> type = parsePointer(std::move(specs.type));

    // Check for parenthesized declarator (for function pointer parameters like `int (*)(long)` or `int (*f)(long)`)
    if (check(TokenType::LeftParen)) {
        // Could be abstract parenthesized declarator or concrete declarator
        // Save position to potentially backtrack
        size_t savedPos = currentIndex_;
        advance(); // consume '('

        if (check(TokenType::Star) || check(TokenType::LeftParen)) {
            // Declarator like (*) or (*f) or ((*))
            AST::Ptr<AST::Type> innerType = parsePointer(type->clone());

            // Check for optional name after the pointer (like (*f))
            if (check(TokenType::Identifier)) {
                name = current().value;
                line = current().line;
                col = current().column;
                advance();
            }

            // Handle nested parentheses
            while (check(TokenType::LeftParen)) {
                size_t nestedPos = currentIndex_;
                advance();
                if (check(TokenType::Star) || check(TokenType::LeftParen)) {
                    innerType = parsePointer(std::move(innerType));
                    // Check for name in nested case
                    if (check(TokenType::Identifier) && name.empty()) {
                        name = current().value;
                        line = current().line;
                        col = current().column;
                        advance();
                    }
                } else {
                    currentIndex_ = nestedPos;
                    break;
                }
                consume(TokenType::RightParen, "expected ')' in declarator");
            }

            consume(TokenType::RightParen, "expected ')' after declarator");

            // Now check for function suffix
            if (match(TokenType::LeftParen)) {
                bool isVariadic = false;
                auto params = parseParameterList(isVariadic);
                consume(TokenType::RightParen, "expected ')' after parameters");

                std::vector<AST::Ptr<AST::Type>> paramTypes;
                for (const auto& param : params) {
                    if (param->type) {
                        paramTypes.push_back(param->type->clone());
                    }
                }
                type = AST::make<AST::FunctionType>(std::move(innerType), std::move(paramTypes), isVariadic, 0, 0);
            } else {
                type = std::move(innerType);
            }
        } else if (check(TokenType::Identifier)) {
            // Might be a named declarator like (foo) - but this is rare in parameters
            // Backtrack and use regular declarator parsing
            currentIndex_ = savedPos;
            Declarator decl = parseDirectDeclarator(type);
            name = decl.name;
            type = std::move(decl.type);
            line = decl.line;
            col = decl.column;
        } else {
            // Backtrack - it's probably function suffix on the main type
            currentIndex_ = savedPos;
        }
    }

    if (name.empty() && check(TokenType::Identifier)) {
        name = current().value;
        line = current().line;
        col = current().column;
        advance();

        // Check for array or function suffixes
        while (true) {
            if (match(TokenType::LeftBracket)) {
                AST::Ptr<AST::Expression> size;
                if (!check(TokenType::RightBracket)) {
                    size = parseConstantExpression();
                }
                consume(TokenType::RightBracket, "expected ']'");
                // Arrays decay to pointers in parameters
                type = AST::make<AST::PointerType>(std::move(type), 0, 0);
            } else if (match(TokenType::LeftParen)) {
                // Function parameter suffix
                bool isVariadic = false;
                auto params = parseParameterList(isVariadic);
                consume(TokenType::RightParen, "expected ')' after parameters");

                std::vector<AST::Ptr<AST::Type>> paramTypes;
                for (const auto& param : params) {
                    if (param->type) {
                        paramTypes.push_back(param->type->clone());
                    }
                }
                type = AST::make<AST::FunctionType>(std::move(type), std::move(paramTypes), isVariadic, 0, 0);
            } else {
                break;
            }
        }
    }

    return AST::make<AST::ParamDecl>(name, std::move(type), line, col);
}

} // namespace cc1
