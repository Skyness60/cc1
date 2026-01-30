#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for the translation unit, pre-collecting typedefs/structs.
// FR: Genere l IR pour l unite, en precollectant typedefs/structs.
void IRGenerator::visit(AST::TranslationUnit& snode) {
    inGlobalScope_ = true;

    
    for (auto& decl : snode.declarations) {
        
        if (auto* typedefDecl = dynamic_cast<AST::TypedefDecl*>(decl.get())) {
            if (typedefDecl->underlyingType) {
                typedefMap_[typedefDecl->name] = typedefDecl->underlyingType.get();

                
                if (auto* structType = dynamic_cast<AST::StructType*>(typedefDecl->underlyingType.get())) {
                    if (structType->name.empty()) {
                        structType->name = typedefDecl->name;
                    }
                }
            }
        }
        
        else if (auto* varDecl = dynamic_cast<AST::VarDecl*>(decl.get())) {
            if (varDecl->storageClass == AST::StorageClass::Typedef && varDecl->type) {
                typedefMap_[varDecl->name] = varDecl->type.get();

                
                if (auto* structType = dynamic_cast<AST::StructType*>(varDecl->type.get())) {
                    if (structType->name.empty()) {
                        structType->name = varDecl->name;
                    }
                }
            }
        }
    }

    
    for (auto& decl : snode.declarations) {
        if (auto* structDecl = dynamic_cast<AST::StructDecl*>(decl.get())) {
            if (!structDecl->members.empty()) {
                StructLayout layout = computeStructLayout(structDecl);
                structLayouts_[structDecl->name] = layout;

                
                if (!namedStructDefs_.count(structDecl->name)) {
                    namedStructDefs_[structDecl->name] = {layout.llvmType, structDecl->declaredType.get()};
                }
            }
        }
    }

    
    for (auto& decl : snode.declarations) {
        if (decl) decl->accept(*this);
    }
}

} 
