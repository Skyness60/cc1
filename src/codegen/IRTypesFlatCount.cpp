#include <codegen/IRGenerator.hpp>

namespace cc1 {

// Count how many flat initializers are needed to fill a struct type
// This accounts for nested structs and arrays
size_t IRGenerator::countFlattedMembers(AST::Type* type) {
    if (!type) return 0;

    type = stripQualifiers(type);

    // Primitive types: 1 initializer
    if (dynamic_cast<AST::PrimitiveType*>(type)) {
        return 1;
    }
    if (dynamic_cast<AST::PointerType*>(type)) {
        return 1;
    }

    // Arrays: count elements (not total flattened)
    // In C flat initialization, arrays take as many initializers as they have elements
    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        if (arr->size > 0) {
            return arr->size;
        }
        return 1;  // Unknown size
    }

    // Structs: sum of all member counts (recursively flattened)
    // For unions, only count 1 since they initialize only the first member
    if (auto* st = dynamic_cast<AST::StructType*>(type)) {
        if (st->isUnion) {
            return 1;  // Union consumes only one initializer
        }
        size_t total = 0;
        size_t i = 0;
        while (i < st->members.size()) {
            const auto& member = st->members[i];

            // Handle consecutive bitfields as a single unit
            if (member.isBitfield()) {
                total += 1;  // Packed bitfields count as 1 initializer
                // Skip all consecutive bitfields
                while (i < st->members.size() && st->members[i].isBitfield()) {
                    i++;
                }
            } else {
                total += countFlattedMembers(member.type.get());
                i++;
            }
        }
        return total;
    }

    return 1;
}

} // namespace cc1
