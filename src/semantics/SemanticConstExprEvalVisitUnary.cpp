#include <semantics/SemanticConstExprVisitors.hpp>

#include <cstdint>

namespace cc1 {

void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::UnaryExpr& node) {
    if (!node.operand) {
        ok = false;
        return;
    }

    if (node.op == AST::UnaryOp::Dereference) {
        AST::Ptr<AST::Type> operandType = sema.getExprType(node.operand.get());
        if (operandType) {
            AST::Type* t = operandType.get();
            while (auto* qual = dynamic_cast<AST::QualifiedType*>(t)) {
                t = qual->baseType.get();
            }
            if (auto* td = dynamic_cast<AST::TypedefType*>(t)) {
                if (td->underlyingType) t = td->underlyingType.get();
            }

            if (auto* ptr = dynamic_cast<AST::PointerType*>(t)) {
                if (ptr->pointee) {
                    result = getTypeSize(ptr->pointee.get());
                    ok = true;
                    return;
                }
            }
            if (auto* arr = dynamic_cast<AST::ArrayType*>(t)) {
                if (arr->elementType) {
                    result = getTypeSize(arr->elementType.get());
                    ok = true;
                    return;
                }
            }
        }
    }

    long long operand;
    if (!sema.evaluateConstantExpr(node.operand.get(), operand)) {
        ok = false;
        return;
    }

    switch (node.op) {
        case AST::UnaryOp::Plus:
            result = operand;
            ok = true;
            return;
        case AST::UnaryOp::Negate:
            result = -operand;
            ok = true;
            return;
        case AST::UnaryOp::BitwiseNot: {
            int bits = inferIntWidthBits(node.operand.get());
            uint64_t uop = maskUnsigned(static_cast<uint64_t>(operand), bits);
            uint64_t ures = maskUnsigned(~uop, bits);
            result = static_cast<long long>(ures);
            ok = true;
            return;
        }
        case AST::UnaryOp::LogicalNot:
            result = !operand ? 1 : 0;
            ok = true;
            return;
        default:
            ok = false;
            return;
    }
}

} // namespace cc1
