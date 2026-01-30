#pragma once



#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <functional>

// EN: Scans decimal integer and floating literals.
// FR: Scanne les litteraux decimaux entiers et flottants.
class DecimalScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&)>;
    
    // EN: Initializes with an error handler for invalid literals.
    // FR: Initialise avec un handler d erreur pour litteraux invalides.
    explicit DecimalScanner(ErrorHandler onError) : onError_(std::move(onError)) {}
    
    // EN: Starts when the first character is a digit.
    // FR: Demarre si le premier caractere est un chiffre.
    bool canScan(char c) const override { 
        return CharClassifier::isDigit(c);
    }
    
    // EN: Scans a decimal literal and classifies as int or float.
    // FR: Scanne un litteral decimal et classe en int ou float.
    Token scan(SourceReader& reader) override {
        int line = reader.line();
        int col = reader.column();
        std::string text;
        bool isFloat = false;
        
        while (CharClassifier::isDigit(reader.peek()))
            text += reader.advance();
        
        checkInvalidDigit(reader);
        
        if (reader.peek() == '.' && !CharClassifier::isAlpha(reader.peekNext())) {
            isFloat = true;
            text += reader.advance();
            while (CharClassifier::isDigit(reader.peek()))
                text += reader.advance();
        }
        
        if (reader.peek() == 'e' || reader.peek() == 'E') {
            isFloat = true;
            scanExponent(text, reader);
        }
        
        scanSuffix(text, reader, isFloat);
        
        return Token(isFloat ? TokenType::FloatLiteral : TokenType::IntegerLiteral, text, line, col);
    }

private:
    ErrorHandler onError_;
    
    // EN: Detects invalid trailing characters in decimal literals.
    // FR: Detecte les caracteres invalides apres un litteral decimal.
    void checkInvalidDigit(SourceReader& reader) {
        char c = reader.peek();
        if (CharClassifier::isAlpha(c) && 
            c != 'e' && c != 'E' && c != 'f' && c != 'F' && 
            c != 'l' && c != 'L' && c != 'u' && c != 'U') {
            onError_("invalid digit '" + std::string(1, c) + "' in decimal constant");
        }
    }
    
    // EN: Scans exponent part for floating literals.
    // FR: Scanne l exposant pour les flottants.
    void scanExponent(std::string& text, SourceReader& reader) {
        text += reader.advance(); 
        if (reader.peek() == '+' || reader.peek() == '-')
            text += reader.advance();
        if (!CharClassifier::isDigit(reader.peek()))
            onError_("exponent has no digits");
        while (CharClassifier::isDigit(reader.peek()))
            text += reader.advance();
    }
    
    // EN: Scans integer/float suffixes and appends them to the literal.
    // FR: Scanne les suffixes entiers/flottants et les ajoute au litteral.
    void scanSuffix(std::string& text, SourceReader& reader, bool isFloat) {
        if (isFloat) {
            if (reader.peek() == 'f' || reader.peek() == 'F' || 
                reader.peek() == 'l' || reader.peek() == 'L')
                text += reader.advance();
        } else {
            while (reader.peek() == 'u' || reader.peek() == 'U' || 
                   reader.peek() == 'l' || reader.peek() == 'L')
                text += reader.advance();
        }
    }
};

// TODO(cc1) EN: Validate combinations of suffixes for stricter diagnostics.
// FR: Valider les combinaisons de suffixes pour diagnostics plus stricts.
