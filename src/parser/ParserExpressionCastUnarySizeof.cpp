#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses cast expressions, disambiguating from parenthesized expressions.
// FR: Parse les expressions de cast, en distinguant les parentheses simples.
AST::Ptr<AST::Expression> Parser::parseCastExpression() {
    
    if (check(TokenType::LeftParen)) {
        
        size_t saved = currentIndex_;
        advance(); 

        if (isTypeName()) {
            auto type = parseTypeName();
            consume(TokenType::RightParen, "expected ')' after type name");

            int line = previous().line;
            int col = previous().column;
            auto operand = parseCastExpression();

            return AST::make<AST::CastExpr>(std::move(type), std::move(operand), line, col);
        }

        
        currentIndex_ = saved;
    }

    return parseUnaryExpression();
}

// EN: Parses unary expressions including prefix operators and sizeof.
// FR: Parse les expressions unaires avec operateurs prefixes et sizeof.
AST::Ptr<AST::Expression> Parser::parseUnaryExpression() {
    
    if (matchAny({TokenType::PlusPlus, TokenType::MinusMinus})) {
        Token op = previous();
        auto operand = parseUnaryExpression();
        AST::UnaryOp unaryOp = (op.type == TokenType::PlusPlus) ? AST::UnaryOp::PreIncrement
                                                                : AST::UnaryOp::PreDecrement;
        return AST::make<AST::UnaryExpr>(unaryOp, std::move(operand), true, op.line, op.column);
    }

    if (matchAny({TokenType::Ampersand, TokenType::Star, TokenType::Plus, TokenType::Minus,
                  TokenType::Tilde, TokenType::Exclamation})) {
        Token op = previous();
        auto operand = parseCastExpression();
        AST::UnaryOp unaryOp = tokenToUnaryOp(op.type);
        return AST::make<AST::UnaryExpr>(unaryOp, std::move(operand), true, op.line, op.column);
    }

    if (match(TokenType::Sizeof)) {
        return parseSizeofExpression();
    }

    return parsePostfixExpression();
}

// EN: Parses sizeof expressions for types or expressions.
// FR: Parse les expressions sizeof pour types ou expressions.
AST::Ptr<AST::Expression> Parser::parseSizeofExpression() {
    int line = previous().line;
    int col = previous().column;

    if (match(TokenType::LeftParen)) {
        if (isTypeName()) {
            auto type = parseTypeName();
            consume(TokenType::RightParen, "expected ')' after type name");
            return AST::make<AST::SizeofExpr>(std::move(type), line, col);
        }

        
        auto expr = parseExpression();
        consume(TokenType::RightParen, "expected ')'");
        return AST::make<AST::SizeofExpr>(std::move(expr), line, col);
    }

    
    auto expr = parseUnaryExpression();
    return AST::make<AST::SizeofExpr>(std::move(expr), line, col);
}

} 

// TODO(cc1) EN: Handle GNU typeof or _Alignof if supported.
// FR: Gérer typeof GNU ou _Alignof si supporte.
