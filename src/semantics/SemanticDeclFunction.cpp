#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// EN: Analyzes a function declaration/definition and its body scope.
// FR: Analyse une declaration/definition de fonction et son scope.
void SemanticAnalyzer::visit(AST::FunctionDecl& node) {
    Symbol sym;
    sym.name = node.name;
    sym.isFunction = true;
    sym.line = node.line;
    sym.column = node.column;

    std::vector<AST::Ptr<AST::Type>> paramTypes;
    for (const auto& param : node.parameters) {
        if (param && param->type) {
            paramTypes.push_back(param->type->clone());
        }
    }
    auto funcType = AST::make<AST::FunctionType>(
        node.returnType ? node.returnType->clone() : nullptr, std::move(paramTypes), node.isVariadic,
        node.line, node.column);

    currentScope_->define(node.name, sym, std::move(funcType));

    if (node.body) {
        currentFunction_ = &node;
        inGlobalScope_ = false;

        enterScope(true);

        for (auto& param : node.parameters) {
            if (param) param->accept(*this);
        }

        node.body->accept(*this);

        exitScope();

        currentFunction_ = nullptr;
        inGlobalScope_ = true;
    }
}

} 

// TODO(cc1) EN: Diagnose conflicting function prototypes across scopes.
// FR: Diagnostiquer les prototypes de fonctions en conflit entre scopes.
