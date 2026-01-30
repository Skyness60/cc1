#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Evaluates floating constant expressions for compile-time init.
// FR: Evalue les expressions constantes flottantes pour init compile-time.
bool IRGenerator::evaluateConstantFloatExpr(AST::Expression* expr, double& result) {
    if (!expr) return false;

    if (auto* lit = dynamic_cast<AST::FloatLiteral*>(expr)) {
        result = lit->value;
        return true;
    }

    if (auto* lit = dynamic_cast<AST::IntegerLiteral*>(expr)) {
        result = static_cast<double>(lit->value);
        return true;
    }

    if (auto* lit = dynamic_cast<AST::CharLiteral*>(expr)) {
        result = static_cast<double>(lit->value);
        return true;
    }

    if (auto* id = dynamic_cast<AST::Identifier*>(expr)) {
        auto it = enumValues_.find(id->name);
        if (it != enumValues_.end()) {
            result = static_cast<double>(it->second);
            return true;
        }
        return false;
    }

    if (auto* unary = dynamic_cast<AST::UnaryExpr*>(expr)) {
        double operand;
        if (!evaluateConstantFloatExpr(unary->operand.get(), operand)) {
            long long intOperand;
            if (!evaluateConstantExpr(unary->operand.get(), intOperand)) return false;
            operand = static_cast<double>(intOperand);
        }

        switch (unary->op) {
            case AST::UnaryOp::Plus:
                result = operand;
                return true;
            case AST::UnaryOp::Negate:
                result = -operand;
                return true;
            case AST::UnaryOp::LogicalNot:
                result = (operand == 0.0) ? 1.0 : 0.0;
                return true;
            default:
                return false;
        }
    }

    if (auto* bin = dynamic_cast<AST::BinaryExpr*>(expr)) {
        double left, right;
        if (!evaluateConstantFloatExpr(bin->left.get(), left)) {
            long long intLeft;
            if (!evaluateConstantExpr(bin->left.get(), intLeft)) return false;
            left = static_cast<double>(intLeft);
        }
        if (!evaluateConstantFloatExpr(bin->right.get(), right)) {
            long long intRight;
            if (!evaluateConstantExpr(bin->right.get(), intRight)) return false;
            right = static_cast<double>(intRight);
        }

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
                if (right == 0.0) return false;
                result = left / right;
                return true;
            case AST::BinaryOp::Equal:
                result = (left == right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::NotEqual:
                result = (left != right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::Less:
                result = (left < right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::LessEqual:
                result = (left <= right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::Greater:
                result = (left > right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::GreaterEqual:
                result = (left >= right) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::LogicalAnd:
                result = (left != 0.0 && right != 0.0) ? 1.0 : 0.0;
                return true;
            case AST::BinaryOp::LogicalOr:
                result = (left != 0.0 || right != 0.0) ? 1.0 : 0.0;
                return true;
            default:
                return false;
        }
    }

    if (auto* tern = dynamic_cast<AST::TernaryExpr*>(expr)) {
        double cond;
        if (!evaluateConstantFloatExpr(tern->condition.get(), cond)) {
            long long intCond;
            if (!evaluateConstantExpr(tern->condition.get(), intCond)) return false;
            cond = static_cast<double>(intCond);
        }
        if (cond != 0.0) {
            return evaluateConstantFloatExpr(tern->thenExpr.get(), result);
        }
        return evaluateConstantFloatExpr(tern->elseExpr.get(), result);
    }

    if (auto* sizeofExpr = dynamic_cast<AST::SizeofExpr*>(expr)) {
        if (sizeofExpr->targetType) {
            result = static_cast<double>(getTypeSize(sizeofExpr->targetType.get()));
            return true;
        }
        return false;
    }

    if (auto* castExpr = dynamic_cast<AST::CastExpr*>(expr)) {
        double operand;
        if (!evaluateConstantFloatExpr(castExpr->operand.get(), operand)) {
            long long intOperand;
            if (!evaluateConstantExpr(castExpr->operand.get(), intOperand)) return false;
            operand = static_cast<double>(intOperand);
        }

        AST::Type* target = stripQualifiers(castExpr->targetType.get());
        if (auto* prim = dynamic_cast<AST::PrimitiveType*>(target)) {
            switch (prim->kind) {
                case AST::PrimitiveKind::Float:
                    result = static_cast<double>(static_cast<float>(operand));
                    return true;
                case AST::PrimitiveKind::Double:
                    result = operand;
                    return true;
                case AST::PrimitiveKind::LongDouble:
                    result = operand;
                    return true;
                default:
                    break;
            }
        }
        return false;
    }

    return false;
}

} 
