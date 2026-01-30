#include <semantics/SemanticConstExprVisitors.hpp>

#include <cstdint>

namespace cc1 {

// EN: Evaluates ternary constant expressions by selecting a branch.
// FR: Evalue les ternaires constantes en choisissant une branche.
void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::TernaryExpr& node) {
    long long cond;
    if (!sema.evaluateConstantExpr(node.condition.get(), cond)) {
        ok = false;
        return;
    }
    if (cond) {
        ok = sema.evaluateConstantExpr(node.thenExpr.get(), result);
    } else {
        ok = sema.evaluateConstantExpr(node.elseExpr.get(), result);
    }
}

// EN: Applies integer cast semantics to constant-expression results.
// FR: Applique les conversions entieres aux resultats constants.
void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::CastExpr& node) {
    if (!sema.evaluateConstantExpr(node.operand.get(), result)) {
        ok = false;
        return;
    }

    AST::Type* t = node.targetType.get();
    while (auto* qual = dynamic_cast<AST::QualifiedType*>(t)) {
        t = qual->baseType.get();
    }
    if (auto* td = dynamic_cast<AST::TypedefType*>(t)) {
        if (td->underlyingType) t = td->underlyingType.get();
    }

    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(t)) {
        switch (prim->kind) {
            case AST::PrimitiveKind::UnsignedChar:
                result = static_cast<long long>(static_cast<uint8_t>(result));
                ok = true;
                return;
            case AST::PrimitiveKind::UnsignedShort:
                result = static_cast<long long>(static_cast<uint16_t>(result));
                ok = true;
                return;
            case AST::PrimitiveKind::UnsignedInt:
                result = static_cast<long long>(static_cast<uint32_t>(result));
                ok = true;
                return;
            case AST::PrimitiveKind::UnsignedLong:
                result = static_cast<long long>(sema.is64bit_ ? static_cast<uint64_t>(result)
                                                             : static_cast<uint32_t>(result));
                ok = true;
                return;
            case AST::PrimitiveKind::UnsignedLongLong:
                result = static_cast<long long>(static_cast<uint64_t>(result));
                ok = true;
                return;
            case AST::PrimitiveKind::SignedChar:
                result = static_cast<long long>(static_cast<int8_t>(result));
                ok = true;
                return;
            case AST::PrimitiveKind::Short:
                result = static_cast<long long>(static_cast<int16_t>(result));
                ok = true;
                return;
            case AST::PrimitiveKind::Int:
                result = static_cast<long long>(static_cast<int32_t>(result));
                ok = true;
                return;
            case AST::PrimitiveKind::Long:
                result = static_cast<long long>(sema.is64bit_ ? static_cast<int64_t>(result)
                                                             : static_cast<int32_t>(result));
                ok = true;
                return;
            case AST::PrimitiveKind::LongLong:
                result = static_cast<long long>(static_cast<int64_t>(result));
                ok = true;
                return;
            default:
                break;
        }
    }

    ok = true;
}

} 
