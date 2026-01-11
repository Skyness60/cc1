#include <parser/Parser.hpp>

namespace cc1 {

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

} // namespace cc1
