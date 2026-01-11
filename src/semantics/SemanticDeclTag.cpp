#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

void SemanticAnalyzer::visit(AST::StructDecl& node) {
    Symbol* existingTag = currentScope_->lookupTag(node.name);
    if (existingTag) {
        Symbol::TagKind expectedKind = node.isUnion ? Symbol::TagKind::Union : Symbol::TagKind::Struct;
        if (existingTag->tagKind != expectedKind) {
            error(node.line, node.column,
                  "use of '" + node.name + "' with tag type that does not match previous declaration");
            return;
        }
    }

    Symbol sym;
    sym.name = node.name;
    sym.tagKind = node.isUnion ? Symbol::TagKind::Union : Symbol::TagKind::Struct;
    sym.line = node.line;
    sym.column = node.column;
    sym.structDecl = &node;
    currentScope_->defineTag(node.name, sym);

    for (auto& member : node.members) {
        if (member) member->accept(*this);
    }
}

void SemanticAnalyzer::visit(AST::EnumDecl& node) {
    if (!node.name.empty()) {
        Symbol* existingTag = currentScope_->lookupTag(node.name);
        if (existingTag && existingTag->tagKind != Symbol::TagKind::Enum) {
            error(node.line, 1,
                  "use of '" + node.name + "' with tag type that does not match previous declaration");
            return;
        }

        Symbol sym;
        sym.name = node.name;
        sym.tagKind = Symbol::TagKind::Enum;
        sym.line = node.line;
        sym.column = node.column;
        currentScope_->defineTag(node.name, sym);
    }

    long long nextValue = 0;
    for (auto& enumerator : node.enumerators) {
        if (enumerator.value) {
            if (hasDivisionByZero(enumerator.value.get())) {
                int col = enumerator.equalColumn > 0 ? enumerator.equalColumn : enumerator.column;
                error(enumerator.line, col, "expression is not an integer constant expression");
                return;
            }

            if (!evaluateConstantExpr(enumerator.value.get(), nextValue)) {
                int col = enumerator.equalColumn > 0 ? enumerator.equalColumn : enumerator.column;
                error(enumerator.line, col, "expression is not an integer constant expression");
                return;
            }
        }

        enumerator.computedValue = nextValue;

        Symbol sym;
        sym.name = enumerator.name;
        sym.isEnumConstant = true;
        sym.enumValue = nextValue;
        sym.line = enumerator.line;
        sym.column = enumerator.column;
        currentScope_->define(enumerator.name, sym,
                              AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, 0, 0));

        nextValue++;
    }
}

void SemanticAnalyzer::visit(AST::TypedefDecl& node) {
    Symbol sym;
    sym.name = node.name;
    sym.isTypedef = true;
    sym.line = node.line;
    sym.column = node.column;
    currentScope_->define(node.name, sym,
                          node.underlyingType ? node.underlyingType->clone() : nullptr);
}

} // namespace cc1
