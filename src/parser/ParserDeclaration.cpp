#include <parser/Parser.hpp>

namespace cc1 {

// ============================================================================
// Declaration Parsing
// ============================================================================

AST::Ptr<AST::Declaration> Parser::parseExternalDeclaration() {
    DeclSpecifiers specs = parseDeclarationSpecifiers();
    
    if (!specs.type) {
        error("expected type specifier");
    }
    
    // Check for control flow keywords used outside of functions
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
    
    // Check for function definition or declaration
    if (check(TokenType::Semicolon)) {
        // Standalone struct/union/enum definition (e.g., "struct Foo { int x; };")
        advance();
        
        // Check if this is a standalone struct/union definition
        if (auto* structType = dynamic_cast<AST::StructType*>(specs.type.get())) {
            if (structType->isComplete && !structType->name.empty()) {
                auto structDecl = AST::make<AST::StructDecl>(
                    structType->name, structType->isUnion,
                    structType->line, structType->column);
                for (auto& member : structType->members) {
                    auto memberDecl = AST::make<AST::VarDecl>(
                        member.name, member.type ? member.type->clone() : nullptr,
                        member.line, member.column);
                    structDecl->members.push_back(std::move(memberDecl));
                }
                return structDecl;
            }
        }
        
        // Check if this is a standalone enum definition
        // We keep the EnumType as-is and return a VarDecl with empty name
        // so the semantic analyzer can process the enum values
        if (auto* enumType = dynamic_cast<AST::EnumType*>(specs.type.get())) {
            if (!enumType->name.empty() || !enumType->enumerators.empty()) {
                // Create a dummy VarDecl with the enum type so semantic analysis processes it
                auto varDecl = AST::make<AST::VarDecl>("", std::move(specs.type), enumType->line, enumType->column);
                return varDecl;
            }
        }
        
        // Forward declaration or anonymous type (return nullptr)
        return nullptr;
    }
    
    Declarator decl = parseDeclarator(specs.type);
    
    // Function definition?
    if (check(TokenType::LeftBrace)) {
        // Check for redefinition at global scope only
        if (functionDepth_ == 0 && !decl.name.empty() && globalIdentifiers_.count(decl.name)) {
            std::string newType = decl.type ? decl.type->toString() : "unknown";
            std::string oldType = globalIdentifiers_[decl.name];
            errorAtPosition(decl.line, decl.column, "redefinition of '" + decl.name + "' with a different type: '" + newType + "' vs '" + oldType + "'");
        }
        if (functionDepth_ == 0 && !decl.name.empty()) {
            globalIdentifiers_[decl.name] = decl.type ? decl.type->toString() : "unknown";
        }
        return parseFunctionDefinition(specs, decl);
    }
    
    // Variable declaration or function prototype
    if (specs.isTypedef) {
        // Check for redefinition at global scope only
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
        
        // Handle multiple declarators
        while (match(TokenType::Comma)) {
            Declarator nextDecl = parseDeclarator(specs.type);
            // Check for redefinition at global scope only
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
    
    // Regular variable declaration - check for redefinition at global scope only
    if (functionDepth_ == 0 && !decl.name.empty() && globalIdentifiers_.count(decl.name)) {
        std::string newType = decl.type ? decl.type->toString() : "unknown";
        std::string oldType = globalIdentifiers_[decl.name];
        errorAtPosition(decl.line, decl.column, "redefinition of '" + decl.name + "' with a different type: '" + newType + "' vs '" + oldType + "'");
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
    
    // Handle multiple declarators
    while (match(TokenType::Comma)) {
        Declarator nextDecl = parseDeclarator(specs.type);
        // Check for redefinition at global scope only
        if (functionDepth_ == 0 && !nextDecl.name.empty() && globalIdentifiers_.count(nextDecl.name)) {
            std::string newType = nextDecl.type ? nextDecl.type->toString() : "unknown";
            std::string oldType = globalIdentifiers_[nextDecl.name];
            error("redefinition of '" + nextDecl.name + "' with a different type: '" + newType + "' vs '" + oldType + "'");
        }
        if (functionDepth_ == 0 && !nextDecl.name.empty()) {
            globalIdentifiers_[nextDecl.name] = nextDecl.type ? nextDecl.type->toString() : "unknown";
        }
        // TODO: Store additional declarations
        if (match(TokenType::Equal)) {
            parseInitializer(); // Parse but don't store yet
        }
    }
    
    consume(TokenType::Semicolon, "expected ';' after declaration");
    return var;
}

AST::Ptr<AST::Declaration> Parser::parseDeclaration() {
    return parseExternalDeclaration();
}

Parser::DeclSpecifiers Parser::parseDeclarationSpecifiers() {
    DeclSpecifiers specs;
    
    // Track type specifiers to build combined types
    bool hasVoid = false, hasChar = false, hasShort = false;
    bool hasInt = false, hasLong = false, hasFloat = false;
    bool hasDouble = false, hasSigned = false, hasUnsigned = false;
    int longCount = 0;
    bool hasTypeSpec = false;  // Track if we already have a type
    
    while (isDeclarationSpecifier()) {
        // Storage class specifiers - capture position before matching
        if (check(TokenType::Typedef)) {
            specs.storageClassLine = current().line;
            specs.storageClassColumn = current().column;
            advance();
            specs.isTypedef = true;
            specs.storageClass = AST::StorageClass::Typedef;
        } else if (check(TokenType::Extern)) {
            specs.storageClassLine = current().line;
            specs.storageClassColumn = current().column;
            advance();
            specs.storageClass = AST::StorageClass::Extern;
        } else if (check(TokenType::Static)) {
            specs.storageClassLine = current().line;
            specs.storageClassColumn = current().column;
            advance();
            specs.storageClass = AST::StorageClass::Static;
        } else if (check(TokenType::Auto)) {
            specs.storageClassLine = current().line;
            specs.storageClassColumn = current().column;
            advance();
            specs.storageClass = AST::StorageClass::Auto;
        } else if (check(TokenType::Register)) {
            specs.storageClassLine = current().line;
            specs.storageClassColumn = current().column;
            advance();
            specs.storageClass = AST::StorageClass::Register;
        }
        // Type qualifiers
        else if (match(TokenType::Const)) {
            specs.isConst = true;
        } else if (match(TokenType::Volatile)) {
            specs.isVolatile = true;
        }
        // Type specifiers - only one type specifier sequence allowed
        else if (!hasTypeSpec && match(TokenType::Void)) {
            hasVoid = true;
            hasTypeSpec = true;
        } else if (!hasTypeSpec && match(TokenType::Char)) {
            hasChar = true;
            hasTypeSpec = true;
        } else if (!hasTypeSpec && match(TokenType::Short)) {
            hasShort = true;
        } else if (!hasTypeSpec && match(TokenType::Int)) {
            hasInt = true;
            hasTypeSpec = true;
        } else if (match(TokenType::Long)) {
            hasLong = true;
            longCount++;
        } else if (!hasTypeSpec && match(TokenType::Float)) {
            hasFloat = true;
            hasTypeSpec = true;
        } else if (!hasTypeSpec && match(TokenType::Double)) {
            hasDouble = true;
            hasTypeSpec = true;
        } else if (match(TokenType::Signed)) {
            hasSigned = true;
        } else if (match(TokenType::Unsigned)) {
            hasUnsigned = true;
        }
        // Struct/Union
        else if (!hasTypeSpec && (check(TokenType::Struct) || check(TokenType::Union))) {
            specs.type = parseStructOrUnionSpecifier();
            hasTypeSpec = true;
        }
        // Enum
        else if (!hasTypeSpec && check(TokenType::Enum)) {
            specs.type = parseEnumSpecifier();
            hasTypeSpec = true;
        }
        // Typedef name
        else if (!hasTypeSpec && check(TokenType::Identifier)) {
            std::string name = current().value;
            if (typedefNames_.find(name) != typedefNames_.end()) {
                advance();
                specs.type = AST::make<AST::TypedefType>(name, 0, 0);
                hasTypeSpec = true;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    
    // Build the type if we parsed type specifiers
    if (!specs.type) {
        AST::PrimitiveKind kind = AST::PrimitiveKind::Int; // default
        
        if (hasVoid) {
            kind = AST::PrimitiveKind::Void;
        } else if (hasChar) {
            if (hasUnsigned) {
                kind = AST::PrimitiveKind::UnsignedChar;
            } else if (hasSigned) {
                kind = AST::PrimitiveKind::SignedChar;
            } else {
                kind = AST::PrimitiveKind::Char;
            }
        } else if (hasShort) {
            kind = hasUnsigned ? AST::PrimitiveKind::UnsignedShort 
                               : AST::PrimitiveKind::Short;
        } else if (hasLong) {
            if (longCount >= 2) {
                kind = hasUnsigned ? AST::PrimitiveKind::UnsignedLongLong 
                                   : AST::PrimitiveKind::LongLong;
            } else if (hasDouble) {
                kind = AST::PrimitiveKind::LongDouble;
            } else {
                kind = hasUnsigned ? AST::PrimitiveKind::UnsignedLong 
                                   : AST::PrimitiveKind::Long;
            }
        } else if (hasFloat) {
            kind = AST::PrimitiveKind::Float;
        } else if (hasDouble) {
            kind = AST::PrimitiveKind::Double;
        } else if (hasUnsigned) {
            kind = AST::PrimitiveKind::UnsignedInt;
        } else if (hasSigned || hasInt) {
            kind = AST::PrimitiveKind::Int;
        }
        
        specs.type = AST::make<AST::PrimitiveType>(kind, 0, 0);
    }
    
    // Apply qualifiers
    if (specs.isConst || specs.isVolatile) {
        specs.type = AST::make<AST::QualifiedType>(
            std::move(specs.type), specs.isConst, specs.isVolatile, 0, 0);
    }
    
    return specs;
}

bool Parser::isDeclarationSpecifier() const {
    return isStorageClassSpecifier() || isTypeSpecifier() || isTypeQualifier();
}

bool Parser::isTypeSpecifier() const {
    switch (current().type) {
        case TokenType::Void:
        case TokenType::Char:
        case TokenType::Short:
        case TokenType::Int:
        case TokenType::Long:
        case TokenType::Float:
        case TokenType::Double:
        case TokenType::Signed:
        case TokenType::Unsigned:
        case TokenType::Struct:
        case TokenType::Union:
        case TokenType::Enum:
            return true;
        case TokenType::Identifier:
            return typedefNames_.find(current().value) != typedefNames_.end();
        default:
            return false;
    }
}

bool Parser::isStorageClassSpecifier() const {
    switch (current().type) {
        case TokenType::Typedef:
        case TokenType::Extern:
        case TokenType::Static:
        case TokenType::Auto:
        case TokenType::Register:
            return true;
        default:
            return false;
    }
}

bool Parser::isTypeQualifier() const {
    return current().type == TokenType::Const || 
           current().type == TokenType::Volatile;
}

// ============================================================================
// Declarator Parsing
// ============================================================================

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

// ============================================================================
// Function Declaration Parsing
// ============================================================================

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
    
    // Check for parenthesized declarator (for function pointer parameters like `int (*)(long)`)
    if (check(TokenType::LeftParen)) {
        // Could be abstract parenthesized declarator or concrete declarator
        // Save position to potentially backtrack
        size_t savedPos = currentIndex_;
        advance(); // consume '('
        
        if (check(TokenType::Star) || check(TokenType::LeftParen)) {
            // Abstract declarator like (*) or ((*))
            AST::Ptr<AST::Type> innerType = parsePointer(type->clone());
            
            // Handle nested parentheses
            while (check(TokenType::LeftParen)) {
                size_t nestedPos = currentIndex_;
                advance();
                if (check(TokenType::Star) || check(TokenType::LeftParen)) {
                    innerType = parsePointer(std::move(innerType));
                } else {
                    currentIndex_ = nestedPos;
                    break;
                }
                consume(TokenType::RightParen, "expected ')' in abstract declarator");
            }
            
            consume(TokenType::RightParen, "expected ')' after abstract declarator");
            
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

// ============================================================================
// Struct/Union/Enum Parsing
// ============================================================================

AST::Ptr<AST::Type> Parser::parseStructOrUnionSpecifier() {
    bool isUnion = match(TokenType::Union);
    if (!isUnion) {
        consume(TokenType::Struct, "expected 'struct' or 'union'");
    }
    
    std::string name;
    int line = current().line;
    int col = current().column;
    
    if (check(TokenType::Identifier)) {
        name = current().value;
        advance();
    }
    
    // Check for struct definition
    if (match(TokenType::LeftBrace)) {
        auto structType = AST::make<AST::StructType>(name, isUnion, line, col);
        
        // Parse member list
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            DeclSpecifiers specs = parseDeclarationSpecifiers();
            
            if (!specs.type) {
                error("expected type in struct member");
            }
            
            // Storage class specifiers are not allowed in struct members
            if (specs.storageClass != AST::StorageClass::None || specs.isTypedef) {
                errorAtPosition(specs.storageClassLine, specs.storageClassColumn, "type name does not allow storage class to be specified");
            }
            
            do {
                Declarator decl = parseDeclarator(specs.type);
                
                // Check for bitfield - bad_semantic_6.c
                if (match(TokenType::Colon)) {
                    // Bitfield - check that the type is integral
                    AST::Type* baseType = decl.type.get();
                    // Strip qualifiers
                    if (auto* qual = dynamic_cast<AST::QualifiedType*>(baseType)) {
                        baseType = qual->baseType.get();
                    }
                    auto* prim = dynamic_cast<AST::PrimitiveType*>(baseType);
                    bool isIntegral = prim && (
                        prim->kind == AST::PrimitiveKind::Char ||
                        prim->kind == AST::PrimitiveKind::SignedChar ||
                        prim->kind == AST::PrimitiveKind::UnsignedChar ||
                        prim->kind == AST::PrimitiveKind::Short ||
                        prim->kind == AST::PrimitiveKind::UnsignedShort ||
                        prim->kind == AST::PrimitiveKind::Int ||
                        prim->kind == AST::PrimitiveKind::UnsignedInt ||
                        prim->kind == AST::PrimitiveKind::Long ||
                        prim->kind == AST::PrimitiveKind::UnsignedLong ||
                        prim->kind == AST::PrimitiveKind::LongLong ||
                        prim->kind == AST::PrimitiveKind::UnsignedLongLong
                    );
                    
                    if (!isIntegral) {
                        // Get the type name for error message
                        std::string typeName = decl.type ? decl.type->toString() : "unknown";
                        errorAtPosition(decl.line, decl.column,
                            "bit-field '" + decl.name + "' has non-integral type '" + typeName + "'");
                    }
                    
                    // Parse the bitfield width expression
                    parseAssignmentExpression(); // Discard for now
                }
                
                structType->members.emplace_back(decl.name, std::move(decl.type),
                                                  decl.line, decl.column);
            } while (match(TokenType::Comma));
            
            consume(TokenType::Semicolon, "expected ';' after struct member");
        }
        
        consume(TokenType::RightBrace, "expected '}' after struct members");
        structType->isComplete = true;
        return structType;
    }
    
    // Just a reference to a struct type
    return AST::make<AST::StructType>(name, isUnion, line, col);
}

std::vector<AST::Ptr<AST::VarDecl>> Parser::parseStructMemberList() {
    std::vector<AST::Ptr<AST::VarDecl>> members;
    
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        DeclSpecifiers specs = parseDeclarationSpecifiers();
        
        if (!specs.type) {
            error("expected type in struct member");
        }
        
        // Storage class specifiers are not allowed in struct members
        if (specs.storageClass != AST::StorageClass::None || specs.isTypedef) {
            errorAtPosition(specs.storageClassLine, specs.storageClassColumn, "type name does not allow storage class to be specified");
        }
        
        do {
            Declarator decl = parseDeclarator(specs.type);
            auto member = AST::make<AST::VarDecl>(decl.name, std::move(decl.type),
                                                   decl.line, decl.column);
            members.push_back(std::move(member));
        } while (match(TokenType::Comma));
        
        consume(TokenType::Semicolon, "expected ';' after struct member");
    }
    
    return members;
}

AST::Ptr<AST::Type> Parser::parseEnumSpecifier() {
    consume(TokenType::Enum, "expected 'enum'");
    
    std::string name;
    int line = current().line;
    int col = current().column;
    
    if (check(TokenType::Identifier)) {
        name = current().value;
        advance();
    }
    
    auto enumType = AST::make<AST::EnumType>(name, line, col);
    
    if (match(TokenType::LeftBrace)) {
        // Parse enumerator list
        if (!check(TokenType::RightBrace)) {
            do {
                Token nameToken = consume(TokenType::Identifier, 
                    "expected enumerator name");
                std::string enumName = nameToken.value;
                int enumLine = nameToken.line;
                int enumCol = nameToken.column;
                int equalCol = 0;
                
                AST::Ptr<AST::Expression> value;
                if (check(TokenType::Equal)) {
                    equalCol = current().column;
                    advance();  // consume '='
                    value = parseConstantExpression();
                }
                
                enumType->enumerators.push_back({enumName, std::move(value), enumLine, enumCol, equalCol});
            } while (match(TokenType::Comma) && !check(TokenType::RightBrace));
        }
        
        consume(TokenType::RightBrace, "expected '}' after enum");
    }
    
    return enumType;
}

// ============================================================================
// Initializer Parsing
// ============================================================================

AST::Ptr<AST::Expression> Parser::parseInitializer() {
    if (check(TokenType::LeftBrace)) {
        return parseInitializerList();
    }
    return parseAssignmentExpression();
}

AST::Ptr<AST::InitializerList> Parser::parseInitializerList() {
    int line = current().line;
    int col = current().column;
    consume(TokenType::LeftBrace, "expected '{'");
    
    auto initList = AST::make<AST::InitializerList>(line, col);
    
    if (!check(TokenType::RightBrace)) {
        do {
            initList->initializers.push_back(parseInitializer());
        } while (match(TokenType::Comma) && !check(TokenType::RightBrace));
    }
    
    consume(TokenType::RightBrace, "expected '}'");
    return initList;
}

} // namespace cc1
