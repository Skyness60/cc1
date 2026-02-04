#include <codegen/IRGenerator.hpp>

#include <iostream>

namespace cc1 {

// EN: Emits IR for identifiers by resolving symbols and enum constants.
// FR: Genere l IR pour les identifiants via symboles et enums.
void IRGenerator::visit(AST::Identifier& node) {
    
    auto enumIt = enumValues_.find(node.name);
    if (enumIt != enumValues_.end()) {
        lastValue_ = IRValue(std::to_string(enumIt->second), "i32", false, true);
        return;
    }

    
    IRSymbol* sym = lookupSymbol(node.name);
    if (!sym) {
        std::cerr << "error: unknown identifier in codegen: '" << node.name
                  << "' at " << node.line << ":" << node.column << std::endl;
        hadError_ = true;
        lastValue_ = IRValue("0", "i32", false, true);
        return;
    }

    if (sym->isFunction) {
        // Functions are not objects: in LLVM IR, a direct function reference is a
        // function value, and its pointer type is obtained by adding one '*'.
        //
        // Here, `sym->type` is already the full function type spelling like:
        //   "i8 (i32, i8)"   for: char (*)(unsigned int, char)
        //
        // So the function *value* should have type `sym->type` (NOT `sym->type + "*"`,
        // which would turn it into a pointer-to-function-pointer and break indirect calls).
        lastValue_ = IRValue(sym->irName, sym->type, false, false);
    } else {
        
        lastValue_ = IRValue(sym->irName, sym->type + "*", true, false);
    }
}

} 
