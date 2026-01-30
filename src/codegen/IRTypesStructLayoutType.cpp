#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Computes layout for a struct or union type.
// FR: Calcule le layout pour une struct ou union.
IRGenerator::StructLayout IRGenerator::computeStructLayout(AST::StructType* type) {
    if (!type) {
        return StructLayout{};
    }

    if (type->isUnion) {
        return computeStructLayoutUnion(type);
    }
    return computeStructLayoutStruct(type);
}

} 
