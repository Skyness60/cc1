#include <preprocessor/PPMacro.hpp>

namespace cc1 {
namespace pp {

// EN: Defines or replaces a macro entry by name.
// FR: Definit ou remplace une entree de macro par nom.
void MacroTable::define(const std::string& name, const MacroDefinition& macro) {
    macros_[name] = macro;
}

// EN: Removes a macro entry by name.
// FR: Supprime une entree de macro par nom.
void MacroTable::undefine(const std::string& name) {
    macros_.erase(name);
}

// EN: Checks whether a macro name exists.
// FR: Verifie si un nom de macro existe.
bool MacroTable::isDefined(const std::string& name) const {
    return macros_.find(name) != macros_.end();
}

// EN: Looks up a macro and returns nullptr if missing.
// FR: Cherche une macro et renvoie nullptr si absente.
const MacroDefinition* MacroTable::lookup(const std::string& name) const {
    std::unordered_map<std::string, MacroDefinition>::const_iterator it = macros_.find(name);
    if (it != macros_.end()) {
        return &it->second;
    }
    return nullptr;
}

} 
} 

// TODO(cc1) EN: Track macro redefinition diagnostics (same/different bodies).
// FR: Suivre les diagnostics de redef de macro (corps identique/different).
