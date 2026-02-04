#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Returns the default zero initializer for a given type.
// FR: Renvoie l initialiseur zero par defaut pour un type.
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
        // For unions, just use zeroinitializer (they're now byte arrays)
        if (structType->isUnion) {
            return "zeroinitializer";
        }
        
        if (structType->members.empty()) {
            return "zeroinitializer";
        }
        
        std::string result = "{ ";
        bool first = true;

        for (size_t i = 0; i < structType->members.size(); ++i) {
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

} 
