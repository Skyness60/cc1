#include <preprocessor/PPUtils.hpp>
#include <cctype>

namespace cc1 {
namespace pp {

// EN: Removes line/block comments while preserving newlines and literals.
// FR: Supprime les commentaires en preservant les sauts de ligne et les litteraux.
std::string CommentRemover::remove(const std::string& source) {
    std::string result;
    result.reserve(source.size());
    
    bool inLineComment = false;
    bool inBlockComment = false;
    bool inString = false;
    bool inChar = false;
    
    for (size_t i = 0; i < source.size(); ++i) {
        char c = source[i];
        char next = (i + 1 < source.size()) ? source[i + 1] : '\0';
        
        if (inLineComment) {
            if (c == '\n') {
                inLineComment = false;
                result += c;
            }
            continue;
        }
        
        if (inBlockComment) {
            if (c == '*' && next == '/') {
                inBlockComment = false;
                result += ' ';  
                ++i;
            } else if (c == '\n') {
                result += c;  
            }
            continue;
        }
        
        if (inString) {
            result += c;
            if (c == '\\' && i + 1 < source.size()) {
                result += source[++i];  
            } else if (c == '"') {
                inString = false;
            }
            continue;
        }
        
        if (inChar) {
            result += c;
            if (c == '\\' && i + 1 < source.size()) {
                result += source[++i];  
            } else if (c == '\'') {
                inChar = false;
            }
            continue;
        }
        
        
        if (c == '"') {
            inString = true;
            result += c;
        } else if (c == '\'') {
            inChar = true;
            result += c;
        } else if (c == '/' && next == '/') {
            inLineComment = true;
            ++i;
        } else if (c == '/' && next == '*') {
            inBlockComment = true;
            ++i;
        } else {
            result += c;
        }
    }
    
    return result;
}

// EN: Merges adjacent string literals across whitespace to match C rules.
// FR: Fusionne les chaines adjacentes a travers les espaces selon les regles C.
std::string StringConcatenator::concatenate(const std::string& source) {
    std::string result;
    result.reserve(source.size());
    
    size_t i = 0;
    while (i < source.size()) {
        
        if (std::isspace(source[i])) {
            result += source[i++];
            continue;
        }
        
        
        if (source[i] == '"') {
            auto parsed = parseString(source, i);
            std::string combined = parsed.first;
            bool terminated = parsed.second;
            
            if (!terminated) {
                result += combined;
                continue;
            }
            
            
            while (i < source.size()) {
                size_t savedPos = i;
                
                
                while (i < source.size() && (source[i] == ' ' || source[i] == '\t' || source[i] == '\n')) {
                    i++;
                }
                
                if (i < source.size() && source[i] == '"') {
                    
                    auto next = parseString(source, i);
                    
                    if (!next.second) {
                        i = savedPos;
                        break;
                    }
                    
                    combined = combined.substr(0, combined.size() - 1) + next.first.substr(1);
                } else {
                    
                    i = savedPos;
                    break;
                }
            }
            
            result += combined;
        } else {
            result += source[i++];
        }
    }
    
    return result;
}

// EN: Parses one quoted string and advances the cursor safely.
// FR: Parse une chaine guillemetee et avance le curseur en securite.
std::pair<std::string, bool> StringConcatenator::parseString(const std::string& source, size_t& pos) {
    std::string result;
    bool terminated = false;
    
    if (pos >= source.size() || source[pos] != '"') {
        return {result, terminated};
    }
    
    result += source[pos++];  
    
    while (pos < source.size()) {
        char c = source[pos++];
        result += c;
        
        if (c == '\\' && pos < source.size()) {
            result += source[pos++];  
        } else if (c == '"') {
            terminated = true;
            break;  
        }
    }
    
    return {result, terminated};
}

} 
} 

// TODO(cc1) EN: Improve handling of unterminated strings/comments with errors.
// FR: Mieux gerer les chaines/commentaires non termines avec erreurs.
