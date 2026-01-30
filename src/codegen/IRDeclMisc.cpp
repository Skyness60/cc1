#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: No-op for parameter declarations in codegen (handled in functions).
// FR: No-op pour les parametres en codegen (gere dans fonctions).
void IRGenerator::visit(AST::ParamDecl& ) {
    
}

// EN: Emits IR type definitions for struct declarations.
// FR: Genere les definitions de type IR pour les structs.
void IRGenerator::visit(AST::StructDecl& node) {
    if (node.members.empty()) {
        
        return;
    }

    
    if (structLayouts_.find(node.name) != structLayouts_.end()) {
        return;
    }

    
    StructLayout layout = computeStructLayout(&node);
    structLayouts_[node.name] = layout;

    
    emitGlobal(layout.llvmType);
}

// EN: Registers enum constants for later codegen lookup.
// FR: Enregistre les constantes enum pour codegen.
void IRGenerator::visit(AST::EnumDecl& node) {
    
    for (auto& enumerator : node.enumerators) {
        enumValues_[enumerator.name] = enumerator.computedValue;
    }
}

} 
