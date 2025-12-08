#include <preprocessor/PPUtils.hpp>
#include <cctype>

namespace cc1 {
namespace pp {

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
                result += ' ';  // Replace comment with space
                ++i;
            } else if (c == '\n') {
                result += c;  // Preserve newlines
            }
            continue;
        }
        
        if (inString) {
            result += c;
            if (c == '\\' && i + 1 < source.size()) {
                result += source[++i];  // Skip escaped character
            } else if (c == '"') {
                inString = false;
            }
            continue;
        }
        
        if (inChar) {
            result += c;
            if (c == '\\' && i + 1 < source.size()) {
                result += source[++i];  // Skip escaped character
            } else if (c == '\'') {
                inChar = false;
            }
            continue;
        }
        
        // Not in any special context
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

std::string StringConcatenator::concatenate(const std::string& source) {
    std::string result;
    result.reserve(source.size());
    
    size_t i = 0;
    while (i < source.size()) {
        // Skip whitespace
        if (std::isspace(source[i])) {
            result += source[i++];
            continue;
        }
        
        // Check for string literal
        if (source[i] == '"') {
            std::string combined = parseString(source, i);
            
            // Look for adjacent string literals
            while (i < source.size()) {
                size_t savedPos = i;
                
                // Skip whitespace between strings
                while (i < source.size() && (source[i] == ' ' || source[i] == '\t' || source[i] == '\n')) {
                    i++;
                }
                
                if (i < source.size() && source[i] == '"') {
                    // Another string literal - concatenate (without quotes)
                    std::string next = parseString(source, i);
                    // Remove quotes and concatenate
                    combined = combined.substr(0, combined.size() - 1) + next.substr(1);
                } else {
                    // Not a string - restore position and break
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

std::string StringConcatenator::parseString(const std::string& source, size_t& pos) {
    std::string result;
    
    if (pos >= source.size() || source[pos] != '"') {
        return result;
    }
    
    result += source[pos++];  // Opening quote
    
    while (pos < source.size()) {
        char c = source[pos++];
        result += c;
        
        if (c == '\\' && pos < source.size()) {
            result += source[pos++];  // Escape sequence
        } else if (c == '"') {
            break;  // End of string
        }
    }
    
    return result;
}

} // namespace pp
} // namespace cc1
