#pragma once

#include <string>
#include <iostream>
#include <utils/SourceLocation.hpp>
#include <utils/color.hpp>

enum class DiagnosticKind {
    Error,
    Warning,
    Note
};

class DiagnosticPrinter {
public:
    static void print(DiagnosticKind kind, const SourceLocation& loc,
                      const std::string& message, const std::string& sourceLine = "") {
        std::cerr << format(kind, loc, message, sourceLine) << std::endl;
    }
    
    static std::string format(DiagnosticKind kind, const SourceLocation& loc,
                              const std::string& message, const std::string& sourceLine = "") {
        std::string result;
        
        // Location: file:line:col:
        result += WHITE + loc.toString() + ": " RESET;
        
        // Kind: error/warning/note
        result += kindToString(kind);
        
        // Message
        result += WHITE + message + RESET "\n";
        
        // Source line with caret
        if (!sourceLine.empty()) {
            result += formatSourceLine(loc.line, loc.column, sourceLine);
        }
        
        return result;
    }

private:
    static std::string kindToString(DiagnosticKind kind) {
        switch (kind) {
            case DiagnosticKind::Error:   return RED "error: " RESET;
            case DiagnosticKind::Warning: return MAGENTA "warning: " RESET;
            case DiagnosticKind::Note:    return CYAN "note: " RESET;
        }
        return "";
    }
    
    static std::string formatSourceLine(int line, int column, const std::string& sourceLine) {
        std::string result;
        std::string lineNumStr = std::to_string(line);
        
        // Line with source code: "    N | source"
        result += "    " + lineNumStr + " | " + sourceLine + "\n";
        
        // Caret line: "      | ^"
        // We need to handle tabs by preserving them in the caret line
        result += "    " + std::string(lineNumStr.length(), ' ') + " | ";
        
        // Build the spacing to reach the caret position, preserving tabs
        std::string spacing;
        int pos = 1;
        for (size_t i = 0; i < sourceLine.length() && pos < column; ++i, ++pos) {
            if (sourceLine[i] == '\t') {
                spacing += '\t';
            } else {
                spacing += ' ';
            }
        }
        result += GREEN + spacing + "^" RESET;
        
        return result;
    }
};
