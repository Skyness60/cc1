#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// EN: Visits the translation unit and analyzes all top-level declarations.
// FR: Visite l unite de traduction et analyse toutes les declarations globales.
void SemanticAnalyzer::visit(AST::TranslationUnit& node) {
    inGlobalScope_ = true;
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
}

} 

// TODO(cc1) EN: Track per-translation-unit diagnostics summary.
// FR: Suivre un resume de diagnostics par unite de traduction.
