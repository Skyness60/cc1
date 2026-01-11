#include <codegen/IRGenerator.hpp>

namespace cc1 {

std::string IRGenerator::getDefaultValue(AST::Type* type) {
    if (!type) return "0";

    type = stripQualifiers(type);

    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        switch (prim->kind) {
            case AST::PrimitiveKind::Float:
                return "0.0";
            case AST::PrimitiveKind::Double:
            case AST::PrimitiveKind::LongDouble:
                return "0.0";
            default:
                return "0";
        }
    }

    if (dynamic_cast<AST::PointerType*>(type)) {
        return "null";
    }

    if (dynamic_cast<AST::ArrayType*>(type)) {
        return "zeroinitializer";
    }

    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        // For structs, generate proper initializer with all members defaulted
        // For unions, only initialize the first member
        std::string result = "{ ";
        bool first = true;

        size_t memberCount = structType->isUnion ? 1 : structType->members.size();
        for (size_t i = 0; i < memberCount; ++i) {
            if (!first) result += ", ";
            first = false;
            const auto& member = structType->members[i];
            std::string memberType = typeToLLVM(stripQualifiers(member.type.get()));
            result += memberType + " " + getDefaultValue(stripQualifiers(member.type.get()));
        }
        result += " }";
        return result;

    }

    return "0";
}

} // namespace cc1
