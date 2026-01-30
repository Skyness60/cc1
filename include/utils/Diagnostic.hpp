#pragma once



#include <string>
#include <iostream>
#include <utils/SourceLocation.hpp>
#include <utils/color.hpp>

// EN: Kinds of diagnostics supported by the printer.
// FR: Types de diagnostics supportes par le printer.
enum class DiagnosticKind {
    Error,
    Warning,
    Note
};

// EN: Formats and prints diagnostics with optional source context.
// FR: Formate et affiche les diagnostics avec contexte source optionnel.
class DiagnosticPrinter {
public:
    // EN: Prints a diagnostic to stderr immediately.
    // FR: Affiche un diagnostic sur stderr immediatement.
    static void print(DiagnosticKind kind, const SourceLocation& loc,
                      const std::string& message, const std::string& sourceLine = "") {
        std::cerr << format(kind, loc, message, sourceLine) << std::endl;
    }
    
    // EN: Formats a diagnostic into a string for logging.
    // FR: Formate un diagnostic en string pour journalisation.
    static std::string format(DiagnosticKind kind, const SourceLocation& loc,
                              const std::string& message, const std::string& sourceLine = "") {
        std::string result;
        
        
        result += WHITE + loc.toString() + ": " RESET;
        
        
        result += kindToString(kind);
        
        
        result += WHITE + message + RESET "\n";
        
        
        if (!sourceLine.empty()) {
            result += formatSourceLine(loc.line, loc.column, sourceLine);
        }
        
        return result;
    }

private:
    // EN: Maps diagnostic kinds to colored prefixes.
    // FR: Mappe les types vers des prefixes colores.
    static std::string kindToString(DiagnosticKind kind) {
        switch (kind) {
            case DiagnosticKind::Error:   return RED "error: " RESET;
            case DiagnosticKind::Warning: return MAGENTA "warning: " RESET;
            case DiagnosticKind::Note:    return CYAN "note: " RESET;
        }
        return "";
    }
    
    // EN: Builds a source line caret marker for the diagnostic.
    // FR: Construit le marqueur de source (caret) pour le diagnostic.
    static std::string formatSourceLine(int line, int column, const std::string& sourceLine) {
        std::string result;
        std::string lineNumStr = std::to_string(line);
        
        
        result += "    " + lineNumStr + " | " + sourceLine + "\n";
        
        
        
        result += "    " + std::string(lineNumStr.length(), ' ') + " | ";
        
        
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
