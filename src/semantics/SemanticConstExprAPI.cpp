#include <semantics/SemanticConstExprVisitors.hpp>

namespace cc1 {

bool SemanticAnalyzer::isConstantExpr(AST::Expression* expr) {
    if (!expr) return false;
    ConstExprIsConstVisitor v(*this);
    expr->accept(v);
    return v.result;
}

bool SemanticAnalyzer::evaluateConstantExpr(AST::Expression* expr, long long& result) {
    if (!expr) return false;
    ConstExprEvalVisitor v(*this, result);
    expr->accept(v);
    return v.ok;
}

bool SemanticAnalyzer::hasDivisionByZero(AST::Expression* expr) {
    if (!expr) return false;
    ConstExprDivZeroVisitor v(*this);
    expr->accept(v);
    return v.found;
}

} // namespace cc1
