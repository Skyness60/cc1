#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// ============================================================================
// Enum Processing
// ============================================================================

void SemanticAnalyzer::processEnumType(AST::EnumType* enumType) {
    if (!enumType) return;

    // Register enum tag if named
    if (!enumType->name.empty()) {
        Symbol* existingTag = currentScope_->lookupTag(enumType->name);
        if (existingTag && existingTag->tagKind != Symbol::TagKind::Enum) {
            error(enumType->line, 1,  // Column 1 points to 'enum' keyword like clang
                  "use of '" + enumType->name + "' with tag type that does not match previous declaration");
            return;
        }

        Symbol sym;
        sym.name = enumType->name;
        sym.tagKind = Symbol::TagKind::Enum;
        sym.line = enumType->line;
        sym.column = enumType->column;
        currentScope_->defineTag(enumType->name, sym);
    }

    // Process enumerators - check for division by zero (bad_semantic_7.c)
    long long nextValue = 0;
    for (const auto& enumerator : enumType->enumerators) {
        if (enumerator.value) {
            // Check for division by zero in the value expression
            if (hasDivisionByZero(enumerator.value.get())) {
                // Point to the expression like clang does
                int line = enumerator.value->line;
                int col = enumerator.value->column;
                error(line, col, "expression is not an integer constant expression");
                return;
            }

            // Evaluate the constant expression
            if (!evaluateConstantExpr(enumerator.value.get(), nextValue)) {
                // Point to the expression like clang does
                int line = enumerator.value->line;
                int col = enumerator.value->column;
                error(line, col, "expression is not an integer constant expression");
                return;
            }
        }

        // Store computed value in AST
        enumerator.computedValue = nextValue;

        // Register enumerator as constant with its value
        Symbol sym;
        sym.name = enumerator.name;
        sym.isEnumConstant = true;
        sym.enumValue = nextValue;
        sym.line = enumType->line;
        sym.column = enumType->column;
        currentScope_->define(enumerator.name, sym, AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, 0, 0));

        nextValue++;
    }
}

} // namespace cc1
