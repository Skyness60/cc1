#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// EN: Infers expression type from cached info or AST shape.
// FR: Deduit le type d une expression via cache ou AST.
AST::Ptr<AST::Type> SemanticAnalyzer::getExprType(AST::Expression* expr) {
    if (!expr) return nullptr;

    auto it = exprTypes_.find(expr);
    if (it != exprTypes_.end()) {
        return it->second->clone();
    }

    if (auto* lit = dynamic_cast<AST::IntegerLiteral*>(expr)) {
        return AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, lit->line, lit->column);
    }

    if (auto* lit = dynamic_cast<AST::CharLiteral*>(expr)) {
        return AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Char, lit->line, lit->column);
    }

    if (auto* lit = dynamic_cast<AST::StringLiteral*>(expr)) {
        auto charType = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Char, lit->line, lit->column);
        auto size = AST::make<AST::IntegerLiteral>(static_cast<long long>(lit->value.length() + 1), "", lit->line, lit->column);
        return AST::make<AST::ArrayType>(std::move(charType), std::move(size), lit->line, lit->column);
    }

    if (auto* id = dynamic_cast<AST::Identifier*>(expr)) {
        Symbol* sym = currentScope_->lookup(id->name);
        if (sym && sym->type) {
            return sym->type->clone();
        }
    }

    if (auto* index = dynamic_cast<AST::IndexExpr*>(expr)) {
        AST::Ptr<AST::Type> arrayType = getExprType(index->array.get());
        if (arrayType) {
            if (auto* arr = dynamic_cast<AST::ArrayType*>(arrayType.get())) {
                if (arr->elementType) {
                    return arr->elementType->clone();
                }
            }
            if (auto* ptr = dynamic_cast<AST::PointerType*>(arrayType.get())) {
                if (ptr->pointee) {
                    return ptr->pointee->clone();
                }
            }
        }
    }

    if (auto* unary = dynamic_cast<AST::UnaryExpr*>(expr)) {
        if (unary->op == AST::UnaryOp::Dereference) {
            AST::Ptr<AST::Type> operandType = getExprType(unary->operand.get());
            if (operandType) {
                if (auto* ptr = dynamic_cast<AST::PointerType*>(operandType.get())) {
                    if (ptr->pointee) return ptr->pointee->clone();
                }
                if (auto* arr = dynamic_cast<AST::ArrayType*>(operandType.get())) {
                    if (arr->elementType) return arr->elementType->clone();
                }
            }
        } else if (unary->op == AST::UnaryOp::AddressOf) {
            AST::Ptr<AST::Type> operandType = getExprType(unary->operand.get());
            if (operandType) {
                return AST::make<AST::PointerType>(std::move(operandType), expr->line, expr->column);
            }
        } else {
            return getExprType(unary->operand.get());
        }
    }

    if (dynamic_cast<AST::SizeofExpr*>(expr)) {
        return AST::make<AST::PrimitiveType>(AST::PrimitiveKind::UnsignedLong, expr->line, expr->column);
    }

    return nullptr;
}

} 
