#include <semantics/SemanticConstExprVisitors.hpp>

namespace cc1 {

// EN: Checks whether an expression can be treated as a constant expression.
// FR: Verifie si une expression peut etre consideree constante.
bool SemanticAnalyzer::isConstantExpr(AST::Expression* expr) {
    if (!expr) return false;
    ConstExprIsConstVisitor v(*this);
    expr->accept(v);
    return v.result;
}

// EN: Evaluates a constant expression and returns its integer value.
// FR: Evalue une expression constante et renvoie sa valeur entiere.
bool SemanticAnalyzer::evaluateConstantExpr(AST::Expression* expr, long long& result) {
    if (!expr) return false;
    ConstExprEvalVisitor v(*this, result);
    expr->accept(v);
    return v.ok;
}

// EN: Detects division by zero in constant-expression subtrees.
// FR: Detecte les divisions par zero dans une expression constante.
bool SemanticAnalyzer::hasDivisionByZero(AST::Expression* expr) {
    if (!expr) return false;
    ConstExprDivZeroVisitor v(*this);
    expr->accept(v);
    return v.found;
}

} 
