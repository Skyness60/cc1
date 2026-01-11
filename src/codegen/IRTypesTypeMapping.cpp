#include <codegen/IRGenerator.hpp>

namespace cc1 {

std::string IRGenerator::nextAnonStructName() {
    static int anonCounter = 0;
    return "anon." + std::to_string(anonCounter++);
}

std::string IRGenerator::typeToLLVM(AST::Type* type) {
    if (!type) return "void";

    type = stripQualifiers(type);

    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        return primitiveToLLVM(prim->kind);
    }

    if (auto* ptr = dynamic_cast<AST::PointerType*>(type)) {
        std::string pointeeType = typeToLLVM(ptr->pointee.get());
        // In LLVM, void* is not valid - use i8* instead
        if (pointeeType == "void") {
            return "i8*";
        }
        return pointeeType + "*";
    }

    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        std::string elemType = typeToLLVM(arr->elementType.get());
        long long size = arr->size;
        if (size < 0 && arr->sizeExpr) {
            evaluateConstantExpr(arr->sizeExpr.get(), size);
        }
        if (size > 0) {
            return "[" + std::to_string(size) + " x " + elemType + "]";
        }
        // Unsized array - treat as pointer
        return elemType + "*";
    }

    if (auto* func = dynamic_cast<AST::FunctionType*>(type)) {
        std::string result = typeToLLVM(func->returnType.get()) + " (";
        for (size_t i = 0; i < func->parameterTypes.size(); ++i) {
            if (i > 0) result += ", ";
            result += typeToLLVM(func->parameterTypes[i].get());
        }
        if (func->isVariadic) {
            if (!func->parameterTypes.empty()) result += ", ";
            result += "...";
        }
        result += ")*";
        return result;
    }

    if (auto* typedefType = dynamic_cast<AST::TypedefType*>(type)) {
        // Resolve typedef to its underlying type
        AST::Type* resolved = resolveTypedef(typedefType->name);
        if (resolved) {
            return typeToLLVM(resolved);
        }
        // Fallback if not resolved
        return "i32";
    }

    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        if (structType->name.empty()) {
            // Anonymous struct/union types are distinct in C.
            // If we inline them, multiple different anonymous structs can collapse to the
            // same LLVM type string (e.g., "{ i8 }") which breaks member/bitfield access.
            // Give them a unique internal name and emit them as named %struct.*.
            structType->name = nextAnonStructName();
        }
        // Named struct (including auto-named anonymous): register for module-level definition.
        collectNamedStruct(structType);
        return "%struct." + structType->name;
    }

    if (auto* enumType = dynamic_cast<AST::EnumType*>(type)) {
        (void)enumType;
        return is64bit_ ? "i32" : "i32";
    }

    return "i32";  // Default fallback
}

std::string IRGenerator::primitiveToLLVM(AST::PrimitiveKind kind) {
    switch (kind) {
        case AST::PrimitiveKind::Void:
            return "void";
        case AST::PrimitiveKind::Char:
        case AST::PrimitiveKind::SignedChar:
        case AST::PrimitiveKind::UnsignedChar:
            return "i8";
        case AST::PrimitiveKind::Short:
        case AST::PrimitiveKind::UnsignedShort:
            return "i16";
        case AST::PrimitiveKind::Int:
        case AST::PrimitiveKind::UnsignedInt:
            return "i32";
        case AST::PrimitiveKind::Long:
        case AST::PrimitiveKind::UnsignedLong:
            return is64bit_ ? "i64" : "i32";
        case AST::PrimitiveKind::LongLong:
        case AST::PrimitiveKind::UnsignedLongLong:
            return "i64";
        case AST::PrimitiveKind::Float:
            return "float";
        case AST::PrimitiveKind::Double:
            return "double";
        case AST::PrimitiveKind::LongDouble:
            return is64bit_ ? "x86_fp80" : "x86_fp80";
    }
    return "i32";
}

} // namespace cc1
