#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Generates an LLVM initializer for a type and initializer list.
// FR: Genere un initialiseur LLVM pour un type et une liste d init.
std::string IRGenerator::generateInitializerValue(AST::Type* type, AST::InitializerList* initList) {
    if (!type || !initList) return "zeroinitializer";

    type = stripQualifiers(type);

    if (auto* arrayType = dynamic_cast<AST::ArrayType*>(type)) {
        return generateArrayInitializerValue(arrayType, initList);
    }

    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        // For unions, use a simple byte initializer representation
        if (structType->isUnion) {
            // Unions are now represented as byte arrays, so return a byte initializer
            return "zeroinitializer";
        }
        return generateStructInitializerValue(structType, initList);
    }

    
    return "zeroinitializer";
}

} 
