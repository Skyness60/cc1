#pragma once



#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace cc1 {
namespace pp {





// EN: Represents a macro definition (object-like or function-like) as stored
// by the preprocessor. FR: Represente une definition de macro (objet ou fonction).
struct MacroDefinition {
    std::string name;
    std::vector<std::string> parameters;
    std::string body;
    bool isFunctionLike;
    bool isVariadic;
    
    // EN: Default-constructs an empty macro with flags disabled.
    // FR: Construit une macro vide avec flags desactives.
    MacroDefinition()
        : isFunctionLike(false), isVariadic(false) {}
    
    // EN: Builds a macro definition with full metadata (name, params, body).
    // FR: Construit une definition complete (nom, params, corps).
    MacroDefinition(const std::string& n, const std::vector<std::string>& params,
                    const std::string& b, bool func, bool var)
        : name(n), parameters(params), body(b), isFunctionLike(func), isVariadic(var) {}
};





// EN: Stores macro definitions and provides lookup/update operations.
// FR: Stocke les macros et fournit les operations de lookup/mise a jour.
class MacroTable {
public:
    // EN: Defines or replaces a macro by name.
    // FR: Definit ou remplace une macro par nom.
    void define(const std::string& name, const MacroDefinition& macro);
    // EN: Removes a macro definition.
    // FR: Supprime une definition de macro.
    void undefine(const std::string& name);
    // EN: Checks whether a macro is defined.
    // FR: Verifie si une macro est definie.
    bool isDefined(const std::string& name) const;
    // EN: Looks up a macro definition; returns null if absent.
    // FR: Cherche une macro ; renvoie null si absente.
    const MacroDefinition* lookup(const std::string& name) const;
    
    
    // EN: Provides iteration over macro definitions for diagnostics/debugging.
    // FR: Fournit une iteration pour diagnostic/debug.
    typedef std::unordered_map<std::string, MacroDefinition>::const_iterator const_iterator;
    const_iterator begin() const { return macros_.begin(); }
    const_iterator end() const { return macros_.end(); }
    
private:
    std::unordered_map<std::string, MacroDefinition> macros_;
};





// EN: Expands macro invocations within text while preventing infinite recursion.
// FR: Etend les macros dans un texte tout en evitant la recursion infinie.
class MacroExpander {
public:
    // EN: Binds the expander to a macro table for lookups.
    // FR: Lie l expandeur a la table de macros.
    explicit MacroExpander(MacroTable& table);
    
    
    // EN: Expands all macros in the provided text.
    // FR: Etend toutes les macros dans le texte.
    std::string expand(const std::string& text);
    
private:
    MacroTable& macroTable_;
    std::unordered_set<std::string> expandingMacros_;  
    
    
    // EN: Internal expansion with recursion tracking and depth limiting.
    // FR: Expansion interne avec suivi de recursion et limite de profondeur.
    std::string expand(const std::string& text, const std::string& expandingMacro, int depth);
    
    
    // EN: Expands a specific macro with its argument list.
    // FR: Etend une macro specifique avec ses arguments.
    std::string expandMacro(const MacroDefinition& macro, const std::vector<std::string>& args);
    
    
    // EN: Tokenizes text for macro detection/expansion.
    // FR: Tokenise le texte pour detecter/etendre les macros.
    std::vector<std::string> tokenize(const std::string& text);
    
    
    // EN: Parses macro arguments from text at a given position.
    // FR: Parse les arguments d une macro depuis une position.
    std::vector<std::string> parseArguments(const std::string& text, size_t& pos);
    
    
    // EN: Implements stringification (#) for macro arguments.
    // FR: Implemente la stringification (#) des arguments.
    std::string stringify(const std::string& arg);
};

// TODO(cc1) EN: Support token-pasting (##) to match full C macro behavior.
// FR: Supporter le collage de tokens (##) pour coller au C complet.
} 
} 
