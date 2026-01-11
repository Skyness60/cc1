#include <codegen/IRGenerator.hpp>

namespace cc1 {

std::string IRGenerator::generateInitializerValue(AST::Type* type, AST::InitializerList* initList) {
    if (!type || !initList) return "zeroinitializer";

    type = stripQualifiers(type);

    if (auto* arrayType = dynamic_cast<AST::ArrayType*>(type)) {
        return generateArrayInitializerValue(arrayType, initList);
    }

    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        return generateStructInitializerValue(structType, initList);
    }

    // For other types, return zeroinitializer
    return "zeroinitializer";
}

} // namespace cc1
