#pragma once



#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <functional>

// EN: Scans hexadecimal integer literals (0x...).
// FR: Scanne les litteraux entiers hexadecimaux (0x...).
class HexScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&)>;
    
    // EN: Initializes with an error handler for invalid literals.
    // FR: Initialise avec un handler d erreur pour litteraux invalides.
    explicit HexScanner(ErrorHandler onError) : onError_(std::move(onError)) {}
    
    // EN: Not used directly; hex scanning is dispatched by NumberScanner.
    // FR: Non utilise directement ; le scan hexa est delegue par NumberScanner.
    bool canScan(char ) const override { 
        return false; 
    }
    
    // EN: Scans a hex literal and returns an integer token.
    // FR: Scanne un litteral hexa et renvoie un token entier.
    Token scan(SourceReader& reader) override {
        int line = reader.line();
        int col = reader.column();
        std::string text;
        
        text += reader.advance(); 
        text += reader.advance(); 
        
        if (!CharClassifier::isHexDigit(reader.peek()))
            onError_("invalid hexadecimal constant");
        
        while (CharClassifier::isHexDigit(reader.peek()))
            text += reader.advance();
        
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

// TODO(cc1) EN: Support hexadecimal floating literals if required.
// FR: Supporter les flottants hexadecimaux si necessaire.
