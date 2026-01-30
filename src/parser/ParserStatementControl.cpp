#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses an if statement with optional else branch.
// FR: Parse une instruction if avec else optionnel.
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

// EN: Parses a while loop statement.
// FR: Parse une boucle while.
AST::Ptr<AST::Statement> Parser::parseWhileStatement() {
    int line = previous().line;
    int col = previous().column;

    consume(TokenType::LeftParen, "expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::RightParen, "expected ')' after condition");

    auto body = parseStatement();

    return AST::make<AST::WhileStmt>(std::move(condition), std::move(body), line, col);
}

// EN: Parses a do-while loop statement.
// FR: Parse une boucle do-while.
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

// EN: Parses a for loop statement with init/cond/increment.
// FR: Parse une boucle for avec init/cond/increment.
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

// EN: Parses a switch statement and tracks switch depth.
// FR: Parse un switch et suit la profondeur de switch.
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

// EN: Parses a case label inside a switch statement.
// FR: Parse un label case dans un switch.
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

// EN: Parses a default label inside a switch statement.
// FR: Parse un label default dans un switch.
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

} 

// TODO(cc1) EN: Support C23 'case ...' ranges if desired.
// FR: Supporter les ranges de case C23 si souhaite.
