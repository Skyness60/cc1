#pragma once

#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <functional>

class DecimalScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&)>;
    
    explicit DecimalScanner(ErrorHandler onError) : onError_(std::move(onError)) {}
    
    bool canScan(char c) const override { 
        return CharClassifier::isDigit(c);
    }
    
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
    
    void checkInvalidDigit(SourceReader& reader) {
        char c = reader.peek();
        if (CharClassifier::isAlpha(c) && 
            c != 'e' && c != 'E' && c != 'f' && c != 'F' && 
            c != 'l' && c != 'L' && c != 'u' && c != 'U') {
            onError_("invalid digit '" + std::string(1, c) + "' in decimal constant");
        }
    }
    
    void scanExponent(std::string& text, SourceReader& reader) {
        text += reader.advance(); // 'e' or 'E'
        if (reader.peek() == '+' || reader.peek() == '-')
            text += reader.advance();
        if (!CharClassifier::isDigit(reader.peek()))
            onError_("exponent has no digits");
        while (CharClassifier::isDigit(reader.peek()))
            text += reader.advance();
    }
    
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
