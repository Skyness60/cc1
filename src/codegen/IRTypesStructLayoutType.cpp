#include <codegen/IRGenerator.hpp>

namespace cc1 {

IRGenerator::StructLayout IRGenerator::computeStructLayout(AST::StructType* type) {
    if (!type) {
        return StructLayout{};
    }

    if (type->isUnion) {
        return computeStructLayoutUnion(type);
    }
    return computeStructLayoutStruct(type);
}

} // namespace cc1
