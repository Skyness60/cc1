#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

bool SemanticAnalyzer::isVoidType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        return prim->kind == AST::PrimitiveKind::Void;
    }
    return false;
}

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

bool SemanticAnalyzer::isPointerType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    return dynamic_cast<AST::PointerType*>(type) != nullptr;
}

bool SemanticAnalyzer::isArrayType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    return dynamic_cast<AST::ArrayType*>(type) != nullptr;
}

bool SemanticAnalyzer::isStructType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    return dynamic_cast<AST::StructType*>(type) != nullptr;
}

bool SemanticAnalyzer::isConstQualified(AST::Type* type) {
    if (!type) return false;
    if (auto* qual = dynamic_cast<AST::QualifiedType*>(type)) {
        return qual->isConst;
    }
    return false;
}

bool SemanticAnalyzer::isScalarType(AST::Type* type) {
    return isIntegerType(type) || isFloatType(type) || isPointerType(type);
}

AST::Type* SemanticAnalyzer::stripQualifiers(AST::Type* type) {
    while (auto* qual = dynamic_cast<AST::QualifiedType*>(type)) {
        type = qual->baseType.get();
    }
    return type;
}

std::string SemanticAnalyzer::typeToString(AST::Type* type) {
    if (!type) return "unknown";
    return type->toString();
}

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

bool SemanticAnalyzer::canAssign(AST::Type* target, AST::Type* source, bool /*isInit*/) {
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

} // namespace cc1
