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

        // Colorize builtin C type keywords in the diagnostic message (clang-like).
        std::string coloredMessage = colorizeBuiltinTypes(message);
        
        
        result += WHITE + loc.toString() + ": " RESET;
        
        
        result += kindToString(kind);
        
        
        result += WHITE + coloredMessage + RESET "\n";
        
        
        if (!sourceLine.empty()) {
            result += formatSourceLine(loc.line, loc.column, sourceLine, message);
        }
        
        return result;
    }

private:
    static bool isIdentChar(char c) {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               (c >= '0' && c <= '9') ||
               c == '_';
    }

    static bool isBuiltinTypeWord(const std::string& w) {
        // Builtin type keywords + common qualifiers/specifiers that appear in type spellings.
        // Keep conservative: only whole-word matches.
        if (w == "void") return true;
        if (w == "char") return true;
        if (w == "short") return true;
        if (w == "int") return true;
        if (w == "long") return true;
        if (w == "signed") return true;
        if (w == "unsigned") return true;
        if (w == "float") return true;
        if (w == "double") return true;
        if (w == "_Bool") return true;

        // Qualifiers often printed as part of a type.
        if (w == "const") return true;
        if (w == "volatile") return true;
        if (w == "restrict") return true;

        // Tag keywords used in type spellings.
        if (w == "struct") return true;
        if (w == "union") return true;
        if (w == "enum") return true;

        return false;
    }

    // EN: Wraps builtin type keywords in BLUE/RESET, matching clang's "types in blue" style.
    // FR: Colore les mots-clefs de types builtin en bleu (style clang).
    static std::string colorizeBuiltinTypes(const std::string& message) {
        std::string out;
        out.reserve(message.size());

        for (size_t i = 0; i < message.size();) {
            if (isIdentChar(message[i])) {
                size_t j = i;
                while (j < message.size() && isIdentChar(message[j])) ++j;
                std::string word = message.substr(i, j - i);

                if (isBuiltinTypeWord(word)) {
                    out += BLUE;
                    out += word;
                    out += RESET;
                } else {
                    out += word;
                }

                i = j;
            } else {
                out += message[i++];
            }
        }

        return out;
    }

    // EN: Colorizes the type specifier segment at the start of a source line (best-effort).
    // FR: Colore la partie "type" au debut d'une ligne source (best-effort).
    //
    // Example: "char a;" -> "char" in blue.
    //          "const unsigned long x;" -> "const unsigned long" in blue.
    static std::string colorizeLeadingTypeInSourceLine(const std::string& sourceLine) {
        // Find first non-space/tab.
        size_t i = 0;
        while (i < sourceLine.size() && (sourceLine[i] == ' ' || sourceLine[i] == '\t')) ++i;
        if (i >= sourceLine.size()) return sourceLine;

        // Consume a sequence of "type words" possibly separated by whitespace,
        // allowing one '*' attached to the type (e.g., "char *a").
        size_t start = i;
        size_t end = i;

        auto skipSpaces = [&](size_t& k) {
            while (k < sourceLine.size() && (sourceLine[k] == ' ' || sourceLine[k] == '\t')) ++k;
        };

        size_t k = i;
        bool consumedAny = false;
        while (k < sourceLine.size()) {
            skipSpaces(k);
            if (k >= sourceLine.size()) break;

            // Identifier-like word.
            if (isIdentChar(sourceLine[k])) {
                size_t w0 = k;
                while (k < sourceLine.size() && isIdentChar(sourceLine[k])) ++k;
                std::string word = sourceLine.substr(w0, k - w0);

                if (!isBuiltinTypeWord(word)) {
                    // Stop before the first non-type word (likely the variable/function name).
                    break;
                }

                consumedAny = true;
                end = k;
                continue;
            }

            // Allow '*' as part of the type segment (e.g. "char * a").
            if (sourceLine[k] == '*') {
                ++k;
                end = k;
                continue;
            }

            // Stop on any other char.
            break;
        }

        if (!consumedAny || end <= start) return sourceLine;

        // Rebuild: prefix + BLUE(typeSegment)+RESET + rest
        std::string out;
        out.reserve(sourceLine.size() + 16);
        out.append(sourceLine.substr(0, start));
        out.append(BLUE);
        out.append(sourceLine.substr(start, end - start));
        out.append(RESET);
        out.append(sourceLine.substr(end));
        return out;
    }

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
    static std::string formatSourceLine(int line, int column, const std::string& sourceLine, const std::string& message) {
        std::string result;
        std::string lineNumStr = std::to_string(line);
        
        
        std::string coloredSourceLine = colorizeLeadingTypeInSourceLine(sourceLine);
        result += "    " + lineNumStr + " | " + coloredSourceLine + "\n";
        
        
        
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
        if (message.find("expected ';'") != std::string::npos) {
            result += "\n";
            result += "    " + std::string(lineNumStr.length(), ' ') + " | " + spacing + GREEN + ";" + RESET;
        }
        
        return result;
    }
};
