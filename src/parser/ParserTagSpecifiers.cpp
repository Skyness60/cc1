#include <parser/Parser.hpp>

namespace cc1 {

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

                    // Parse the bitfield width expression and evaluate it
                    int bitWidth = 0;
                    auto widthExpr = parseAssignmentExpression();
                    if (widthExpr) {
                        // Try to evaluate the expression as a constant
                        if (auto* constExpr = dynamic_cast<AST::IntegerLiteral*>(widthExpr.get())) {
                            bitWidth = static_cast<int>(constExpr->value);
                        }
                        // If not a simple constant, bitWidth stays 0 (error handling)
                    }

                    structType->members.emplace_back(decl.name, std::move(decl.type),
                                                      decl.line, decl.column, bitWidth);
                } else {
                    structType->members.emplace_back(decl.name, std::move(decl.type),
                                                      decl.line, decl.column);
                }
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

} // namespace cc1
