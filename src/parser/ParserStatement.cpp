#include <parser/Parser.hpp>

namespace cc1 {

// ============================================================================
// Statement Parsing
// ============================================================================

AST::Ptr<AST::Statement> Parser::parseStatement() {
    // Labeled statement
    if (check(TokenType::Identifier) && peek().type == TokenType::Colon) {
        return parseLabeledStatement();
    }
    
    // Control flow statements
    if (match(TokenType::If)) return parseIfStatement();
    if (match(TokenType::While)) return parseWhileStatement();
    if (match(TokenType::Do)) return parseDoWhileStatement();
    if (match(TokenType::For)) return parseForStatement();
    if (match(TokenType::Switch)) return parseSwitchStatement();
    if (match(TokenType::Case)) return parseCaseStatement();
    if (match(TokenType::Default)) return parseDefaultStatement();
    if (match(TokenType::Return)) return parseReturnStatement();
    if (match(TokenType::Goto)) return parseGotoStatement();
    if (match(TokenType::Break)) return parseBreakStatement();
    if (match(TokenType::Continue)) return parseContinueStatement();
    
    // Compound statement
    if (check(TokenType::LeftBrace)) {
        return parseCompoundStatement();
    }
    
    // Declaration (in C89, declarations must come at start of block,
    // but we'll be lenient for now)
    if (isDeclarationStart()) {
        // Convert declaration to declaration statement
        auto decl = parseDeclaration();
        // For now, just skip - in full implementation we'd have a DeclStmt
        return parseStatement();
    }
    
    // Expression statement
    return parseExpressionStatement();
}

AST::Ptr<AST::CompoundStmt> Parser::parseCompoundStatement() {
    int line = current().line;
    int col = current().column;
    consume(TokenType::LeftBrace, "expected '{'");
    
    auto compound = AST::make<AST::CompoundStmt>(line, col);
    
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        // In C89, declarations come first, then statements
        if (isDeclarationStart()) {
            auto decl = parseDeclaration();
            if (decl) {
                compound->declarations.push_back(std::move(decl));
            }
        } else {
            auto stmt = parseStatement();
            if (stmt) {
                compound->statements.push_back(std::move(stmt));
            }
        }
    }
    
    consume(TokenType::RightBrace, "expected '}'");
    return compound;
}

