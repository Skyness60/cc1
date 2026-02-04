#pragma once



#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <utils/SourceLocation.hpp>
#include <functional>

// EN: Scans string literals, including escapes and diagnostics.
// FR: Scanne les litteraux chaine, avec echappements et diagnostics.
class StringScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&, const SourceLocation&)>;
    
    // EN: Initializes with filename and error handler for diagnostics.
    // FR: Initialise avec le nom de fichier et le handler d erreur.
    StringScanner(const std::string& filename, ErrorHandler onError)
        : filename_(filename), onError_(std::move(onError)) {}
    
    // EN: Starts when the first character is a double quote.
    // FR: Demarre si le premier caractere est un guillemet.
    bool canScan(char c) const override { return c == '"'; }
    
    // EN: Scans a string literal, handling escapes and unterminated cases.
    // FR: Scanne une chaine, gere les echappements et les cas non termines.
    Token scan(SourceReader& reader) override {
        // NOTE: the opening quote has already been consumed by the lexer.
        SourceLocation start(filename_, reader.line(), reader.column() - 1);
        std::string text = "\"";
        
        while (reader.peek() != '"' && reader.peek() != '\0' && reader.peek() != '\n') {
            if (reader.peek() == '\\') {
                text += reader.advance();
                // Backslash-newline: allow line continuation in string literals.
                if (reader.peek() == '\n') { text += reader.advance(); continue; }
                if (reader.peek() == '\0') break;
                text += reader.advance();
            } else {
                text += reader.advance();
            }
        }
        
        if (reader.peek() != '"') {
            // Unterminated string literal (newline or EOF reached).
            // Clang emits "missing terminating \" character" at the opening quote.
            onError_("missing terminating \" character", start);
            return Token(TokenType::StringLiteral, text, start.line, start.column);
        }
        
        text += reader.advance(); // consume closing quote
        return Token(TokenType::StringLiteral, text, start.line, start.column);
    }

private:
    std::string filename_;
    ErrorHandler onError_;
};

// TODO(cc1) EN: Support wide/UTF prefixes (L, u8, u, U) if required.
// FR: Supporter les prefixes wide/UTF (L, u8, u, U) si necessaire.
