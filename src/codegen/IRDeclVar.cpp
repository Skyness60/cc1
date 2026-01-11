#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Variable Declaration
// ============================================================================

void IRGenerator::visit(AST::VarDecl& node) {
    // Skip typedef declarations - they don't generate code
    if (node.storageClass == AST::StorageClass::Typedef) {
        return;
    }

    if (node.name.empty()) {
        // Anonymous declaration (e.g., standalone enum definition)
        if (auto* enumType = dynamic_cast<AST::EnumType*>(stripQualifiers(node.type.get()))) {
            // Use pre-computed enum values from semantic analysis
            for (const auto& enumerator : enumType->enumerators) {
                enumValues_[enumerator.name] = enumerator.computedValue;
            }
        }
        return;
    }

    // Check if this is a function prototype (VarDecl with FunctionType)
    if (auto* funcType = dynamic_cast<AST::FunctionType*>(stripQualifiers(node.type.get()))) {
        emitVarDeclFunctionPrototype(node, *funcType);
        return;
    }

    std::string llvmType = typeToLLVM(node.type.get());

    if (inGlobalScope_) {
        emitGlobalVarDecl(node, llvmType);
        return;
    }

    emitLocalVarDecl(node, llvmType);
}

} // namespace cc1