AST::Ptr<AST::Statement> Parser::parseIfStatement() {
    int line = previous().line;
    int col = previous().column;
    
    consume(TokenType::LeftParen, "expected '(' after 'if'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "expected ')' after condition");
    
    auto thenBranch = parseStatement();
    
    AST::Ptr<AST::Statement> elseBranch;
    if (match(TokenType::Else)) {
        elseBranch = parseStatement();
    }
    
    return AST::make<AST::IfStmt>(std::move(condition), std::move(thenBranch),
                                   std::move(elseBranch), line, col);
}

AST::Ptr<AST::Statement> Parser::parseWhileStatement() {
    int line = previous().line;
    int col = previous().column;
    
    consume(TokenType::LeftParen, "expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "expected ')' after condition");
    
    auto body = parseStatement();
    
    return AST::make<AST::WhileStmt>(std::move(condition), std::move(body),
                                      line, col);
}

AST::Ptr<AST::Statement> Parser::parseDoWhileStatement() {
    int line = previous().line;
    int col = previous().column;
    
    auto body = parseStatement();
    
    consume(TokenType::While, "expected 'while' after do body");
    consume(TokenType::LeftParen, "expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "expected ')' after condition");
    consume(TokenType::Semicolon, "expected ';' after do-while");
    
    return AST::make<AST::DoWhileStmt>(std::move(body), std::move(condition),
                                        line, col);
}

AST::Ptr<AST::Statement> Parser::parseForStatement() {
    int line = previous().line;
    int col = previous().column;
    
    consume(TokenType::LeftParen, "expected '(' after 'for'");
    
    AST::Ptr<AST::Expression> init;
    if (!check(TokenType::Semicolon)) {
        init = parseExpression();
    }
    consume(TokenType::Semicolon, "expected ';' after for init");
    
    AST::Ptr<AST::Expression> condition;
    if (!check(TokenType::Semicolon)) {
        condition = parseExpression();
    }
    consume(TokenType::Semicolon, "expected ';' after for condition");
    
    AST::Ptr<AST::Expression> increment;
    if (!check(TokenType::RightParen)) {
        increment = parseExpression();
    }
    consume(TokenType::RightParen, "expected ')' after for clauses");
    
    auto body = parseStatement();
    
    return AST::make<AST::ForStmt>(std::move(init), std::move(condition),
                                    std::move(increment), std::move(body),
                                    line, col);
}

AST::Ptr<AST::Statement> Parser::parseSwitchStatement() {
    int line = previous().line;
    int col = previous().column;
    
    consume(TokenType::LeftParen, "expected '(' after 'switch'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "expected ')' after switch condition");
    
    // Parse body with incremented switch depth
    switchDepth_++;
    auto body = parseStatement();
    switchDepth_--;
    
    return AST::make<AST::SwitchStmt>(std::move(condition), std::move(body),
                                       line, col);
}

AST::Ptr<AST::Statement> Parser::parseCaseStatement() {
    int line = previous().line;
    int col = previous().column;
    
    // Check if we're inside a switch statement
    if (switchDepth_ == 0) {
        error("'case' statement not in switch statement");
    }
    
    auto value = parseConstantExpression();
    consume(TokenType::Colon, "expected ':' after case value");
    
    auto stmt = parseStatement();
    
    return AST::make<AST::CaseStmt>(std::move(value), std::move(stmt), line, col);
}

AST::Ptr<AST::Statement> Parser::parseDefaultStatement() {
    int line = previous().line;
    int col = previous().column;
    
    // Check if we're inside a switch statement
    if (switchDepth_ == 0) {
        errorAtPosition(line, col, "'default' statement not in switch statement");
    }
    
    consume(TokenType::Colon, "expected ':' after 'default'");
    
    auto stmt = parseStatement();
    
    return AST::make<AST::DefaultStmt>(std::move(stmt), line, col);
}

AST::Ptr<AST::Statement> Parser::parseReturnStatement() {
    int line = previous().line;
    int col = previous().column;
    
    AST::Ptr<AST::Expression> value;
    if (!check(TokenType::Semicolon)) {
        value = parseExpression();
    }
    
    consume(TokenType::Semicolon, "expected ';' after return");
    
    return AST::make<AST::ReturnStmt>(std::move(value), line, col);
}

AST::Ptr<AST::Statement> Parser::parseGotoStatement() {
    int line = previous().line;
    int col = previous().column;
    
    std::string label = consume(TokenType::Identifier, "expected label name").value;
    consume(TokenType::Semicolon, "expected ';' after goto");
    
    return AST::make<AST::GotoStmt>(label, line, col);
}

AST::Ptr<AST::Statement> Parser::parseBreakStatement() {
    int line = previous().line;
    int col = previous().column;
    
    consume(TokenType::Semicolon, "expected ';' after break");
    
    return AST::make<AST::BreakStmt>(line, col);
}

AST::Ptr<AST::Statement> Parser::parseContinueStatement() {
    int line = previous().line;
    int col = previous().column;
    
    consume(TokenType::Semicolon, "expected ';' after continue");
    
    return AST::make<AST::ContinueStmt>(line, col);
}

AST::Ptr<AST::Statement> Parser::parseLabeledStatement() {
    int line = current().line;
    int col = current().column;
    
    std::string label = current().value;
    advance(); // identifier
    advance(); // colon
    
    auto stmt = parseStatement();
    
    return AST::make<AST::LabelStmt>(label, std::move(stmt), line, col);
}

AST::Ptr<AST::Statement> Parser::parseExpressionStatement() {
    int line = current().line;
    int col = current().column;
    
    if (match(TokenType::Semicolon)) {
        // Empty statement
        return AST::make<AST::ExpressionStmt>(nullptr, line, col);
    }
    
    auto expr = parseExpression();
    consume(TokenType::Semicolon, "expected ';' after expression");
    
    return AST::make<AST::ExpressionStmt>(std::move(expr), line, col);
}

} // namespace cc1
