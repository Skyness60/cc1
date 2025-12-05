#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// ============================================================================
// Scope Implementation
// ============================================================================

Symbol* Scope::lookup(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return &it->second;
    }
    if (parent) {
        return parent->lookup(name);
    }
    return nullptr;
}

Symbol* Scope::lookupTag(const std::string& name) {
    auto it = tags.find(name);
    if (it != tags.end()) {
        return &it->second;
    }
    if (parent) {
        return parent->lookupTag(name);
    }
    return nullptr;
}

Symbol* Scope::lookupLocal(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return &it->second;
    }
    return nullptr;
}

void Scope::define(const std::string& name, const Symbol& sym, AST::Ptr<AST::Type> type) {
    Symbol newSym = sym;
    if (type) {
        ownedTypes.push_back(std::move(type));
        newSym.type = ownedTypes.back().get();
    }
    symbols[name] = newSym;
}

void Scope::defineTag(const std::string& name, const Symbol& sym) {
    tags[name] = sym;
}

} // namespace cc1
