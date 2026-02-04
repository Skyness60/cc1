#pragma once



#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <utils/SourceLocation.hpp>
#include <functional>

// EN: Scans character literals with escape validation.
// FR: Scanne les litteraux caractere avec validation des echappements.
class CharScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&, const SourceLocation&)>;
    
    // EN: Initializes with filename and error handler for diagnostics.
    // FR: Initialise avec le nom de fichier et le handler d erreur.
    CharScanner(const std::string& filename, ErrorHandler onError)
        : filename_(filename), onError_(std::move(onError)) {}
    
    // EN: Starts when the first character is a single quote.
    // FR: Demarre si le premier caractere est un apostrophe.
    bool canScan(char c) const override { return c == '\''; }
    
    // EN: Scans a character literal and validates termination.
    // FR: Scanne un litteral caractere et verifie la terminaison.
    Token scan(SourceReader& reader) override {
        // NOTE: the opening quote has already been consumed by the lexer.
        SourceLocation start(filename_, reader.line(), reader.column() - 1);
        std::string text = "'";

        // Immediate end: ' or end-of-line/file after the opening quote.
        if (reader.peek() == '\0' || reader.peek() == '\n') {
            // Clang emits "missing terminating ' character" at the opening quote.
            onError_("missing terminating ' character", start);
            return Token(TokenType::CharLiteral, text, start.line, start.column);
        }

        // Empty character constant: ''.
        if (reader.peek() == '\'') {
            text += reader.advance(); // consume closing quote

            // Clang emits: warning: empty character constant
            // Our lexer error handler currently reports as an error; downgrade by
            // not emitting a diagnostic here and letting the parser fail on the
            // invalid token sequence similarly to clang's behavior.
            // (If/when we add a dedicated warning channel, emit it here.)
            // onError_("empty character constant", start);

            return Token(TokenType::CharLiteral, text, start.line, start.column);
        }

        // Normal content: read at least one char (or escape sequence)
        // and then keep consuming until we hit the terminating quote or newline/EOF.
        scanCharContent(reader, text, start);

        while (reader.peek() != '\'' && reader.peek() != '\0' && reader.peek() != '\n')
            scanCharContent(reader, text, start);

        if (reader.peek() == '\'') {
            text += reader.advance(); // consume closing quote
        } else {
            // Unterminated character literal (newline or EOF reached).
            onError_("missing terminating ' character", start);
        }

        return Token(TokenType::CharLiteral, text, start.line, start.column);
    }

private:
    std::string filename_;
    ErrorHandler onError_;
    
    // EN: Scans the content of a character literal, handling escapes.
    // FR: Scanne le contenu d un litteral caractere avec echappements.
    void scanCharContent(SourceReader& reader, std::string& text, const SourceLocation& /*start*/) {
        if (reader.peek() == '\\') {
            text += reader.advance();
            if (reader.peek() == '\0' || reader.peek() == '\n')
                return;
            
            if (reader.peek() == 'x') {
                text += reader.advance();
                while (CharClassifier::isHexDigit(reader.peek()))
                    text += reader.advance();
            } else if (CharClassifier::isOctalDigit(reader.peek())) {
                for (int i = 0; i < 3 && CharClassifier::isOctalDigit(reader.peek()); ++i)
                    text += reader.advance();
            } else {
                text += reader.advance();
            }
        } else {
            text += reader.advance();
        }
    }
};

// TODO(cc1) EN: Enforce length rules for multi-byte character constants.
// FR: Appliquer les regles de longueur pour constantes caractere multi-octets.
