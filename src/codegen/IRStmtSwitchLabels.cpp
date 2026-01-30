#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for a case label within the current switch context.
// FR: Genere l IR pour un label case dans le switch courant.
void IRGenerator::visit(AST::CaseStmt& node) {
    if (switchStack_.empty()) {
        
        hadError_ = true;
        return;
    }

    SwitchContext& ctx = switchStack_.top();
    auto it = ctx.labelForNode.find(&node);
    if (it == ctx.labelForNode.end()) {
        hadError_ = true;
        return;
    }
    const std::string& caseLabel = it->second;

    
    if (!blockTerminated_) {
        emit("br label %" + caseLabel);
    }
    emitLabel(caseLabel);

    if (node.body) {
        node.body->accept(*this);
    }
}

// EN: Emits IR for the default label within the current switch context.
// FR: Genere l IR pour le label default dans le switch courant.
void IRGenerator::visit(AST::DefaultStmt& node) {
    if (switchStack_.empty()) {
        hadError_ = true;
        return;
    }

    SwitchContext& ctx = switchStack_.top();
    auto it = ctx.labelForNode.find(&node);
    if (it == ctx.labelForNode.end()) {
        hadError_ = true;
        return;
    }
    const std::string& defaultLabel = it->second;

    if (!blockTerminated_) {
        emit("br label %" + defaultLabel);
    }
    emitLabel(defaultLabel);

    if (node.body) {
        node.body->accept(*this);
    }
}

} 
