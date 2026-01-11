#include <parser/Parser.hpp>

namespace cc1 {

AST::Ptr<AST::Declaration> Parser::parseDeclaration() {
    return parseExternalDeclaration();
}

std::vector<AST::Ptr<AST::Declaration>> Parser::parseDeclarations() {
    std::vector<AST::Ptr<AST::Declaration>> result;
    auto first = parseDeclaration();
    if (first) {
        result.push_back(std::move(first));
    }
    // Move any additional declarations collected during parsing
    while (!additionalDeclarations_.empty()) {
        result.push_back(std::move(additionalDeclarations_.back()));
        additionalDeclarations_.pop_back();
    }
    return result;
}

} // namespace cc1
