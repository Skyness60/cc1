#include <semantics/SemanticConstExprVisitors.hpp>

namespace cc1 {

// EN: Initializes the const-expression checker visitor.
// FR: Initialise le visiteur de verification des expressions constantes.
SemanticAnalyzer::ConstExprIsConstVisitor::ConstExprIsConstVisitor(SemanticAnalyzer& s)
    : sema(s) {}

// EN: Integer literals are constant expressions.
// FR: Les entiers litteraux sont des expressions constantes.
void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::IntegerLiteral&) { result = true; }
// EN: Char literals are constant expressions.
// FR: Les caracteres litteraux sont des expressions constantes.
void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::CharLiteral&) { result = true; }
// EN: Float literals are constant expressions.
// FR: Les flottants litteraux sont des expressions constantes.
void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::FloatLiteral&) { result = true; }
// EN: sizeof is a constant expression.
// FR: sizeof est une expression constante.
void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::SizeofExpr&) { result = true; }

// EN: Identifiers are constant only for enum constants.
// FR: Les identifiants sont constants seulement pour enum constants.
void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::Identifier& node) {
    Symbol* sym = sema.currentScope_ ? sema.currentScope_->lookup(node.name) : nullptr;
    result = sym && sym->isEnumConstant;
}

// EN: Cast is constant if its operand is constant.
// FR: Un cast est constant si son operande l est.
void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::CastExpr& node) {
    if (!node.operand) {
        result = false;
        return;
    }
    ConstExprIsConstVisitor v(sema);
    node.operand->accept(v);
    result = v.result;
}

// EN: Unary is constant if its operand is constant.
// FR: Un unaire est constant si son operande l est.
void SemanticAnalyzer::ConstExprIsConstVisitor::visit(AST::UnaryExpr& node) {
    if (!node.operand) {
        result = false;
        return;
    }
    ConstExprIsConstVisitor v(sema);
    node.operand->accept(v);
    result = v.result;
}

// EN: Binary is constant if both operands are constant.
// FR: Un binaire est constant si les deux operandes le sont.
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

// EN: Ternary is constant if all branches are constant.
// FR: Un ternaire est constant si toutes les branches le sont.
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

} 
