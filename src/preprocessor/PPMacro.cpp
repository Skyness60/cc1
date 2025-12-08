#include <preprocessor/PPMacro.hpp>
#include <cctype>
#include <sstream>

namespace cc1 {
namespace pp {

// ============================================================================
// MacroTable Implementation
// ============================================================================

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

// ============================================================================
// MacroExpander Implementation
// ============================================================================

MacroExpander::MacroExpander(MacroTable& table)
    : macroTable_(table) {}

std::string MacroExpander::expand(const std::string& text) {
    expandingMacros_.clear();
    return expand(text, "", 0);
}

std::string MacroExpander::expand(const std::string& text, const std::string& expandingMacro, int depth) {
    if (depth > 100) {
        // Prevent infinite recursion
        return text;
    }
    
    std::string result;
    size_t i = 0;
    
    while (i < text.size()) {
        // Skip string literals
        if (text[i] == '"') {
            result += text[i++];
            while (i < text.size() && text[i] != '"') {
                if (text[i] == '\\' && i + 1 < text.size()) {
                    result += text[i++];
                }
                result += text[i++];
            }
            if (i < text.size()) result += text[i++];
            continue;
        }
        
        // Skip character literals
        if (text[i] == '\'') {
            result += text[i++];
            while (i < text.size() && text[i] != '\'') {
                if (text[i] == '\\' && i + 1 < text.size()) {
                    result += text[i++];
                }
                result += text[i++];
            }
            if (i < text.size()) result += text[i++];
            continue;
        }
        
        // Check for identifier (potential macro)
        if (std::isalpha(text[i]) || text[i] == '_') {
            size_t start = i;
            std::string ident;
            while (i < text.size() && (std::isalnum(text[i]) || text[i] == '_')) {
                ident += text[i++];
            }
            
            // Check if it's a macro
            const MacroDefinition* macro = macroTable_.lookup(ident);
            if (macro && expandingMacros_.find(ident) == expandingMacros_.end()) {
                if (macro->isFunctionLike) {
                    // Skip whitespace
                    size_t wsStart = i;
                    while (i < text.size() && (text[i] == ' ' || text[i] == '\t')) {
                        i++;
                    }
                    
                    if (i < text.size() && text[i] == '(') {
                        // Parse arguments
                        std::vector<std::string> args = parseArguments(text, i);
                        expandingMacros_.insert(ident);
                        std::string expanded = expandMacro(*macro, args);
                        expanded = expand(expanded, ident, depth + 1);
                        expandingMacros_.erase(ident);
                        result += expanded;
                    } else {
                        // Not a function call, restore whitespace
                        i = wsStart;
                        result += ident;
                    }
                } else {
                    // Object-like macro
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

std::vector<std::string> MacroExpander::parseArguments(const std::string& text, size_t& pos) {
    std::vector<std::string> args;
    
    if (pos >= text.size() || text[pos] != '(') {
        return args;
    }
    pos++;  // Skip '('
    
    int parenDepth = 1;
    std::string currentArg;
    
    while (pos < text.size() && parenDepth > 0) {
        char c = text[pos];
        
        if (c == '(') {
            parenDepth++;
            currentArg += c;
        } else if (c == ')') {
            parenDepth--;
            if (parenDepth > 0) {
                currentArg += c;
            }
        } else if (c == ',' && parenDepth == 1) {
            args.push_back(currentArg);
            currentArg.clear();
        } else if (c == '"') {
            // String literal
            currentArg += c;
            pos++;
            while (pos < text.size() && text[pos] != '"') {
                if (text[pos] == '\\' && pos + 1 < text.size()) {
                    currentArg += text[pos++];
                }
                currentArg += text[pos++];
            }
            if (pos < text.size()) {
                currentArg += text[pos];  // closing quote
            }
        } else if (c == '\'') {
            // Character literal
            currentArg += c;
            pos++;
            while (pos < text.size() && text[pos] != '\'') {
                if (text[pos] == '\\' && pos + 1 < text.size()) {
                    currentArg += text[pos++];
                }
                currentArg += text[pos++];
            }
            if (pos < text.size()) {
                currentArg += text[pos];  // closing quote
            }
        } else {
            currentArg += c;
        }
        pos++;
    }
    
    // Add the last argument
    if (!currentArg.empty() || !args.empty()) {
        args.push_back(currentArg);
    }
    
    // Trim whitespace from arguments
    for (size_t j = 0; j < args.size(); j++) {
        std::string& arg = args[j];
        size_t start = 0;
        while (start < arg.size() && (arg[start] == ' ' || arg[start] == '\t')) {
            start++;
        }
        size_t end = arg.size();
        while (end > start && (arg[end - 1] == ' ' || arg[end - 1] == '\t')) {
            end--;
        }
        arg = arg.substr(start, end - start);
    }
    
    return args;
}

std::string MacroExpander::stringify(const std::string& arg) {
    std::string result = "\"";
    for (size_t i = 0; i < arg.size(); i++) {
        if (arg[i] == '"' || arg[i] == '\\') {
            result += '\\';
        }
        result += arg[i];
    }
    result += "\"";
    return result;
}

std::string MacroExpander::expandMacro(const MacroDefinition& macro, const std::vector<std::string>& args) {
    std::string body = macro.body;
    
    // Handle token pasting (##) first
    std::string result;
    size_t i = 0;
    
    while (i < body.size()) {
        // Check for ##
        if (i + 1 < body.size() && body[i] == '#' && body[i + 1] == '#') {
            // Remove trailing whitespace from result
            while (!result.empty() && (result.back() == ' ' || result.back() == '\t')) {
                result.pop_back();
            }
            i += 2;
            // Skip leading whitespace after ##
            while (i < body.size() && (body[i] == ' ' || body[i] == '\t')) {
                i++;
            }
            continue;
        }
        
        // Check for # (stringification)
        if (body[i] == '#') {
            i++;
            // Skip whitespace after #
            while (i < body.size() && (body[i] == ' ' || body[i] == '\t')) {
                i++;
            }
            
            // Get parameter name
            if (i < body.size() && (std::isalpha(body[i]) || body[i] == '_')) {
                std::string paramName;
                while (i < body.size() && (std::isalnum(body[i]) || body[i] == '_')) {
                    paramName += body[i++];
                }
                
                // Find parameter index
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
        
        // Check for identifier (potential parameter)
        if (std::isalpha(body[i]) || body[i] == '_') {
            std::string ident;
            while (i < body.size() && (std::isalnum(body[i]) || body[i] == '_')) {
                ident += body[i++];
            }
            
            // Check if it's a parameter
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
            
            // Check for __VA_ARGS__
            if (!isParam && ident == "__VA_ARGS__" && macro.isVariadic) {
                // Collect variadic arguments
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

std::vector<std::string> MacroExpander::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    size_t i = 0;
    
    while (i < text.size()) {
        // Skip whitespace
        while (i < text.size() && (text[i] == ' ' || text[i] == '\t')) {
            i++;
        }
        
        if (i >= text.size()) break;
        
        // Identifier
        if (std::isalpha(text[i]) || text[i] == '_') {
            std::string token;
            while (i < text.size() && (std::isalnum(text[i]) || text[i] == '_')) {
                token += text[i++];
            }
            tokens.push_back(token);
        }
        // Number
        else if (std::isdigit(text[i])) {
            std::string token;
            while (i < text.size() && (std::isalnum(text[i]) || text[i] == '.' || text[i] == 'x' || text[i] == 'X')) {
                token += text[i++];
            }
            tokens.push_back(token);
        }
        // String literal
        else if (text[i] == '"') {
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
        }
        // Character literal
        else if (text[i] == '\'') {
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
        }
        // Operators and punctuation
        else {
            std::string token;
            token += text[i++];
            tokens.push_back(token);
        }
    }
    
    return tokens;
}

} // namespace pp
} // namespace cc1
