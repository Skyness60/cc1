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
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::Comma,
                                           std::move(expr), std::move(right),
                                           line, col);
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

AST::Ptr<AST::Expression> Parser::parseLogicalOrExpression() {
    auto expr = parseLogicalAndExpression();
    
    while (match(TokenType::PipePipe)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseLogicalAndExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::LogicalOr,
                                           std::move(expr), std::move(right),
                                           line, col);
    }
    
    return expr;
}

AST::Ptr<AST::Expression> Parser::parseLogicalAndExpression() {
    auto expr = parseBitwiseOrExpression();
    
    while (match(TokenType::AmpersandAmpersand)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseBitwiseOrExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::LogicalAnd,
                                           std::move(expr), std::move(right),
                                           line, col);
    }
    
    return expr;
}

AST::Ptr<AST::Expression> Parser::parseBitwiseOrExpression() {
    auto expr = parseBitwiseXorExpression();
    
    while (match(TokenType::Pipe)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseBitwiseXorExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::BitwiseOr,
                                           std::move(expr), std::move(right),
                                           line, col);
    }
    
    return expr;
}

AST::Ptr<AST::Expression> Parser::parseBitwiseXorExpression() {
    auto expr = parseBitwiseAndExpression();
    
    while (match(TokenType::Caret)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseBitwiseAndExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::BitwiseXor,
                                           std::move(expr), std::move(right),
                                           line, col);
    }
    
    return expr;
}

AST::Ptr<AST::Expression> Parser::parseBitwiseAndExpression() {
    auto expr = parseEqualityExpression();
    
    while (match(TokenType::Ampersand)) {
        int line = previous().line;
        int col = previous().column;
        auto right = parseEqualityExpression();
        expr = AST::make<AST::BinaryExpr>(AST::BinaryOp::BitwiseAnd,
                                           std::move(expr), std::move(right),
                                           line, col);
    }
    
    return expr;
}

AST::Ptr<AST::Expression> Parser::parseEqualityExpression() {
    auto expr = parseRelationalExpression();
    
    while (matchAny({TokenType::EqualEqual, TokenType::NotEqual})) {
        Token op = previous();
        auto right = parseRelationalExpression();
        AST::BinaryOp binOp = (op.type == TokenType::EqualEqual) 
            ? AST::BinaryOp::Equal : AST::BinaryOp::NotEqual;
        expr = AST::make<AST::BinaryExpr>(binOp, std::move(expr), std::move(right),
                                           op.line, op.column);
    }
    
    return expr;
}

