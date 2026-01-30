#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses a function definition, assembling signature and body.
// FR: Parse une definition de fonction, assemble signature et corps.
AST::Ptr<AST::FunctionDecl> Parser::parseFunctionDefinition(
    const DeclSpecifiers& specs, const Declarator& decl) {

    
    if (functionDepth_ > 0) {
        error("function definition is not allowed here");
    }

    auto func = AST::make<AST::FunctionDecl>(decl.name, nullptr,
                                              decl.line, decl.column);

    
    if (auto* funcType = dynamic_cast<AST::FunctionType*>(decl.type.get())) {
        func->returnType = std::move(funcType->returnType);
    } else {
        
        
        func->returnType = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, 0, 0);
    }

    func->parameters = std::vector<AST::Ptr<AST::ParamDecl>>();
    for (auto& p : const_cast<Declarator&>(decl).params) {
        func->parameters.push_back(std::move(p));
    }
    func->isVariadic = decl.isVariadic;
    func->storageClass = specs.storageClass;
    func->isDefinition = true;

    
    functionDepth_++;
    func->body = parseCompoundStatement();
    functionDepth_--;

    return func;
}

// EN: Parses a parameter list, handling void and variadic markers.
// FR: Parse une liste de parametres avec gestion de void et variadique.
std::vector<AST::Ptr<AST::ParamDecl>> Parser::parseParameterList(bool& isVariadic) {
    std::vector<AST::Ptr<AST::ParamDecl>> params;
    isVariadic = false;

    if (check(TokenType::RightParen)) {
        return params; 
    }

    
    if (check(TokenType::Void) && peek().type == TokenType::RightParen) {
        advance(); 
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

// EN: Parses a single parameter declaration, including function-pointer cases.
// FR: Parse une declaration de parametre, y compris pointeurs de fonction.
AST::Ptr<AST::ParamDecl> Parser::parseParameterDeclaration() {
    DeclSpecifiers specs = parseDeclarationSpecifiers();

    if (!specs.type) {
        error("expected type in parameter declaration");
    }

    
    std::string name;
    int line = current().line;
    int col = current().column;

    
    AST::Ptr<AST::Type> type = parsePointer(std::move(specs.type));

    
    if (check(TokenType::LeftParen)) {
        
        
        size_t savedPos = currentIndex_;
        advance(); 

        if (check(TokenType::Star) || check(TokenType::LeftParen)) {
            
            AST::Ptr<AST::Type> innerType = parsePointer(type->clone());

            
            if (check(TokenType::Identifier)) {
                name = current().value;
                line = current().line;
                col = current().column;
                advance();
            }

            
            while (check(TokenType::LeftParen)) {
                size_t nestedPos = currentIndex_;
                advance();
                if (check(TokenType::Star) || check(TokenType::LeftParen)) {
                    innerType = parsePointer(std::move(innerType));
                    
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
            
            
            currentIndex_ = savedPos;
            Declarator decl = parseDirectDeclarator(type);
            name = decl.name;
            type = std::move(decl.type);
            line = decl.line;
            col = decl.column;
        } else {
            
            currentIndex_ = savedPos;
        }
    }

    if (name.empty() && check(TokenType::Identifier)) {
        name = current().value;
        line = current().line;
        col = current().column;
        advance();

        
        while (true) {
            if (match(TokenType::LeftBracket)) {
                AST::Ptr<AST::Expression> size;
                if (!check(TokenType::RightBracket)) {
                    size = parseConstantExpression();
                }
                consume(TokenType::RightBracket, "expected ']'");
                
                type = AST::make<AST::PointerType>(std::move(type), 0, 0);
            } else if (match(TokenType::LeftParen)) {
                
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

} 

// TODO(cc1) EN: Simplify complex parameter declarator parsing.
// FR: Simplifier le parsing complexe des declarators de parametres.
