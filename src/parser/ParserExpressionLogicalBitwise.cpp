#include <parser/Parser.hpp>

namespace cc1 {

AST::Ptr<AST::Expression> Parser::parseLogicalOrExpression() {
    auto expr = parseLogicalAndExpression();

    while (match(TokenType::PipePipe)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseLogicalAndExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::LogicalOr, std::move(expr),
                                          std::move(right), line, col);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseLogicalAndExpression() {
    auto expr = parseBitwiseOrExpression();

    while (match(TokenType::AmpersandAmpersand)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseBitwiseOrExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::LogicalAnd, std::move(expr),
                                          std::move(right), line, col);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseBitwiseOrExpression() {
    auto expr = parseBitwiseXorExpression();

    while (match(TokenType::Pipe)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseBitwiseXorExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::BitwiseOr, std::move(expr),
                                          std::move(right), line, col);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseBitwiseXorExpression() {
    auto expr = parseBitwiseAndExpression();

    while (match(TokenType::Caret)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseBitwiseAndExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::BitwiseXor, std::move(expr),
                                          std::move(right), line, col);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseBitwiseAndExpression() {
    auto expr = parseEqualityExpression();

    while (match(TokenType::Ampersand)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseEqualityExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::BitwiseAnd, std::move(expr),
                                          std::move(right), line, col);
    }

    return expr;
}

} // namespace cc1
