#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Enters a new symbol scope for codegen.
// FR: Entre dans un nouveau scope de symboles en codegen.
void IRGenerator::enterScope() {
    scopes_.push_back({});
}

// EN: Exits the current symbol scope.
// FR: Sort du scope de symboles courant.
void IRGenerator::exitScope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

// EN: Looks up a symbol, searching from innermost scope outward.
// FR: Cherche un symbole du scope interne vers l externe.
IRSymbol* IRGenerator::lookupSymbol(const std::string& name) {
    
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return &found->second;
        }
    }
    return nullptr;
}

// EN: Defines or updates a symbol in the current scope.
// FR: Definit ou met a jour un symbole dans le scope courant.
void IRGenerator::defineSymbol(const std::string& name, const IRSymbol& sym) {
    if (!scopes_.empty()) {
        scopes_.back()[name] = sym;
    }
}

} 
