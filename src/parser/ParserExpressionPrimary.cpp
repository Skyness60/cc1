#include <parser/Parser.hpp>

namespace cc1 {

AST::Ptr<AST::Expression> Parser::parsePrimaryExpression() {
    if (check(TokenType::IntegerLiteral)) {
        return parsePrimaryIntegerLiteral();
    }

    if (check(TokenType::FloatLiteral)) {
        return parsePrimaryFloatLiteral();
    }

    if (check(TokenType::StringLiteral)) {
        return parsePrimaryStringLiteral();
    }

    if (check(TokenType::CharLiteral)) {
        return parsePrimaryCharLiteral();
    }

    if (check(TokenType::Identifier)) {
        Token tok = current();
        advance();
        return AST::make<AST::Identifier>(tok.value, tok.line, tok.column);
    }

    if (match(TokenType::LeftParen)) {
        auto expr = parseExpression();
        consume(TokenType::RightParen, "expected ')' after expression");
        return expr;
    }

    error("expected statement");
}

} // namespace cc1
