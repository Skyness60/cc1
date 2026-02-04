#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Generates a unique name for anonymous struct types.
// FR: Genere un nom unique pour les structs anonymes.
std::string IRGenerator::nextAnonStructName() {
    static int anonCounter = 0;
    return "anon." + std::to_string(anonCounter++);
}

// EN: Maps AST types to LLVM IR type strings.
// FR: Mappe les types AST vers les types LLVM IR.
std::string IRGenerator::typeToLLVM(AST::Type* type) {
    if (!type) return "void";

    type = stripQualifiers(type);

    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        return primitiveToLLVM(prim->kind);
    }

    if (auto* ptr = dynamic_cast<AST::PointerType*>(type)) {
        std::string pointeeType = typeToLLVM(ptr->pointee.get());
        
        // Pointer-to-function: AST wraps the function type in PointerType.
        // Function types themselves must NOT include a trailing '*'.
        if (!pointeeType.empty() && pointeeType.back() == ')') {
            return pointeeType + "*";
        }

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
        
        return elemType + "*";
    }

    if (auto* func = dynamic_cast<AST::FunctionType*>(type)) {
        // IMPORTANT:
        // An AST::FunctionType should map to an LLVM *function type* "ret (args)".
        // Do NOT append '*' here. The pointer indirection is handled by AST::PointerType.
        std::string result = typeToLLVM(func->returnType.get()) + " (";
        for (size_t i = 0; i < func->parameterTypes.size(); ++i) {
            if (i > 0) result += ", ";
            result += typeToLLVM(func->parameterTypes[i].get());
        }
        if (func->isVariadic) {
            if (!func->parameterTypes.empty()) result += ", ";
            result += "...";
        }
        result += ")";
        return result;
    }

    if (auto* typedefType = dynamic_cast<AST::TypedefType*>(type)) {
        
        AST::Type* resolved = resolveTypedef(typedefType->name);
        if (resolved) {
            return typeToLLVM(resolved);
        }
        
        return "i32";
    }

    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        if (structType->name.empty()) {
            
            
            
            
            structType->name = nextAnonStructName();
        }
        
        collectNamedStruct(structType);
        return "%struct." + structType->name;
    }

    if (auto* enumType = dynamic_cast<AST::EnumType*>(type)) {
        (void)enumType;
        return is64bit_ ? "i32" : "i32";
    }

    return "i32";  
}

// EN: Maps primitive kinds to LLVM IR type strings.
// FR: Mappe les types primitifs vers les types LLVM IR.
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

} 
