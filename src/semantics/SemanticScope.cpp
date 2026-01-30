#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {





// EN: Looks up a symbol by name through parent scopes.
// FR: Cherche un symbole par nom dans les scopes parents.
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

// EN: Looks up a tag symbol by name through parent scopes.
// FR: Cherche un tag par nom dans les scopes parents.
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

// EN: Looks up a symbol only in the current scope.
// FR: Cherche un symbole seulement dans le scope courant.
Symbol* Scope::lookupLocal(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return &it->second;
    }
    return nullptr;
}

// EN: Defines a symbol and optionally stores an owned type.
// FR: Definit un symbole et stocke un type possede optionnel.
void Scope::define(const std::string& name, const Symbol& sym, AST::Ptr<AST::Type> type) {
    Symbol newSym = sym;
    if (type) {
        ownedTypes.push_back(std::move(type));
        newSym.type = ownedTypes.back().get();
    }
    symbols[name] = newSym;
}

// EN: Defines a tag symbol in the current scope.
// FR: Definit un tag dans le scope courant.
void Scope::defineTag(const std::string& name, const Symbol& sym) {
    tags[name] = sym;
}

} 

// TODO(cc1) EN: Add scope dump/debug helpers for diagnostics.
// FR: Ajouter des helpers de debug pour les scopes.
