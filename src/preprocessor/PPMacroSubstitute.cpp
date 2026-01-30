#include <preprocessor/PPMacro.hpp>

#include <cctype>

namespace cc1 {
namespace pp {

// EN: Expands a macro body by substituting parameters, handling # and __VA_ARGS__.
// FR: Etend un corps de macro en substituant les params, gere # et __VA_ARGS__.
std::string MacroExpander::expandMacro(const MacroDefinition& macro, const std::vector<std::string>& args) {
    std::string body = macro.body;

    std::string result;
    size_t i = 0;

    while (i < body.size()) {
        if (i + 1 < body.size() && body[i] == '#' && body[i + 1] == '#') {
            while (!result.empty() && (result.back() == ' ' || result.back() == '\t')) {
                result.pop_back();
            }
            i += 2;
            while (i < body.size() && (body[i] == ' ' || body[i] == '\t')) {
                i++;
            }
            continue;
        }

        if (body[i] == '#') {
            i++;
            while (i < body.size() && (body[i] == ' ' || body[i] == '\t')) {
                i++;
            }

            if (i < body.size() &&
                (std::isalpha(static_cast<unsigned char>(body[i])) || body[i] == '_')) {
                std::string paramName;
                while (i < body.size() &&
                       (std::isalnum(static_cast<unsigned char>(body[i])) || body[i] == '_')) {
                    paramName += body[i++];
                }

                for (size_t p = 0; p < macro.parameters.size(); p++) {
                    if (macro.parameters[p] == paramName) {
                        if (p < args.size()) {
                            result += stringify(args[p]);
                        } else {
                            result += "\"\"";
                        }
                        break;
                    }
                }
            }
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(body[i])) || body[i] == '_') {
            std::string ident;
            while (i < body.size() &&
                   (std::isalnum(static_cast<unsigned char>(body[i])) || body[i] == '_')) {
                ident += body[i++];
            }

            bool isParam = false;
            for (size_t p = 0; p < macro.parameters.size(); p++) {
                if (macro.parameters[p] == ident) {
                    if (p < args.size()) {
                        result += args[p];
                    }
                    isParam = true;
                    break;
                }
            }

            if (!isParam && ident == "__VA_ARGS__" && macro.isVariadic) {
                std::string vaArgs;
                for (size_t v = macro.parameters.size() - 1; v < args.size(); v++) {
                    if (!vaArgs.empty()) {
                        vaArgs += ", ";
                    }
                    vaArgs += args[v];
                }
                result += vaArgs;
                isParam = true;
            }

            if (!isParam) {
                result += ident;
            }
        } else {
            result += body[i++];
        }
    }

    return result;
}

// EN: Tokenizes text into identifiers, numbers, strings, and single chars.
// FR: Tokenise le texte en identifiants, nombres, chaines et chars simples.
std::vector<std::string> MacroExpander::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    size_t i = 0;

    while (i < text.size()) {
        while (i < text.size() && (text[i] == ' ' || text[i] == '\t')) {
            i++;
        }

        if (i >= text.size()) {
            break;
        }

        if (std::isalpha(static_cast<unsigned char>(text[i])) || text[i] == '_') {
            std::string token;
            while (i < text.size() &&
                   (std::isalnum(static_cast<unsigned char>(text[i])) || text[i] == '_')) {
                token += text[i++];
            }
            tokens.push_back(token);
        } else if (std::isdigit(static_cast<unsigned char>(text[i]))) {
            std::string token;
            while (i < text.size() &&
                   (std::isalnum(static_cast<unsigned char>(text[i])) || text[i] == '.' ||
                    text[i] == 'x' || text[i] == 'X')) {
                token += text[i++];
            }
            tokens.push_back(token);
        } else if (text[i] == '"') {
            std::string token;
            token += text[i++];
            while (i < text.size() && text[i] != '"') {
                if (text[i] == '\\' && i + 1 < text.size()) {
                    token += text[i++];
                }
                token += text[i++];
            }
            if (i < text.size()) {
                token += text[i++];
            }
            tokens.push_back(token);
        } else if (text[i] == '\'') {
            std::string token;
            token += text[i++];
            while (i < text.size() && text[i] != '\'') {
                if (text[i] == '\\' && i + 1 < text.size()) {
                    token += text[i++];
                }
                token += text[i++];
            }
            if (i < text.size()) {
                token += text[i++];
            }
            tokens.push_back(token);
        } else {
            std::string token;
            token += text[i++];
            tokens.push_back(token);
        }
    }

    return tokens;
}

} 
} 

// TODO(cc1) EN: Make tokenization match full preprocessing token grammar.
// FR: Aligner la tokenisation sur la grammaire complete de pretraitement.
