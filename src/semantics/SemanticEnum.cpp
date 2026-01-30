#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// EN: Processes enum type definitions and registers enum constants.
// FR: Traite les enums et enregistre les constantes enum.
void SemanticAnalyzer::processEnumType(AST::EnumType* enumType) {
    if (!enumType) return;

    
    if (!enumType->name.empty()) {
        Symbol* existingTag = currentScope_->lookupTag(enumType->name);
        if (existingTag && existingTag->tagKind != Symbol::TagKind::Enum) {
            error(enumType->line, 1,  
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

    
    long long nextValue = 0;
    for (const auto& enumerator : enumType->enumerators) {
        if (enumerator.value) {
            
            if (hasDivisionByZero(enumerator.value.get())) {
                
                int line = enumerator.value->line;
                int col = enumerator.value->column;
                error(line, col, "expression is not an integer constant expression");
                return;
            }

            
            if (!evaluateConstantExpr(enumerator.value.get(), nextValue)) {
                
                int line = enumerator.value->line;
                int col = enumerator.value->column;
                error(line, col, "expression is not an integer constant expression");
                return;
            }
        }

        
        enumerator.computedValue = nextValue;

        
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

} 
