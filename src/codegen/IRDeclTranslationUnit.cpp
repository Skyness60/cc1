#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Translation Unit
// ============================================================================

void IRGenerator::visit(AST::TranslationUnit& snode) {
    inGlobalScope_ = true;

    // First pass: collect typedef definitions and assign names to anonymous structs
    for (auto& decl : snode.declarations) {
        // Check for TypedefDecl
        if (auto* typedefDecl = dynamic_cast<AST::TypedefDecl*>(decl.get())) {
            if (typedefDecl->underlyingType) {
                typedefMap_[typedefDecl->name] = typedefDecl->underlyingType.get();

                // If the underlying type is an anonymous struct, assign the typedef name as struct name
                if (auto* structType = dynamic_cast<AST::StructType*>(typedefDecl->underlyingType.get())) {
                    if (structType->name.empty()) {
                        structType->name = typedefDecl->name;
                    }
                }
            }
        }
        // Also check for VarDecl with Typedef storage class (used by parser)
        else if (auto* varDecl = dynamic_cast<AST::VarDecl*>(decl.get())) {
            if (varDecl->storageClass == AST::StorageClass::Typedef && varDecl->type) {
                typedefMap_[varDecl->name] = varDecl->type.get();

                // If the underlying type is an anonymous struct, assign the typedef name as struct name
                if (auto* structType = dynamic_cast<AST::StructType*>(varDecl->type.get())) {
                    if (structType->name.empty()) {
                        structType->name = varDecl->name;
                    }
                }
            }
        }
    }

    // Second pass: collect struct definitions
    for (auto& decl : snode.declarations) {
        if (auto* structDecl = dynamic_cast<AST::StructDecl*>(decl.get())) {
            if (!structDecl->members.empty()) {
                StructLayout layout = computeStructLayout(structDecl);
                structLayouts_[structDecl->name] = layout;

                // Ensure the struct type definition is emitted exactly once via getIR().
                if (!namedStructDefs_.count(structDecl->name)) {
                    namedStructDefs_[structDecl->name] = {layout.llvmType, structDecl->declaredType.get()};
                }
            }
        }
    }

    // Third pass: process declarations
    for (auto& decl : snode.declarations) {
        if (decl) decl->accept(*this);
    }
}

} // namespace cc1
