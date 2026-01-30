#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses postfix expressions (calls, indexing, member access, post-inc/dec).
// FR: Parse les expressions postfixees (appel, index, membre, post-inc/dec).
AST::Ptr<AST::Expression> Parser::parsePostfixExpression() {
    auto expr = parsePrimaryExpression();

    while (true) {
        if (match(TokenType::LeftBracket)) {
            
            int line = previous().line;
            int col = previous().column;
            auto index = parseExpression();
            consume(TokenType::RightBracket, "expected ']' after index");
            expr = AST::make<AST::IndexExpr>(std::move(expr), std::move(index), line, col);
        } else if (match(TokenType::LeftParen)) {
            
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
            
            int line = previous().line;
            int col = previous().column;
            std::string member = consume(TokenType::Identifier, "expected member name").value;
            expr = AST::make<AST::MemberExpr>(std::move(expr), member, false, line, col);
        } else if (match(TokenType::Arrow)) {
            
            int line = previous().line;
            int col = previous().column;
            std::string member = consume(TokenType::Identifier, "expected member name").value;
            expr = AST::make<AST::MemberExpr>(std::move(expr), member, true, line, col);
        } else if (matchAny({TokenType::PlusPlus, TokenType::MinusMinus})) {
            
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

} 

// TODO(cc1) EN: Add support for compound literals in postfix parsing.
// FR: Ajouter le support des litteraux composes en postfixe.
