#include <parser/Parser.hpp>

namespace cc1 {

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

    return AST::make<AST::IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch),
                                  line, col);
}

AST::Ptr<AST::Statement> Parser::parseWhileStatement() {
    int line = previous().line;
    int col = previous().column;

    consume(TokenType::LeftParen, "expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "expected ')' after condition");

    auto body = parseStatement();

    return AST::make<AST::WhileStmt>(std::move(condition), std::move(body), line, col);
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

    return AST::make<AST::DoWhileStmt>(std::move(body), std::move(condition), line, col);
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

    return AST::make<AST::ForStmt>(std::move(init), std::move(condition), std::move(increment),
                                   std::move(body), line, col);
}

AST::Ptr<AST::Statement> Parser::parseSwitchStatement() {
    int line = previous().line;
    int col = previous().column;

    consume(TokenType::LeftParen, "expected '(' after 'switch'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "expected ')' after switch condition");

    switchDepth_++;
    auto body = parseStatement();
    switchDepth_--;

    return AST::make<AST::SwitchStmt>(std::move(condition), std::move(body), line, col);
}

AST::Ptr<AST::Statement> Parser::parseCaseStatement() {
    int line = previous().line;
    int col = previous().column;

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

    if (switchDepth_ == 0) {
        errorAtPosition(line, col, "'default' statement not in switch statement");
    }

    consume(TokenType::Colon, "expected ':' after 'default'");

    auto stmt = parseStatement();

    return AST::make<AST::DefaultStmt>(std::move(stmt), line, col);
}

} // namespace cc1
