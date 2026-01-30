#include <semantics/SemanticConstExprVisitors.hpp>

namespace cc1 {

// EN: Initializes the division-by-zero visitor with semantic context.
// FR: Initialise le visiteur de division par zero avec le contexte sema.
SemanticAnalyzer::ConstExprDivZeroVisitor::ConstExprDivZeroVisitor(SemanticAnalyzer& s)
    : sema(s) {}

// EN: Checks binary expressions for division/modulo by zero.
// FR: Verifie les binaires pour division/modulo par zero.
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

// EN: Visits unary expressions and propagates the found flag.
// FR: Visite les unaires et propage l indicateur trouve.
void SemanticAnalyzer::ConstExprDivZeroVisitor::visit(AST::UnaryExpr& node) {
    if (!node.operand) {
        found = false;
        return;
    }
    ConstExprDivZeroVisitor v(sema);
    node.operand->accept(v);
    found = v.found;
}

// EN: Evaluates ternary branches to detect any division by zero.
// FR: Evalue les branches ternaire pour detecter une division par zero.
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

} 
