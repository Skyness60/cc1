#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// EN: Checks if a type is void after stripping qualifiers.
// FR: Verifie si un type est void apres retrait des qualifiers.
bool SemanticAnalyzer::isVoidType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        return prim->kind == AST::PrimitiveKind::Void;
    }
    return false;
}

// EN: Checks if a type is any integer or enum kind.
// FR: Verifie si un type est un entier ou enum.
bool SemanticAnalyzer::isIntegerType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        switch (prim->kind) {
            case AST::PrimitiveKind::Char:
            case AST::PrimitiveKind::SignedChar:
            case AST::PrimitiveKind::UnsignedChar:
            case AST::PrimitiveKind::Short:
            case AST::PrimitiveKind::UnsignedShort:
            case AST::PrimitiveKind::Int:
            case AST::PrimitiveKind::UnsignedInt:
            case AST::PrimitiveKind::Long:
            case AST::PrimitiveKind::UnsignedLong:
            case AST::PrimitiveKind::LongLong:
            case AST::PrimitiveKind::UnsignedLongLong:
                return true;
            default:
                return false;
        }
    }
    if (dynamic_cast<AST::EnumType*>(type)) {
        return true;
    }
    return false;
}

// EN: Checks if a type is a floating-point kind.
// FR: Verifie si un type est flottant.
bool SemanticAnalyzer::isFloatType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        switch (prim->kind) {
            case AST::PrimitiveKind::Float:
            case AST::PrimitiveKind::Double:
            case AST::PrimitiveKind::LongDouble:
                return true;
            default:
                return false;
        }
    }
    return false;
}

// EN: Checks if a type is a pointer.
// FR: Verifie si un type est un pointeur.
bool SemanticAnalyzer::isPointerType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    return dynamic_cast<AST::PointerType*>(type) != nullptr;
}

// EN: Checks if a type is an array.
// FR: Verifie si un type est un tableau.
bool SemanticAnalyzer::isArrayType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    return dynamic_cast<AST::ArrayType*>(type) != nullptr;
}

// EN: Checks if a type is a struct.
// FR: Verifie si un type est une struct.
bool SemanticAnalyzer::isStructType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    return dynamic_cast<AST::StructType*>(type) != nullptr;
}

// EN: Checks whether a type is const-qualified.
// FR: Verifie si un type est qualifie const.
bool SemanticAnalyzer::isConstQualified(AST::Type* type) {
    if (!type) return false;
    if (auto* qual = dynamic_cast<AST::QualifiedType*>(type)) {
        return qual->isConst;
    }
    return false;
}

// EN: Checks if a type is scalar (int/float/pointer).
// FR: Verifie si un type est scalaire (entier/float/pointeur).
bool SemanticAnalyzer::isScalarType(AST::Type* type) {
    return isIntegerType(type) || isFloatType(type) || isPointerType(type);
}

// EN: Strips top-level qualifier wrappers to get base type.
// FR: Retire les qualifiers de tete pour obtenir le type de base.
AST::Type* SemanticAnalyzer::stripQualifiers(AST::Type* type) {
    while (auto* qual = dynamic_cast<AST::QualifiedType*>(type)) {
        type = qual->baseType.get();
    }
    return type;
}

// EN: Returns a readable string for a type, or "unknown".
// FR: Renvoie une chaine lisible du type, ou "unknown".
std::string SemanticAnalyzer::typeToString(AST::Type* type) {
    if (!type) return "unknown";
    return type->toString();
}

// EN: Checks assignment/compatibility between two types.
// FR: Verifie la compatibilite entre deux types.
bool SemanticAnalyzer::typesCompatible(AST::Type* left, AST::Type* right) {
    if (!left || !right) return false;

    left = stripQualifiers(left);
    right = stripQualifiers(right);

    if (left->equals(*right)) return true;

    if (isIntegerType(left) && isIntegerType(right)) return true;
    if (isFloatType(left) && isFloatType(right)) return true;
    if ((isIntegerType(left) && isFloatType(right)) || (isFloatType(left) && isIntegerType(right))) return true;

    if (isPointerType(left) && isPointerType(right)) {
        auto* ptrLeft = dynamic_cast<AST::PointerType*>(left);
        auto* ptrRight = dynamic_cast<AST::PointerType*>(right);

        if (isVoidType(ptrLeft->pointee.get()) || isVoidType(ptrRight->pointee.get())) {
            return true;
        }

        return typesCompatible(ptrLeft->pointee.get(), ptrRight->pointee.get());
    }

    if (isPointerType(left) && isArrayType(right)) {
        auto* ptr = dynamic_cast<AST::PointerType*>(left);
        auto* arr = dynamic_cast<AST::ArrayType*>(right);
        return typesCompatible(ptr->pointee.get(), arr->elementType.get());
    }
    if (isArrayType(left) && isPointerType(right)) {
        auto* arr = dynamic_cast<AST::ArrayType*>(left);
        auto* ptr = dynamic_cast<AST::PointerType*>(right);
        return typesCompatible(arr->elementType.get(), ptr->pointee.get());
    }

    return false;
}

// EN: Determines if a value of source type can be assigned to target.
// FR: Determine si une valeur source peut etre assignee a cible.
bool SemanticAnalyzer::canAssign(AST::Type* target, AST::Type* source, bool ) {
    if (!target || !source) return false;

    AST::Type* targetBase = stripQualifiers(target);
    AST::Type* sourceBase = stripQualifiers(source);

    if (isVoidType(targetBase)) return false;
    if (isIntegerType(targetBase) && isIntegerType(sourceBase)) return true;
    if (isFloatType(targetBase) && isFloatType(sourceBase)) return true;
    if ((isIntegerType(targetBase) && isFloatType(sourceBase)) ||
        (isFloatType(targetBase) && isIntegerType(sourceBase))) return true;

    if (isPointerType(targetBase) && isPointerType(sourceBase)) {
        auto* ptrTarget = dynamic_cast<AST::PointerType*>(targetBase);
        auto* ptrSource = dynamic_cast<AST::PointerType*>(sourceBase);

        if (isVoidType(ptrTarget->pointee.get()) || isVoidType(ptrSource->pointee.get())) {
            return true;
        }

        return typesCompatible(ptrTarget->pointee.get(), ptrSource->pointee.get());
    }

    if (isPointerType(targetBase) && isArrayType(sourceBase)) {
        auto* ptr = dynamic_cast<AST::PointerType*>(targetBase);
        auto* arr = dynamic_cast<AST::ArrayType*>(sourceBase);
        return typesCompatible(ptr->pointee.get(), arr->elementType.get());
    }

    if (isStructType(targetBase) && isStructType(sourceBase)) {
        return targetBase->equals(*sourceBase);
    }

    return false;
}

} 
