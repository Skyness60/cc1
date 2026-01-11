#include <semantics/SemanticConstExprVisitors.hpp>

namespace cc1 {

void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::IntegerLiteral& node) {
    result = node.value;
    ok = true;
}

void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::CharLiteral& node) {
    result = node.value;
    ok = true;
}

void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::SizeofExpr& node) {
    if (node.targetType) {
        result = getTypeSize(node.targetType.get());
        ok = true;
        return;
    }
    if (node.operand) {
        AST::Ptr<AST::Type> exprType = sema.getExprType(node.operand.get());
        if (exprType) {
            result = getTypeSize(exprType.get());
            ok = true;
            return;
        }
    }
    ok = false;
}

void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::Identifier& node) {
    Symbol* sym = sema.currentScope_ ? sema.currentScope_->lookup(node.name) : nullptr;
    if (sym && sym->isEnumConstant) {
        result = sym->enumValue;
        ok = true;
        return;
    }
    ok = false;
}

} // namespace cc1
