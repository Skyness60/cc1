#include <preprocessor/PPMacro.hpp>

namespace cc1 {
namespace pp {

void MacroTable::define(const std::string& name, const MacroDefinition& macro) {
    macros_[name] = macro;
}

void MacroTable::undefine(const std::string& name) {
    macros_.erase(name);
}

bool MacroTable::isDefined(const std::string& name) const {
    return macros_.find(name) != macros_.end();
}

const MacroDefinition* MacroTable::lookup(const std::string& name) const {
    std::unordered_map<std::string, MacroDefinition>::const_iterator it = macros_.find(name);
    if (it != macros_.end()) {
        return &it->second;
    }
    return nullptr;
}

} // namespace pp
} // namespace cc1
