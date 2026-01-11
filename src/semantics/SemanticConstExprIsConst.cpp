#include <semantics/SemanticConstExprVisitors.hpp>

namespace cc1 {

SemanticAnalyzer::ConstExprIsConstVisitor::ConstExprIsConstVisitor(SemanticAnalyzer& s)
    : sema(s) {}

void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::IntegerLiteral&) { result = true; }
void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::CharLiteral&) { result = true; }
void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::FloatLiteral&) { result = true; }
void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::SizeofExpr&) { result = true; }

void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::Identifier& node) {
    Symbol* sym = sema.currentScope_ ? sema.currentScope_->lookup(node.name) : nullptr;
    result = sym && sym->isEnumConstant;
}

void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::CastExpr& node) {
    if (!node.operand) {
        result = false;
        return;
    }
    ConstExprIsConstVisitor v(sema);
    node.operand->accept(v);
    result = v.result;
}

void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::UnaryExpr& node) {
    if (!node.operand) {
        result = false;
        return;
    }
    ConstExprIsConstVisitor v(sema);
    node.operand->accept(v);
    result = v.result;
}

void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::BinaryExpr& node) {
    if (!node.left || !node.right) {
        result = false;
        return;
    }

    ConstExprIsConstVisitor l(sema);
    node.left->accept(l);
    if (!l.result) {
        result = false;
        return;
    }

    ConstExprIsConstVisitor r(sema);
    node.right->accept(r);
    result = r.result;
}

void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::TernaryExpr& node) {
    if (!node.condition || !node.thenExpr || !node.elseExpr) {
        result = false;
        return;
    }

    ConstExprIsConstVisitor c(sema);
    node.condition->accept(c);
    if (!c.result) {
        result = false;
        return;
    }

    ConstExprIsConstVisitor t(sema);
    node.thenExpr->accept(t);
    if (!t.result) {
        result = false;
        return;
    }

    ConstExprIsConstVisitor e(sema);
    node.elseExpr->accept(e);
    result = e.result;
}

} // namespace cc1
