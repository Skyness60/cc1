#pragma once



#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <functional>

// EN: Scans octal integer literals (0...).
// FR: Scanne les litteraux entiers octaux (0...).
class OctalScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&)>;
    
    // EN: Initializes with an error handler for invalid literals.
    // FR: Initialise avec un handler d erreur pour litteraux invalides.
    explicit OctalScanner(ErrorHandler onError) : onError_(std::move(onError)) {}
    
    // EN: Not used directly; octal scanning is dispatched by NumberScanner.
    // FR: Non utilise directement ; le scan octal est delegue par NumberScanner.
    bool canScan(char ) const override { 
        return false; 
    }
    
    // EN: Scans an octal literal and returns an integer token.
    // FR: Scanne un litteral octal et renvoie un token entier.
    Token scan(SourceReader& reader) override {
        int line = reader.line();
        int col = reader.column();
        std::string text;
        
        text += reader.advance(); 
        
        while (CharClassifier::isOctalDigit(reader.peek()))
            text += reader.advance();
        
        if (CharClassifier::isDigit(reader.peek()))
            onError_("invalid digit in octal constant");
        
        scanSuffix(text, reader);
        return Token(TokenType::IntegerLiteral, text, line, col);
    }

private:
    ErrorHandler onError_;
    
    // EN: Scans integer suffixes and appends them to the literal.
    // FR: Scanne les suffixes entiers et les ajoute au litteral.
    void scanSuffix(std::string& text, SourceReader& reader) {
        while (reader.peek() == 'u' || reader.peek() == 'U' || 
               reader.peek() == 'l' || reader.peek() == 'L')
            text += reader.advance();
    }
};

// TODO(cc1) EN: Support digit separators if the language mode allows it.
// FR: Supporter les separateurs de digits si le mode le permet.
