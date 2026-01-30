#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses an initializer, either a list or an assignment expression.
// FR: Parse un initialiseur, soit liste soit expression d affectation.
AST::Ptr<AST::Expression> Parser::parseInitializer() {
    if (check(TokenType::LeftBrace)) {
        return parseInitializerList();
    }
    return parseAssignmentExpression();
}

// EN: Parses a brace-enclosed initializer list.
// FR: Parse une liste d initialisation entre accolades.
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

} 

// TODO(cc1) EN: Support designated initializers if needed.
// FR: Supporter les initialisateurs designes si necessaire.
