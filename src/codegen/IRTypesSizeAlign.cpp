#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Computes byte size for a type using target pointer width.
// FR: Calcule la taille en octets d un type selon la cible.
int IRGenerator::getTypeSize(AST::Type* type) {
    if (!type) return 0;

    type = stripQualifiers(type);

    
    if (auto* typedefType = dynamic_cast<AST::TypedefType*>(type)) {
        AST::Type* resolved = resolveTypedef(typedefType->name);
        if (resolved) {
            return getTypeSize(resolved);
        }
        return 4;  
    }

    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        return getPrimitiveSize(prim->kind);
    }

    if (dynamic_cast<AST::PointerType*>(type)) {
        return is64bit_ ? 8 : 4;
    }

    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        long long size = arr->size;
        if (size < 0 && arr->sizeExpr) {
            evaluateConstantExpr(arr->sizeExpr.get(), size);
        }
        if (size > 0) {
            return static_cast<int>(size * getTypeSize(arr->elementType.get()));
        }
        return 0;
    }

    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        if (!structType->name.empty()) {
            auto* layout = getStructLayout(structType->name);
            if (layout) return layout->totalSize;
        }

        
        StructLayout layout = computeStructLayout(structType);
        return layout.totalSize;
    }

    if (dynamic_cast<AST::EnumType*>(type)) {
        return 4;  
    }

    return 4;  
}

// EN: Computes alignment for a type using target pointer width.
// FR: Calcule l alignement d un type selon la cible.
int IRGenerator::getTypeAlign(AST::Type* type) {
    if (!type) return 1;

    type = stripQualifiers(type);

    
    if (auto* typedefType = dynamic_cast<AST::TypedefType*>(type)) {
        AST::Type* resolved = resolveTypedef(typedefType->name);
        if (resolved) {
            return getTypeAlign(resolved);
        }
        return 4;  
    }

    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        int size = getPrimitiveSize(prim->kind);
        
        int ptrSize = is64bit_ ? 8 : 4;
        return std::min(size, ptrSize);
    }

    if (dynamic_cast<AST::PointerType*>(type)) {
        return is64bit_ ? 8 : 4;
    }

    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        return getTypeAlign(arr->elementType.get());
    }

    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        if (!structType->name.empty()) {
            auto* layout = getStructLayout(structType->name);
            if (layout) return layout->alignment;
        }

        StructLayout layout = computeStructLayout(structType);
        return layout.alignment;
    }

    return 4;  
}

// EN: Returns byte size for a primitive kind.
// FR: Renvoie la taille en octets pour un type primitif.
int IRGenerator::getPrimitiveSize(AST::PrimitiveKind kind) {
    switch (kind) {
        case AST::PrimitiveKind::Void:
            return 0;
        case AST::PrimitiveKind::Char:
        case AST::PrimitiveKind::SignedChar:
        case AST::PrimitiveKind::UnsignedChar:
            return 1;
        case AST::PrimitiveKind::Short:
        case AST::PrimitiveKind::UnsignedShort:
            return 2;
        case AST::PrimitiveKind::Int:
        case AST::PrimitiveKind::UnsignedInt:
            return 4;
        case AST::PrimitiveKind::Long:
        case AST::PrimitiveKind::UnsignedLong:
            return is64bit_ ? 8 : 4;
        case AST::PrimitiveKind::LongLong:
        case AST::PrimitiveKind::UnsignedLongLong:
            return 8;
        case AST::PrimitiveKind::Float:
            return 4;
        case AST::PrimitiveKind::Double:
            return 8;
        case AST::PrimitiveKind::LongDouble:
            return is64bit_ ? 16 : 12;  
    }
    return 4;
}

} 