AST::Ptr<AST::Expression> Parser::parseRelationalExpression() {
    auto expr = parseShiftExpression();
    
    while (matchAny({TokenType::Less, TokenType::LessEqual,
                     TokenType::Greater, TokenType::GreaterEqual})) {
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
        AST::BinaryOp binOp = (op.type == TokenType::LeftShift)
            ? AST::BinaryOp::LeftShift : AST::BinaryOp::RightShift;
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
        AST::BinaryOp binOp = (op.type == TokenType::Plus)
            ? AST::BinaryOp::Add : AST::BinaryOp::Sub;
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
            
            return AST::make<AST::CastExpr>(std::move(type), std::move(operand),
                                             line, col);
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
        AST::UnaryOp unaryOp = (op.type == TokenType::PlusPlus)
            ? AST::UnaryOp::PreIncrement : AST::UnaryOp::PreDecrement;
        return AST::make<AST::UnaryExpr>(unaryOp, std::move(operand), true,
                                          op.line, op.column);
    }
    
    if (matchAny({TokenType::Ampersand, TokenType::Star, TokenType::Plus,
                  TokenType::Minus, TokenType::Tilde, TokenType::Exclamation})) {
        Token op = previous();
        auto operand = parseCastExpression();
        AST::UnaryOp unaryOp = tokenToUnaryOp(op.type);
        return AST::make<AST::UnaryExpr>(unaryOp, std::move(operand), true,
                                          op.line, op.column);
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

AST::Ptr<AST::Expression> Parser::parsePostfixExpression() {
    auto expr = parsePrimaryExpression();
    
    while (true) {
        if (match(TokenType::LeftBracket)) {
            // Array subscript
            int line = previous().line;
            int col = previous().column;
            auto index = parseExpression();
            consume(TokenType::RightBracket, "expected ']' after index");
            expr = AST::make<AST::IndexExpr>(std::move(expr), std::move(index),
                                              line, col);
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
            std::string member = consume(TokenType::Identifier,
                "expected member name").value;
            expr = AST::make<AST::MemberExpr>(std::move(expr), member, false,
                                               line, col);
        } else if (match(TokenType::Arrow)) {
            // Pointer member access
            int line = previous().line;
            int col = previous().column;
            std::string member = consume(TokenType::Identifier,
                "expected member name").value;
            expr = AST::make<AST::MemberExpr>(std::move(expr), member, true,
                                               line, col);
        } else if (matchAny({TokenType::PlusPlus, TokenType::MinusMinus})) {
            // Postfix increment/decrement
            Token op = previous();
            AST::UnaryOp unaryOp = (op.type == TokenType::PlusPlus)
                ? AST::UnaryOp::PostIncrement : AST::UnaryOp::PostDecrement;
            expr = AST::make<AST::UnaryExpr>(unaryOp, std::move(expr), false,
                                              op.line, op.column);
        } else {
            break;
        }
    }
    
    return expr;
}

AST::Ptr<AST::Expression> Parser::parsePrimaryExpression() {
    // Integer literal
    if (check(TokenType::IntegerLiteral)) {
        Token tok = current();
        advance();
        long long value = std::stoll(tok.value);
        return AST::make<AST::IntegerLiteral>(value, tok.value, tok.line, tok.column);
    }
    
    // Float literal
    if (check(TokenType::FloatLiteral)) {
        Token tok = current();
        advance();
        double value = std::stod(tok.value);
        return AST::make<AST::FloatLiteral>(value, tok.value, tok.line, tok.column);
    }
    
    // String literal
    if (check(TokenType::StringLiteral)) {
        Token tok = current();
        advance();
        // Remove quotes and handle escape sequences (simplified)
        std::string value = tok.value.substr(1, tok.value.length() - 2);
        return AST::make<AST::StringLiteral>(value, tok.value, tok.line, tok.column);
    }
    
    // Char literal
    if (check(TokenType::CharLiteral)) {
        Token tok = current();
        advance();
        int value = tok.value.length() > 2 ? tok.value[1] : 0;
        return AST::make<AST::CharLiteral>(value, tok.value, tok.line, tok.column);
    }
    
    // Identifier
    if (check(TokenType::Identifier)) {
        Token tok = current();
        advance();
        return AST::make<AST::Identifier>(tok.value, tok.line, tok.column);
    }
    
    // Parenthesized expression
    if (match(TokenType::LeftParen)) {
        auto expr = parseExpression();
        consume(TokenType::RightParen, "expected ')' after expression");
        return expr;
    }
    
    error("expected statement");
}

AST::Ptr<AST::Expression> Parser::parseConstantExpression() {
    return parseConditionalExpression();
}

// ============================================================================
// Operator Conversion Utilities
// ============================================================================

AST::BinaryOp Parser::tokenToBinaryOp(TokenType type) const {
    switch (type) {
        case TokenType::Plus:           return AST::BinaryOp::Add;
        case TokenType::Minus:          return AST::BinaryOp::Sub;
        case TokenType::Star:           return AST::BinaryOp::Mul;
        case TokenType::Slash:          return AST::BinaryOp::Div;
        case TokenType::Percent:        return AST::BinaryOp::Mod;
        case TokenType::EqualEqual:     return AST::BinaryOp::Equal;
        case TokenType::NotEqual:       return AST::BinaryOp::NotEqual;
        case TokenType::Less:           return AST::BinaryOp::Less;
        case TokenType::LessEqual:      return AST::BinaryOp::LessEqual;
        case TokenType::Greater:        return AST::BinaryOp::Greater;
        case TokenType::GreaterEqual:   return AST::BinaryOp::GreaterEqual;
        case TokenType::AmpersandAmpersand: return AST::BinaryOp::LogicalAnd;
        case TokenType::PipePipe:       return AST::BinaryOp::LogicalOr;
        case TokenType::Ampersand:      return AST::BinaryOp::BitwiseAnd;
        case TokenType::Pipe:           return AST::BinaryOp::BitwiseOr;
        case TokenType::Caret:          return AST::BinaryOp::BitwiseXor;
        case TokenType::LeftShift:      return AST::BinaryOp::LeftShift;
        case TokenType::RightShift:     return AST::BinaryOp::RightShift;
        case TokenType::Equal:          return AST::BinaryOp::Assign;
        case TokenType::PlusEqual:      return AST::BinaryOp::AddAssign;
        case TokenType::MinusEqual:     return AST::BinaryOp::SubAssign;
        case TokenType::StarEqual:      return AST::BinaryOp::MulAssign;
        case TokenType::SlashEqual:     return AST::BinaryOp::DivAssign;
        case TokenType::PercentEqual:   return AST::BinaryOp::ModAssign;
        case TokenType::AmpersandEqual: return AST::BinaryOp::AndAssign;
        case TokenType::PipeEqual:      return AST::BinaryOp::OrAssign;
        case TokenType::CaretEqual:     return AST::BinaryOp::XorAssign;
        case TokenType::LeftShiftEqual: return AST::BinaryOp::LeftShiftAssign;
        case TokenType::RightShiftEqual:return AST::BinaryOp::RightShiftAssign;
        case TokenType::Comma:          return AST::BinaryOp::Comma;
        default:                        return AST::BinaryOp::Add; // Shouldn't happen
    }
}

AST::UnaryOp Parser::tokenToUnaryOp(TokenType type) const {
    switch (type) {
        case TokenType::Plus:       return AST::UnaryOp::Plus;
        case TokenType::Minus:      return AST::UnaryOp::Negate;
        case TokenType::Exclamation:return AST::UnaryOp::LogicalNot;
        case TokenType::Tilde:      return AST::UnaryOp::BitwiseNot;
        case TokenType::Ampersand:  return AST::UnaryOp::AddressOf;
        case TokenType::Star:       return AST::UnaryOp::Dereference;
        case TokenType::PlusPlus:   return AST::UnaryOp::PreIncrement;
        case TokenType::MinusMinus: return AST::UnaryOp::PreDecrement;
        default:                    return AST::UnaryOp::Plus; // Shouldn't happen
    }
}

AST::UnaryOp Parser::tokenToPostfixOp(TokenType type) const {
    switch (type) {
        case TokenType::PlusPlus:   return AST::UnaryOp::PostIncrement;
        case TokenType::MinusMinus: return AST::UnaryOp::PostDecrement;
        default:                    return AST::UnaryOp::PostIncrement;
    }
}

} // namespace cc1
