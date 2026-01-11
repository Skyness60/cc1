#include <parser/Parser.hpp>

namespace cc1 {

AST::Ptr<AST::Expression> Parser::parseEqualityExpression() {
    auto expr = parseRelationalExpression();

    while (matchAny({TokenType::EqualEqual, TokenType::NotEqual})) {
        Token op = previous();
        auto right = parseRelationalExpression();
        AST::BinaryOp binOp = (op.type == TokenType::EqualEqual) ? AST::BinaryOp::Equal
                                                                 : AST::BinaryOp::NotEqual;
        expr = AST::make<AST::BinaryExpr>(binOp, std::move(expr), std::move(right),
                                          op.line, op.column);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseRelationalExpression() {
    auto expr = parseShiftExpression();

    while (matchAny({TokenType::Less, TokenType::LessEqual, TokenType::Greater,
                     TokenType::GreaterEqual})) {
        Token op = previous();
        auto right = parseShiftExpression();
        AST::BinaryOp binOp = tokenToBinaryOp(op.type);
        expr = AST::make<AST::BinaryExpr>(binOp, std::move(expr), std::move(right),
                                          op.line, op.column);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseShiftExpression() {
    auto expr = parseAdditiveExpression();

    while (matchAny({TokenType::LeftShift, TokenType::RightShift})) {
        Token op = previous();
        auto right = parseAdditiveExpression();
        AST::BinaryOp binOp = (op.type == TokenType::LeftShift) ? AST::BinaryOp::LeftShift
                                                                : AST::BinaryOp::RightShift;
        expr = AST::make<AST::BinaryExpr>(binOp, std::move(expr), std::move(right),
                                          op.line, op.column);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseAdditiveExpression() {
    auto expr = parseMultiplicativeExpression();

    while (matchAny({TokenType::Plus, TokenType::Minus})) {
        Token op = previous();
        auto right = parseMultiplicativeExpression();
        AST::BinaryOp binOp = (op.type == TokenType::Plus) ? AST::BinaryOp::Add
                                                           : AST::BinaryOp::Sub;
        expr = AST::make<AST::BinaryExpr>(binOp, std::move(expr), std::move(right),
                                          op.line, op.column);
    }

    return expr;
}

AST::Ptr<AST::Expression> Parser::parseMultiplicativeExpression() {
    auto expr = parseCastExpression();

    while (matchAny({TokenType::Star, TokenType::Slash, TokenType::Percent})) {
        Token op = previous();
        auto right = parseCastExpression();
        AST::BinaryOp binOp = tokenToBinaryOp(op.type);
        expr = AST::make<AST::BinaryExpr>(binOp, std::move(expr), std::move(right),
                                          op.line, op.column);
    }

    return expr;
}

} // namespace cc1
