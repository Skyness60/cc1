#include <semantics/SemanticConstExprVisitors.hpp>

#include <algorithm>

namespace cc1 {
namespace {

AST::Type* stripQualifiersAndTypedef(AST::Type* type) {
    if (!type) return nullptr;
    while (auto* qual = dynamic_cast<AST::QualifiedType*>(type)) {
        type = qual->baseType.get();
    }
    if (auto* td = dynamic_cast<AST::TypedefType*>(type)) {
        if (td->underlyingType) {
            type = td->underlyingType.get();
            return stripQualifiersAndTypedef(type);
        }
    }
    return type;
}

} // namespace

int SemanticAnalyzer::ConstExprEvalVisitor::getPrimitiveSize(AST::PrimitiveKind kind) const {
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
            return sema.is64bit_ ? 8 : 4;
        case AST::PrimitiveKind::LongLong:
        case AST::PrimitiveKind::UnsignedLongLong:
            return 8;
        case AST::PrimitiveKind::Float:
            return 4;
        case AST::PrimitiveKind::Double:
            return 8;
        case AST::PrimitiveKind::LongDouble:
            return sema.is64bit_ ? 16 : 12;
    }
    return 4;
}

int SemanticAnalyzer::ConstExprEvalVisitor::getTypeAlign(AST::Type* type) {
    if (!type) return 1;
    type = stripQualifiersAndTypedef(type);

    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        int size = getPrimitiveSize(prim->kind);
        int ptrSize = sema.is64bit_ ? 8 : 4;
        return std::min(size, ptrSize);
    }
    if (dynamic_cast<AST::PointerType*>(type)) {
        return sema.is64bit_ ? 8 : 4;
    }
    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        return getTypeAlign(arr->elementType.get());
    }
    if (auto* st = dynamic_cast<AST::StructType*>(type)) {
        int maxAlign = 1;
        for (const auto& m : st->members) {
            if (!m.type) continue;
            maxAlign = std::max(maxAlign, getTypeAlign(m.type.get()));
        }
        return maxAlign;
    }
    if (dynamic_cast<AST::EnumType*>(type)) {
        return 4;
    }
    return 4;
}

} // namespace cc1
