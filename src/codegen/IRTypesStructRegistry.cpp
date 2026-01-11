#include <codegen/IRGenerator.hpp>

namespace cc1 {

const IRGenerator::StructLayout* IRGenerator::getStructLayout(const std::string& name) {
    auto it = structLayouts_.find(name);
    if (it != structLayouts_.end()) {
        return &it->second;
    }
    return nullptr;
}

void IRGenerator::collectNamedStruct(AST::StructType* structType) {
    if (!structType) {
        return;  // Null struct type
    }

    if (structType->name.empty()) {
        return;  // Not a named struct
    }

    // Check if already collected
    if (namedStructDefs_.count(structType->name)) {
        return;
    }

    // First, ensure all nested structs have names and are collected
    if (!structType->members.empty()) {
        for (const auto& member : structType->members) {
            if (member.type) {
                AST::Type* nestedType = stripQualifiers(member.type.get());
                if (auto* nestedStruct = dynamic_cast<AST::StructType*>(nestedType)) {
                    // Give anonymous nested structs a generated name
                    if (nestedStruct->name.empty()) {
                        nestedStruct->name = nextAnonStructName();
                    }
                    // Recursively collect nested structs FIRST
                    collectNamedStruct(nestedStruct);
                }
            }
        }
    }

    // Now generate definition for this struct (nested structs already defined)
    std::string definition = generateStructTypeDefinition(structType);
    namedStructDefs_[structType->name] = {definition, structType};

    // Also compute and register the layout for member access
    if (structLayouts_.find(structType->name) == structLayouts_.end()) {
        StructLayout layout = computeStructLayout(structType);
        structLayouts_[structType->name] = layout;
    }
}

std::string IRGenerator::generateStructTypeDefinition(AST::StructType* structType) {
    if (!structType) {
        return "%struct.unknown = type { i32 }";
    }

    StructLayout layout = computeStructLayout(structType);
    return "%struct." + structType->name + " = type " + layout.llvmType;
}

} // namespace cc1
