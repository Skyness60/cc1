#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses a statement by dispatching on the current token.
// FR: Parse une instruction en dispatchant selon le token courant.
AST::Ptr<AST::Statement> Parser::parseStatement() {
    if (check(TokenType::Identifier) && peek().type == TokenType::Colon) {
        return parseLabeledStatement();
    }

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

    if (check(TokenType::LeftBrace)) {
        return parseCompoundStatement();
    }

    if (isDeclarationStart()) {
        auto decl = parseDeclaration();
        (void)decl;
        return parseStatement();
    }

    return parseExpressionStatement();
}

// EN: Parses a compound statement block with declarations and statements.
// FR: Parse un bloc compose avec declarations et instructions.
AST::Ptr<AST::CompoundStmt> Parser::parseCompoundStatement() {
    int line = current().line;
    int col = current().column;
    consume(TokenType::LeftBrace, "expected '{'");

    auto compound = AST::make<AST::CompoundStmt>(line, col);

    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (isDeclarationStart()) {
            auto decls = parseDeclarations();
            for (auto& decl : decls) {
                if (decl) {
                    compound->declarations.push_back(std::move(decl));
                }
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

} 

// TODO(cc1) EN: Improve mixed declaration/statement ordering errors.
// FR: Ameliorer les erreurs d ordre declaration/instruction.
