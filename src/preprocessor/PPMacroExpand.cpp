#include <preprocessor/PPMacro.hpp>

#include <cctype>

namespace cc1 {
namespace pp {

MacroExpander::MacroExpander(MacroTable& table)
    : macroTable_(table) {}

std::string MacroExpander::expand(const std::string& text) {
    expandingMacros_.clear();
    return expand(text, "", 0);
}

std::string MacroExpander::expand(const std::string& text, const std::string& expandingMacro, int depth) {
    static_cast<void>(expandingMacro);

    if (depth > 100) {
        return text;
    }

    std::string result;
    size_t i = 0;

    while (i < text.size()) {
        if (text[i] == '"') {
            result += text[i++];
            while (i < text.size() && text[i] != '"') {
                if (text[i] == '\\' && i + 1 < text.size()) {
                    result += text[i++];
                }
                result += text[i++];
            }
            if (i < text.size()) {
                result += text[i++];
            }
            continue;
        }

        if (text[i] == '\'') {
            result += text[i++];
            while (i < text.size() && text[i] != '\'') {
                if (text[i] == '\\' && i + 1 < text.size()) {
                    result += text[i++];
                }
                result += text[i++];
            }
            if (i < text.size()) {
                result += text[i++];
            }
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(text[i])) || text[i] == '_') {
            std::string ident;
            while (i < text.size() &&
                   (std::isalnum(static_cast<unsigned char>(text[i])) || text[i] == '_')) {
                ident += text[i++];
            }

            const MacroDefinition* macro = macroTable_.lookup(ident);
            if (macro && expandingMacros_.find(ident) == expandingMacros_.end()) {
                if (macro->isFunctionLike) {
                    size_t wsStart = i;
                    while (i < text.size() && (text[i] == ' ' || text[i] == '\t')) {
                        i++;
                    }

                    if (i < text.size() && text[i] == '(') {
                        std::vector<std::string> args = parseArguments(text, i);
                        expandingMacros_.insert(ident);
                        std::string expanded = expandMacro(*macro, args);
                        expanded = expand(expanded, ident, depth + 1);
                        expandingMacros_.erase(ident);
                        result += expanded;
                    } else {
                        i = wsStart;
                        result += ident;
                    }
                } else {
                    expandingMacros_.insert(ident);
                    std::string expanded = macro->body;
                    expanded = expand(expanded, ident, depth + 1);
                    expandingMacros_.erase(ident);
                    result += expanded;
                }
            } else {
                result += ident;
            }
        } else {
            result += text[i++];
        }
    }

    return result;
}

} // namespace pp
} // namespace cc1
