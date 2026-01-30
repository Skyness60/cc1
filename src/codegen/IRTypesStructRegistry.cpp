#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Returns the cached layout for a named struct, if any.
// FR: Renvoie le layout cache d une struct nommee, si present.
const IRGenerator::StructLayout* IRGenerator::getStructLayout(const std::string& name) {
    auto it = structLayouts_.find(name);
    if (it != structLayouts_.end()) {
        return &it->second;
    }
    return nullptr;
}

// EN: Registers a named struct and its nested structs for codegen.
// FR: Enregistre une struct nommee et ses structs imbriquees.
void IRGenerator::collectNamedStruct(AST::StructType* structType) {
    if (!structType) {
        return;  
    }

    if (structType->name.empty()) {
        return;  
    }

    
    if (namedStructDefs_.count(structType->name)) {
        return;
    }

    
    if (!structType->members.empty()) {
        for (const auto& member : structType->members) {
            if (member.type) {
                AST::Type* nestedType = stripQualifiers(member.type.get());
                if (auto* nestedStruct = dynamic_cast<AST::StructType*>(nestedType)) {
                    
                    if (nestedStruct->name.empty()) {
                        nestedStruct->name = nextAnonStructName();
                    }
                    
                    collectNamedStruct(nestedStruct);
                }
            }
        }
    }

    
    std::string definition = generateStructTypeDefinition(structType);
    namedStructDefs_[structType->name] = {definition, structType};

    
    if (structLayouts_.find(structType->name) == structLayouts_.end()) {
        StructLayout layout = computeStructLayout(structType);
        structLayouts_[structType->name] = layout;
    }
}

// EN: Generates the LLVM type definition string for a struct.
// FR: Genere la definition LLVM de type pour une struct.
std::string IRGenerator::generateStructTypeDefinition(AST::StructType* structType) {
    if (!structType) {
        return "%struct.unknown = type { i32 }";
    }

    StructLayout layout = computeStructLayout(structType);
    return "%struct." + structType->name + " = type " + layout.llvmType;
}

} 
