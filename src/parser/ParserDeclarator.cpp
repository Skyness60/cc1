#include <utility>
#include <functional>
#include <parser/Parser.hpp>

namespace {

template <typename Builder>
AST::Ptr<AST::Type> wrapDerived(const AST::Ptr<AST::Type>& typeNode, const Builder& builder) {
    if (auto qualified = dynamic_cast<AST::QualifiedType*>(typeNode.get())) {
        auto rebuilt = wrapDerived(qualified->baseType, builder);
        return AST::make<AST::QualifiedType>(std::move(rebuilt),
                                             qualified->isConst,
                                             qualified->isVolatile,
                                             qualified->line,
                                             qualified->column);
    }

    if (auto pointer = dynamic_cast<AST::PointerType*>(typeNode.get())) {
        auto rebuilt = wrapDerived(pointer->pointee, builder);
        return AST::make<AST::PointerType>(std::move(rebuilt),
                                           pointer->line,
                                           pointer->column);
    }

    if (auto array = dynamic_cast<AST::ArrayType*>(typeNode.get())) {
        auto rebuilt = wrapDerived(array->elementType, builder);
        if (array->size >= 0) {
            return AST::make<AST::ArrayType>(std::move(rebuilt),
                                             array->size,
                                             array->line,
                                             array->column);
        }
        return AST::make<AST::ArrayType>(std::move(rebuilt),
                                         nullptr,
                                         array->line,
                                         array->column);
    }

    return builder(typeNode->clone());
}

// C declarator rule fix for function pointers (parenthesized only):
//
// The rewrite must apply only to declarators like:   char (*f)(int)
// where the pointer is parenthesized and applies to the declarator `f`.
//
// It must NOT apply to:   char *f(int)
// which is a function returning a pointer (return type is char*).
//
// This helper rewrites:
//   PointerType(T) + function-suffix ( ... )  ==>  PointerType(FunctionType(T, ...))
// but ONLY when the pointer came from a parenthesized declarator.
static AST::Ptr<AST::Type> makeFunctionDerivedForParenPointer(AST::Ptr<AST::Type> base,
                                                              std::vector<AST::Ptr<AST::Type>> paramTypes,
                                                              bool isVariadic,
                                                              bool wasParenthesized) {
    if (wasParenthesized) {
        if (auto* pointer = dynamic_cast<AST::PointerType*>(base.get())) {
            AST::Ptr<AST::Type> pointee = pointer->pointee->clone();
            AST::Ptr<AST::Type> func = AST::make<AST::FunctionType>(std::move(pointee),
                                                                    std::move(paramTypes),
                                                                    isVariadic, 0, 0);
            return AST::make<AST::PointerType>(std::move(func), 0, 0);
        }
    }

    return AST::make<AST::FunctionType>(std::move(base),
                                        std::move(paramTypes),
                                        isVariadic, 0, 0);
}

}  // namespace

namespace cc1 {

// EN: Parses a declarator including pointer qualifiers and direct parts.
// FR: Parse un declarator avec pointeurs/qualifiers et parties directes.
Parser::Declarator Parser::parseDeclarator(const AST::Ptr<AST::Type>& baseType) {
    AST::Ptr<AST::Type> type = parsePointer(baseType->clone());
    return parseDirectDeclarator(type);
}

// EN: Parses the direct declarator: name, arrays, and function params.
// FR: Parse le declarator direct : nom, tableaux, params de fonction.
Parser::Declarator Parser::parseDirectDeclarator(const AST::Ptr<AST::Type>& baseType) {
    Declarator decl;
    decl.type = baseType->clone();
    bool wasParenthesized = false;

    
    if (match(TokenType::LeftParen)) {
        
        
        
        if (check(TokenType::Star) ||
            (!check(TokenType::RightParen) && !isTypeName() && check(TokenType::Identifier)) ||
            check(TokenType::LeftParen)) {
            
            wasParenthesized = true;
            decl = parseDeclarator(decl.type);
            consume(TokenType::RightParen, "expected ')' after declarator");

            
        } else {
            
            currentIndex_--;
        }
    }

    
    if (decl.name.empty() && check(TokenType::Identifier)) {
        decl.name = current().value;
        decl.line = current().line;
        decl.column = current().column;
        advance();
    }

    
    auto applyDerived = [&](const std::function<AST::Ptr<AST::Type>(AST::Ptr<AST::Type>)>& builder) {
        if (wasParenthesized) {
            decl.type = wrapDerived(decl.type, builder);
        } else {
            decl.type = builder(std::move(decl.type));
        }
    };

    while (true) {
        if (match(TokenType::LeftBracket)) {
            
            AST::Ptr<AST::Expression> size;
            if (!check(TokenType::RightBracket)) {
                size = parseConstantExpression();
            }
            consume(TokenType::RightBracket, "expected ']' after array dimension");

            auto makeArray = [&](AST::Ptr<AST::Type> base) {
                return AST::make<AST::ArrayType>(std::move(base),
                                                 std::move(size), 0, 0);
            };
            applyDerived(makeArray);
        } else if (match(TokenType::LeftParen)) {
            
            bool isVariadic = false;
            auto params = parseParameterList(isVariadic);
            consume(TokenType::RightParen, "expected ')' after parameters");

            decl.params = std::move(params);
            decl.isVariadic = isVariadic;

            
            std::vector<AST::Ptr<AST::Type>> paramTypes;
            for (const auto& param : decl.params) {
                if (param->type) {
                    paramTypes.push_back(param->type->clone());
                }
            }

            auto makeFunc = [&](AST::Ptr<AST::Type> base) {
                return makeFunctionDerivedForParenPointer(std::move(base),
                                                          std::move(paramTypes),
                                                          isVariadic,
                                                          wasParenthesized);
            };
            applyDerived(makeFunc);
        } else {
            break;
        }
    }

    return decl;
}

// EN: Parses pointer '*' chains with optional qualifiers.
// FR: Parse les chaines de pointeurs '*' avec qualifiers optionnels.
AST::Ptr<AST::Type> Parser::parsePointer(AST::Ptr<AST::Type> baseType) {
    while (match(TokenType::Star)) {
        bool isConst = false;
        bool isVolatile = false;

        
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

} 

// TODO(cc1) EN: Handle declarators for function pointers more explicitly.
// FR: Gerer plus explicitement les declarators de pointeurs de fonction.