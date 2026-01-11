#include <parser/Parser.hpp>

namespace cc1 {

// ============================================================================
// Expression Parsing
// ============================================================================

AST::Ptr<AST::Expression> Parser::parseExpression() {
    auto expr = parseAssignmentExpression();

    while (match(TokenType::Comma)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseAssignmentExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::Comma, std::move(expr),
                                          std::move(right), line, col);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseAssignmentExpression() {
    auto expr = parseConditionalExpression();

    if (checkAny({TokenType::Equal, TokenType::PlusEqual, TokenType::MinusEqual,
                  TokenType::StarEqual, TokenType::SlashEqual, TokenType::PercentEqual,
                  TokenType::AmpersandEqual, TokenType::PipeEqual, TokenType::CaretEqual,
                  TokenType::LeftShiftEqual, TokenType::RightShiftEqual})) {

        Token op = current();
        advance();
        auto right = parseAssignmentExpression();

        AST::BinaryOp binOp = tokenToBinaryOp(op.type);
        return AST::make<AST::BinaryExpr>(binOp, std::move(expr), std::move(right),
                                          op.line, op.column);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseConditionalExpression() {
    auto expr = parseLogicalOrExpression();

    if (match(TokenType::Question)) {
        // Use position of condition expression, not '?' operator
        int line = expr->line;
        int col = expr->column;
        auto thenExpr = parseExpression();
        consume(TokenType::Colon, "expected ':' in conditional expression");
        auto elseExpr = parseConditionalExpression();

        return AST::make<AST::TernaryExpr>(std::move(expr), std::move(thenExpr),
                                           std::move(elseExpr), line, col);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseConstantExpression() {
    return parseConditionalExpression();
}

} // namespace cc1
