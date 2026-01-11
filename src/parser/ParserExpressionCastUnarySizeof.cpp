#include <parser/Parser.hpp>

namespace cc1 {

AST::Ptr<AST::Expression> Parser::parseCastExpression() {
    // Check for cast: (type-name) cast-expression
    if (check(TokenType::LeftParen)) {
        // Look ahead to see if this is a cast
        size_t saved = currentIndex_;
        advance(); // consume '('

        if (isTypeName()) {
            auto type = parseTypeName();
            consume(TokenType::RightParen, "expected ')' after type name");

            int line = previous().line;
            int col = previous().column;
            auto operand = parseCastExpression();

            return AST::make<AST::CastExpr>(std::move(type), std::move(operand), line, col);
        }

        // Not a cast, backtrack
        currentIndex_ = saved;
    }

    return parseUnaryExpression();
}

AST::Ptr<AST::Expression> Parser::parseUnaryExpression() {
    // Prefix operators
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

AST::Ptr<AST::Expression> Parser::parseSizeofExpression() {
    int line = previous().line;
    int col = previous().column;

    if (match(TokenType::LeftParen)) {
        if (isTypeName()) {
            auto type = parseTypeName();
            consume(TokenType::RightParen, "expected ')' after type name");
            return AST::make<AST::SizeofExpr>(std::move(type), line, col);
        }

        // sizeof (expression)
        auto expr = parseExpression();
        consume(TokenType::RightParen, "expected ')'");
        return AST::make<AST::SizeofExpr>(std::move(expr), line, col);
    }

    // sizeof unary-expression
    auto expr = parseUnaryExpression();
    return AST::make<AST::SizeofExpr>(std::move(expr), line, col);
}

} // namespace cc1
