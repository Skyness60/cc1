#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Symbol Table Management
// ============================================================================

void IRGenerator::enterScope() {
    scopes_.push_back({});
}

void IRGenerator::exitScope() {
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

IRSymbol* IRGenerator::lookupSymbol(const std::string& name) {
    // Search from innermost to outermost scope
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return &found->second;
        }
    }
    return nullptr;
}

void IRGenerator::defineSymbol(const std::string& name, const IRSymbol& sym) {
    if (!scopes_.empty()) {
        scopes_.back()[name] = sym;
    }
}

} // namespace cc1
