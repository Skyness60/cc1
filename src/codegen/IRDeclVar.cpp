#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for variable declarations, dispatching by scope/kind.
// FR: Genere l IR pour declarations de variables selon scope/type.
void IRGenerator::visit(AST::VarDecl& node) {
    
    if (node.storageClass == AST::StorageClass::Typedef) {
        if (node.type) {
            typedefMap_[node.name] = node.type.get();

            if (auto* structType = dynamic_cast<AST::StructType*>(stripQualifiers(node.type.get()))) {
                if (structType->name.empty()) {
                    structType->name = node.name;
                }
            }
        }
        return;
    }

    if (node.name.empty()) {
        
        if (auto* enumType = dynamic_cast<AST::EnumType*>(stripQualifiers(node.type.get()))) {
            
            for (const auto& enumerator : enumType->enumerators) {
                enumValues_[enumerator.name] = enumerator.computedValue;
            }
        }
        return;
    }

    
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

} 
