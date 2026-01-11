#include <parser/Parser.hpp>

namespace cc1 {

AST::Ptr<AST::Expression> Parser::parsePostfixExpression() {
    auto expr = parsePrimaryExpression();

    while (true) {
        if (match(TokenType::LeftBracket)) {
            // Array subscript
            int line = previous().line;
            int col = previous().column;
            auto index = parseExpression();
            consume(TokenType::RightBracket, "expected ']' after index");
            expr = AST::make<AST::IndexExpr>(std::move(expr), std::move(index), line, col);
        } else if (match(TokenType::LeftParen)) {
            // Function call
            int line = previous().line;
            int col = previous().column;
            auto call = AST::make<AST::CallExpr>(std::move(expr), line, col);

            if (!check(TokenType::RightParen)) {
                do {
                    call->arguments.push_back(parseAssignmentExpression());
                } while (match(TokenType::Comma));
            }

            consume(TokenType::RightParen, "expected ')' after arguments");
            expr = std::move(call);
        } else if (match(TokenType::Dot)) {
            // Member access
            int line = previous().line;
            int col = previous().column;
            std::string member = consume(TokenType::Identifier, "expected member name").value;
            expr = AST::make<AST::MemberExpr>(std::move(expr), member, false, line, col);
        } else if (match(TokenType::Arrow)) {
            // Pointer member access
            int line = previous().line;
            int col = previous().column;
            std::string member = consume(TokenType::Identifier, "expected member name").value;
            expr = AST::make<AST::MemberExpr>(std::move(expr), member, true, line, col);
        } else if (matchAny({TokenType::PlusPlus, TokenType::MinusMinus})) {
            // Postfix increment/decrement
            Token op = previous();
            AST::UnaryOp unaryOp = (op.type == TokenType::PlusPlus) ? AST::UnaryOp::PostIncrement
                                                                    : AST::UnaryOp::PostDecrement;
            expr = AST::make<AST::UnaryExpr>(unaryOp, std::move(expr), false, op.line, op.column);
        } else {
            break;
        }
    }

    return expr;
}

} // namespace cc1
