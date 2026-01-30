#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses a top-level declaration/definition and enforces global rules.
// FR: Parse une declaration/definition de haut niveau et applique les regles globales.
AST::Ptr<AST::Declaration> Parser::parseExternalDeclaration() {
    DeclSpecifiers specs = parseDeclarationSpecifiers();

    if (!specs.type) {
        error("expected type specifier");
    }

    
    if (functionDepth_ == 0) {
        if (check(TokenType::While)) {
            error("while loop outside of a function");
        } else if (check(TokenType::For)) {
            error("for loop outside of a function");
        } else if (check(TokenType::If)) {
            error("if statement outside of a function");
        } else if (check(TokenType::Do)) {
            error("do statement outside of a function");
        } else if (check(TokenType::Switch)) {
            error("switch statement outside of a function");
        } else if (check(TokenType::Return)) {
            error("return statement outside of a function");
        } else if (check(TokenType::Break)) {
            error("break statement outside of a function");
        } else if (check(TokenType::Continue)) {
            error("continue statement outside of a function");
        } else if (check(TokenType::Goto)) {
            error("goto statement outside of a function");
        }
    }

    
    if (check(TokenType::Semicolon)) {
        
        advance();

        
        if (auto* structType = dynamic_cast<AST::StructType*>(specs.type.get())) {
            if (structType->isComplete && !structType->name.empty()) {
                auto structDecl = AST::make<AST::StructDecl>(
                    structType->name, structType->isUnion,
                    structType->line, structType->column);

                
                {
                    auto cloned = structType->clone();
                    structDecl->declaredType = AST::Ptr<AST::StructType>(
                        dynamic_cast<AST::StructType*>(cloned.release()));
                }

                for (auto& member : structType->members) {
                    auto memberDecl = AST::make<AST::VarDecl>(
                        member.name, member.type ? member.type->clone() : nullptr,
                        member.line, member.column);
                    structDecl->members.push_back(std::move(memberDecl));
                }
                return structDecl;
            }
        }

        
        
        
        if (auto* enumType = dynamic_cast<AST::EnumType*>(specs.type.get())) {
            if (!enumType->name.empty() || !enumType->enumerators.empty()) {
                
                auto varDecl = AST::make<AST::VarDecl>("", std::move(specs.type), enumType->line, enumType->column);
                return varDecl;
            }
        }

        
        return nullptr;
    }

    Declarator decl = parseDeclarator(specs.type);

    
    if (check(TokenType::LeftBrace)) {
        
        
        if (functionDepth_ == 0 && !decl.name.empty()) {
            if (definedFunctions_.count(decl.name)) {
                errorAtPosition(decl.line, decl.column, "redefinition of function '" + decl.name + "'");
            } else if (globalIdentifiers_.count(decl.name)) {
                
                std::string newType = decl.type ? decl.type->toString() : "unknown";
                std::string oldType = globalIdentifiers_[decl.name];
                if (newType != oldType) {
                    errorAtPosition(decl.line, decl.column, "conflicting types for '" + decl.name + "': '" + newType + "' vs '" + oldType + "'");
                }
            }
            globalIdentifiers_[decl.name] = decl.type ? decl.type->toString() : "unknown";
            definedFunctions_.insert(decl.name);
        }
        return parseFunctionDefinition(specs, decl);
    }

    
    if (specs.isTypedef) {
        
        if (functionDepth_ == 0 && !decl.name.empty() && globalIdentifiers_.count(decl.name)) {
            errorAtPosition(decl.line, decl.column, "redefinition of '" + decl.name + "' as different kind of symbol");
        }

        typedefNames_.insert(decl.name);
        if (functionDepth_ == 0 && !decl.name.empty()) {
            globalIdentifiers_[decl.name] = "typedef " + (decl.type ? decl.type->toString() : "unknown");
        }
        auto var = AST::make<AST::VarDecl>(decl.name, std::move(decl.type),
                                            decl.line, decl.column);
        var->storageClass = AST::StorageClass::Typedef;

        
        while (match(TokenType::Comma)) {
            Declarator nextDecl = parseDeclarator(specs.type);
            
            if (functionDepth_ == 0 && !nextDecl.name.empty() && globalIdentifiers_.count(nextDecl.name)) {
                error("redefinition of '" + nextDecl.name + "' as different kind of symbol");
            }
            typedefNames_.insert(nextDecl.name);
            if (functionDepth_ == 0 && !nextDecl.name.empty()) {
                globalIdentifiers_[nextDecl.name] = "typedef " + (nextDecl.type ? nextDecl.type->toString() : "unknown");
            }
        }

        consume(TokenType::Semicolon, "expected ';' after typedef");
        return var;
    }

    
    
    if (functionDepth_ == 0 && !decl.name.empty() && globalIdentifiers_.count(decl.name)) {
        std::string newType = decl.type ? decl.type->toString() : "unknown";
        std::string oldType = globalIdentifiers_[decl.name];
        if (newType != oldType) {
            errorAtPosition(decl.line, decl.column, "redefinition of '" + decl.name + "' with a different type: '" + newType + "' vs '" + oldType + "'");
        }
    }
    if (functionDepth_ == 0 && !decl.name.empty()) {
        globalIdentifiers_[decl.name] = decl.type ? decl.type->toString() : "unknown";
    }

    auto var = AST::make<AST::VarDecl>(decl.name, std::move(decl.type),
                                        decl.line, decl.column);
    var->storageClass = specs.storageClass;

    if (match(TokenType::Equal)) {
        var->initializer = parseInitializer();
    }

    
    additionalDeclarations_.clear();

    
    while (match(TokenType::Comma)) {
        Declarator nextDecl = parseDeclarator(specs.type);
        
        
        if (functionDepth_ == 0 && !nextDecl.name.empty() && globalIdentifiers_.count(nextDecl.name)) {
            std::string newType = nextDecl.type ? nextDecl.type->toString() : "unknown";
            std::string oldType = globalIdentifiers_[nextDecl.name];
            if (newType != oldType) {
                error("redefinition of '" + nextDecl.name + "' with a different type: '" + newType + "' vs '" + oldType + "'");
            }
        }
        if (functionDepth_ == 0 && !nextDecl.name.empty()) {
            globalIdentifiers_[nextDecl.name] = nextDecl.type ? nextDecl.type->toString() : "unknown";
        }

        
        auto nextVar = AST::make<AST::VarDecl>(nextDecl.name, std::move(nextDecl.type),
                                                nextDecl.line, nextDecl.column);
        nextVar->storageClass = specs.storageClass;

        if (match(TokenType::Equal)) {
            nextVar->initializer = parseInitializer();
        }

        additionalDeclarations_.push_back(std::move(nextVar));
    }

    consume(TokenType::Semicolon, "expected ';' after declaration");
    return var;
}

// TODO(cc1) EN: Improve diagnostics for conflicting declarations across scopes.
// FR: Ameliorer les diagnostics pour declarations en conflit entre scopes.
} 
