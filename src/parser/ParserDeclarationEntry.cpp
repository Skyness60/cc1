#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses a declaration in the current scope using external logic.
// FR: Parse une declaration dans le scope courant via la logique externe.
AST::Ptr<AST::Declaration> Parser::parseDeclaration() {
    return parseExternalDeclaration();
}

// EN: Parses one declaration and drains any queued follow-up declarations.
// FR: Parse une declaration et vide les declarations additionnelles en attente.
std::vector<AST::Ptr<AST::Declaration>> Parser::parseDeclarations() {
    std::vector<AST::Ptr<AST::Declaration>> result;
    auto first = parseDeclaration();
    if (first) {
        result.push_back(std::move(first));
    }
    
    while (!additionalDeclarations_.empty()) {
        result.push_back(std::move(additionalDeclarations_.back()));
        additionalDeclarations_.pop_back();
    }
    return result;
}

} 

// TODO(cc1) EN: Clarify ownership/lifetime of additionalDeclarations_.
// FR: Clarifier la gestion de vie de additionalDeclarations_.
