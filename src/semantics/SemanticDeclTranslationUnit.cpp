#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

void SemanticAnalyzer::visit(AST::TranslationUnit& node) {
    inGlobalScope_ = true;
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
}

} // namespace cc1
