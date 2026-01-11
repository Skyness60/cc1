#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Identifier
// ============================================================================

void IRGenerator::visit(AST::Identifier& node) {
    // Check for enum constant first
    auto enumIt = enumValues_.find(node.name);
    if (enumIt != enumValues_.end()) {
        lastValue_ = IRValue(std::to_string(enumIt->second), "i32", false, true);
        return;
    }

    // Look up in symbol table
    IRSymbol* sym = lookupSymbol(node.name);
    if (!sym) {
        hadError_ = true;
        lastValue_ = IRValue("0", "i32", false, true);
        return;
    }

    if (sym->isFunction) {
        lastValue_ = IRValue(sym->irName, sym->type + "*", false, false);
    } else {
        // Variable - return pointer to it (lvalue)
        lastValue_ = IRValue(sym->irName, sym->type + "*", true, false);
    }
}

} // namespace cc1
