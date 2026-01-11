#include <parser/Parser.hpp>

namespace cc1 {

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
    advance();
    advance();

    auto stmt = parseStatement();

    return AST::make<AST::LabelStmt>(label, std::move(stmt), line, col);
}

AST::Ptr<AST::Statement> Parser::parseExpressionStatement() {
    int line = current().line;
    int col = current().column;

    if (match(TokenType::Semicolon)) {
        return AST::make<AST::ExpressionStmt>(nullptr, line, col);
    }

    auto expr = parseExpression();
    consume(TokenType::Semicolon, "expected ';' after expression");

    return AST::make<AST::ExpressionStmt>(std::move(expr), line, col);
}

} // namespace cc1
