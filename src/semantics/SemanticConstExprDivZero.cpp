#include <semantics/SemanticConstExprVisitors.hpp>

namespace cc1 {

SemanticAnalyzer::ConstExprDivZeroVisitor::ConstExprDivZeroVisitor(SemanticAnalyzer& s)
    : sema(s) {}

void SemanticAnalyzer::ConstExprDivZeroVisitor::visit(AST::BinaryExpr& node) {
    if (node.op == AST::BinaryOp::Div || node.op == AST::BinaryOp::Mod) {
        long long rightVal;
        if (sema.evaluateConstantExpr(node.right.get(), rightVal) && rightVal == 0) {
            found = true;
            return;
        }
    }

    if (node.left) {
        ConstExprDivZeroVisitor l(sema);
        node.left->accept(l);
        if (l.found) {
            found = true;
            return;
        }
    }
    if (node.right) {
        ConstExprDivZeroVisitor r(sema);
        node.right->accept(r);
        found = r.found;
    }
}

void SemanticAnalyzer::ConstExprDivZeroVisitor::visit(AST::UnaryExpr& node) {
    if (!node.operand) {
        found = false;
        return;
    }
    ConstExprDivZeroVisitor v(sema);
    node.operand->accept(v);
    found = v.found;
}

void SemanticAnalyzer::ConstExprDivZeroVisitor::visit(AST::TernaryExpr& node) {
    long long cond;
    if (sema.evaluateConstantExpr(node.condition.get(), cond)) {
        ConstExprDivZeroVisitor v(sema);
        (cond ? node.thenExpr : node.elseExpr)->accept(v);
        found = v.found;
        return;
    }

    ConstExprDivZeroVisitor c(sema);
    node.condition->accept(c);
    if (c.found) {
        found = true;
        return;
    }
    ConstExprDivZeroVisitor t(sema);
    node.thenExpr->accept(t);
    if (t.found) {
        found = true;
        return;
    }
    ConstExprDivZeroVisitor e(sema);
    node.elseExpr->accept(e);
    found = e.found;
}

} // namespace cc1
