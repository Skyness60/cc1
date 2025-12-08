#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace cc1 {
namespace pp {

// ============================================================================
// Macro Definition
// ============================================================================

struct MacroDefinition {
    std::string name;
    std::vector<std::string> parameters;
    std::string body;
    bool isFunctionLike;
    bool isVariadic;
    
    MacroDefinition()
        : isFunctionLike(false), isVariadic(false) {}
    
    MacroDefinition(const std::string& n, const std::vector<std::string>& params,
                    const std::string& b, bool func, bool var)
        : name(n), parameters(params), body(b), isFunctionLike(func), isVariadic(var) {}
};

// ============================================================================
// Macro Table - stores macro definitions
// ============================================================================

class MacroTable {
public:
    void define(const std::string& name, const MacroDefinition& macro);
    void undefine(const std::string& name);
    bool isDefined(const std::string& name) const;
    const MacroDefinition* lookup(const std::string& name) const;
    
    // Iterator support for iterating over all macros
    typedef std::unordered_map<std::string, MacroDefinition>::const_iterator const_iterator;
    const_iterator begin() const { return macros_.begin(); }
    const_iterator end() const { return macros_.end(); }
    
private:
    std::unordered_map<std::string, MacroDefinition> macros_;
};

// ============================================================================
// Macro Expander - expands macros in text
// ============================================================================

class MacroExpander {
public:
    explicit MacroExpander(MacroTable& table);
    
    // Expand all macros in text
    std::string expand(const std::string& text);
    
private:
    MacroTable& macroTable_;
    std::unordered_set<std::string> expandingMacros_;  // Prevent infinite recursion
    
    // Internal expansion with recursion tracking
    std::string expand(const std::string& text, const std::string& expandingMacro, int depth);
    
    // Expand a single macro call
    std::string expandMacro(const MacroDefinition& macro, const std::vector<std::string>& args);
    
    // Tokenize text for macro expansion
    std::vector<std::string> tokenize(const std::string& text);
    
    // Parse macro arguments
    std::vector<std::string> parseArguments(const std::string& text, size_t& pos);
    
    // Stringify a parameter (# operator)
    std::string stringify(const std::string& arg);
};

} // namespace pp
} // namespace cc1
