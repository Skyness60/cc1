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
        SourceLocation start(filename_, reader.line(), reader.column() - 1);
        std::string text = "'";
        
        if (reader.peek() == '\'')
            onError_("empty character constant", start);
        
        if (reader.peek() == '\0' || reader.peek() == '\n')
            onError_("missing terminating ' character", start);
        
        scanCharContent(reader, text, start);
        
        
        while (reader.peek() != '\'' && reader.peek() != '\0' && reader.peek() != '\n')
            scanCharContent(reader, text, start);
        
        if (reader.peek() != '\'')
            onError_("missing terminating ' character", start);
        
        text += reader.advance();
        return Token(TokenType::CharLiteral, text, start.line, start.column);
    }

private:
    std::string filename_;
    ErrorHandler onError_;
    
    // EN: Scans the content of a character literal, handling escapes.
    // FR: Scanne le contenu d un litteral caractere avec echappements.
    void scanCharContent(SourceReader& reader, std::string& text, const SourceLocation& start) {
        if (reader.peek() == '\\') {
            text += reader.advance();
            if (reader.peek() == '\0' || reader.peek() == '\n')
                onError_("missing terminating ' character", start);
            
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
