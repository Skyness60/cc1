#include <semantics/SemanticConstExprVisitors.hpp>

#include <cstdint>

namespace cc1 {

// EN: Evaluates binary constant expressions with arithmetic/comparison ops.
// FR: Evalue les binaires constantes (arithmetique/comparaison).
void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::BinaryExpr& node) {
    long long left, right;
    if (!sema.evaluateConstantExpr(node.left.get(), left)) {
        ok = false;
        return;
    }
    if (!sema.evaluateConstantExpr(node.right.get(), right)) {
        ok = false;
        return;
    }

    switch (node.op) {
        case AST::BinaryOp::Add:
            result = left + right;
            ok = true;
            return;
        case AST::BinaryOp::Sub:
            result = left - right;
            ok = true;
            return;
        case AST::BinaryOp::Mul:
            result = left * right;
            ok = true;
            return;
        case AST::BinaryOp::Div:
            if (right == 0) {
                ok = false;
                return;
            }
            result = left / right;
            ok = true;
            return;
        case AST::BinaryOp::Mod:
            if (right == 0) {
                ok = false;
                return;
            }
            {
                int bits = inferIntWidthBits(node.left.get());
                uint64_t uleft = maskUnsigned(static_cast<uint64_t>(left), bits);
                uint64_t uright = static_cast<uint64_t>(right);
                if (uright == 0) {
                    ok = false;
                    return;
                }
                result = static_cast<long long>(uleft % uright);
                ok = true;
                return;
            }
        case AST::BinaryOp::Equal:
            result = left == right ? 1 : 0;
            ok = true;
            return;
        case AST::BinaryOp::NotEqual:
            result = left != right ? 1 : 0;
            ok = true;
            return;
        case AST::BinaryOp::Less:
            result = left < right ? 1 : 0;
            ok = true;
            return;
        case AST::BinaryOp::LessEqual:
            result = left <= right ? 1 : 0;
            ok = true;
            return;
        case AST::BinaryOp::Greater:
            result = left > right ? 1 : 0;
            ok = true;
            return;
        case AST::BinaryOp::GreaterEqual:
            result = left >= right ? 1 : 0;
            ok = true;
            return;
        case AST::BinaryOp::LogicalAnd:
            result = (left && right) ? 1 : 0;
            ok = true;
            return;
        case AST::BinaryOp::LogicalOr:
            result = (left || right) ? 1 : 0;
            ok = true;
            return;
        case AST::BinaryOp::BitwiseAnd:
            result = left & right;
            ok = true;
            return;
        case AST::BinaryOp::BitwiseOr:
            result = left | right;
            ok = true;
            return;
        case AST::BinaryOp::BitwiseXor:
            result = left ^ right;
            ok = true;
            return;
        case AST::BinaryOp::LeftShift:
            result = left << right;
            ok = true;
            return;
        case AST::BinaryOp::RightShift:
            result = left >> right;
            ok = true;
            return;
        default:
            ok = false;
            return;
    }
}

} 
