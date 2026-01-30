#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Evaluates integer constant expressions for compile-time init.
// FR: Evalue les expressions constantes entieres pour init compile-time.
bool IRGenerator::evaluateConstantExpr(AST::Expression* expr, long long& result) {
    if (!expr) return false;

    if (auto* lit = dynamic_cast<AST::IntegerLiteral*>(expr)) {
        result = lit->value;
        return true;
    }

    if (auto* lit = dynamic_cast<AST::CharLiteral*>(expr)) {
        result = lit->value;
        return true;
    }

    if (auto* id = dynamic_cast<AST::Identifier*>(expr)) {
        auto it = enumValues_.find(id->name);
        if (it != enumValues_.end()) {
            result = it->second;
            return true;
        }
        return false;
    }

    if (auto* unary = dynamic_cast<AST::UnaryExpr*>(expr)) {
        long long operand;
        if (!evaluateConstantExpr(unary->operand.get(), operand)) return false;

        switch (unary->op) {
            case AST::UnaryOp::Plus:
                result = operand;
                return true;
            case AST::UnaryOp::Negate:
                result = -operand;
                return true;
            case AST::UnaryOp::BitwiseNot:
                result = ~operand;
                return true;
            case AST::UnaryOp::LogicalNot:
                result = !operand ? 1 : 0;
                return true;
            default:
                return false;
        }
    }

    if (auto* bin = dynamic_cast<AST::BinaryExpr*>(expr)) {
        long long left, right;
        if (!evaluateConstantExpr(bin->left.get(), left)) return false;
        if (!evaluateConstantExpr(bin->right.get(), right)) return false;

        switch (bin->op) {
            case AST::BinaryOp::Add:
                result = left + right;
                return true;
            case AST::BinaryOp::Sub:
                result = left - right;
                return true;
            case AST::BinaryOp::Mul:
                result = left * right;
                return true;
            case AST::BinaryOp::Div:
                if (right == 0) return false;
                result = left / right;
                return true;
            case AST::BinaryOp::Mod:
                if (right == 0) return false;
                result = left % right;
                return true;
            case AST::BinaryOp::BitwiseAnd:
                result = left & right;
                return true;
            case AST::BinaryOp::BitwiseOr:
                result = left | right;
                return true;
            case AST::BinaryOp::BitwiseXor:
                result = left ^ right;
                return true;
            case AST::BinaryOp::LeftShift:
                result = left << right;
                return true;
            case AST::BinaryOp::RightShift:
                result = left >> right;
                return true;
            case AST::BinaryOp::Equal:
                result = (left == right) ? 1 : 0;
                return true;
            case AST::BinaryOp::NotEqual:
                result = (left != right) ? 1 : 0;
                return true;
            case AST::BinaryOp::Less:
                result = (left < right) ? 1 : 0;
                return true;
            case AST::BinaryOp::LessEqual:
                result = (left <= right) ? 1 : 0;
                return true;
            case AST::BinaryOp::Greater:
                result = (left > right) ? 1 : 0;
                return true;
            case AST::BinaryOp::GreaterEqual:
                result = (left >= right) ? 1 : 0;
                return true;
            case AST::BinaryOp::LogicalAnd:
                result = (left && right) ? 1 : 0;
                return true;
            case AST::BinaryOp::LogicalOr:
                result = (left || right) ? 1 : 0;
                return true;
            default:
                return false;
        }
    }

    if (auto* tern = dynamic_cast<AST::TernaryExpr*>(expr)) {
        long long cond;
        if (!evaluateConstantExpr(tern->condition.get(), cond)) return false;
        if (cond) {
            return evaluateConstantExpr(tern->thenExpr.get(), result);
        }
        return evaluateConstantExpr(tern->elseExpr.get(), result);
    }

    if (auto* sizeofExpr = dynamic_cast<AST::SizeofExpr*>(expr)) {
        if (sizeofExpr->targetType) {
            result = getTypeSize(sizeofExpr->targetType.get());
            return true;
        }
        
        return false;
    }

    return false;
}

} 
