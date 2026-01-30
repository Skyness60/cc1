#include <semantics/SemanticConstExprVisitors.hpp>

namespace cc1 {

// EN: Evaluates integer literal to its value.
// FR: Evalue un literal entier a sa valeur.
void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::IntegerLiteral& node) {
    result = node.value;
    ok = true;
}

// EN: Evaluates char literal to its numeric value.
// FR: Evalue un literal caractere a sa valeur numerique.
void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::CharLiteral& node) {
    result = node.value;
    ok = true;
}

// EN: Evaluates sizeof on a type or operand expression.
// FR: Evalue sizeof sur un type ou une expression.
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

// EN: Resolves enum constants to their value in constant eval.
// FR: Resout les constantes enum vers leur valeur en eval const.
void SemanticAnalyzer::ConstExprEvalVisitor::visit(AST::Identifier& node) {
    Symbol* sym = sema.currentScope_ ? sema.currentScope_->lookup(node.name) : nullptr;
    if (sym && sym->isEnumConstant) {
        result = sym->enumValue;
        ok = true;
        return;
    }
    ok = false;
}

} 
