#include <parser/Parser.hpp>

namespace cc1 {





// EN: Parses comma-separated expressions.
// FR: Parse les expressions separees par virgules.
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

// EN: Parses assignment expressions with right associativity.
// FR: Parse les affectations avec associativite a droite.
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

// EN: Parses ternary conditional expressions.
// FR: Parse les expressions conditionnelles ternaires.
AST::Ptr<AST::Expression> Parser::parseConditionalExpression() {
    auto expr = parseLogicalOrExpression();

    if (match(TokenType::Question)) {
        
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

// EN: Parses a constant expression (currently same as conditional).
// FR: Parse une expression constante (actuellement comme conditionnelle).
AST::Ptr<AST::Expression> Parser::parseConstantExpression() {
    return parseConditionalExpression();
}

} 

// TODO(cc1) EN: Enforce lvalue requirement for assignment left-hand side.
// FR: Appliquer la contrainte lvalue pour la gauche d affectation.
