#include <semantics/SemanticConstExprVisitors.hpp>

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

uint64_t SemanticAnalyzer::ConstExprEvalVisitor::maskUnsigned(uint64_t value, int bits) {
    if (bits >= 64) return value;
    if (bits <= 0) return 0;
    return value & ((1ULL << bits) - 1ULL);
}

int SemanticAnalyzer::ConstExprEvalVisitor::widthBitsForPrim(AST::PrimitiveKind kind) const {
    switch (kind) {
        case AST::PrimitiveKind::Char:
        case AST::PrimitiveKind::SignedChar:
        case AST::PrimitiveKind::UnsignedChar:
            return 8;
        case AST::PrimitiveKind::Short:
        case AST::PrimitiveKind::UnsignedShort:
            return 16;
        case AST::PrimitiveKind::Int:
        case AST::PrimitiveKind::UnsignedInt:
            return 32;
        case AST::PrimitiveKind::Long:
        case AST::PrimitiveKind::UnsignedLong:
            return sema.is64bit_ ? 64 : 32;
        case AST::PrimitiveKind::LongLong:
        case AST::PrimitiveKind::UnsignedLongLong:
            return 64;
        default:
            return 32;
    }
}

int SemanticAnalyzer::ConstExprEvalVisitor::inferIntWidthBits(AST::Expression* e) {
    if (!e) return 32;

    if (auto* cast = dynamic_cast<AST::CastExpr*>(e)) {
        if (auto* prim = dynamic_cast<AST::PrimitiveType*>(stripQualifiersAndTypedef(cast->targetType.get()))) {
            return widthBitsForPrim(prim->kind);
        }
    }

    if (auto* unary = dynamic_cast<AST::UnaryExpr*>(e)) {
        if (unary->op == AST::UnaryOp::BitwiseNot) {
            return inferIntWidthBits(unary->operand.get());
        }
    }

    return 32;
}

} // namespace cc1
