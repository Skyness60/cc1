#include <parser/Parser.hpp>

namespace cc1 {

AST::Ptr<AST::Expression> Parser::parseInitializer() {
    if (check(TokenType::LeftBrace)) {
        return parseInitializerList();
    }
    return parseAssignmentExpression();
}

AST::Ptr<AST::InitializerList> Parser::parseInitializerList() {
    int line = current().line;
    int col = current().column;
    consume(TokenType::LeftBrace, "expected '{'");

    auto initList = AST::make<AST::InitializerList>(line, col);

    if (!check(TokenType::RightBrace)) {
        do {
            initList->initializers.push_back(parseInitializer());
        } while (match(TokenType::Comma) && !check(TokenType::RightBrace));
    }

    consume(TokenType::RightBrace, "expected '}'");
    return initList;
}

} // namespace cc1
