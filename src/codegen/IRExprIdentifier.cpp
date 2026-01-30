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
        lastValue_ = IRValue(sym->irName, sym->type + "*", false, false);
    } else {
        
        lastValue_ = IRValue(sym->irName, sym->type + "*", true, false);
    }
}

} 
