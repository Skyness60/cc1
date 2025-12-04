#pragma once

#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <functional>

class OctalScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&)>;
    
    explicit OctalScanner(ErrorHandler onError) : onError_(std::move(onError)) {}
    
    bool canScan(char /*c*/) const override { 
        return false; // Called explicitly, not via canScan
    }
    
    Token scan(SourceReader& reader) override {
        int line = reader.line();
        int col = reader.column();
        std::string text;
        
        text += reader.advance(); // '0'
        
        while (CharClassifier::isOctalDigit(reader.peek()))
            text += reader.advance();
        
        if (CharClassifier::isDigit(reader.peek()))
            onError_("invalid digit in octal constant");
        
        scanSuffix(text, reader);
        return Token(TokenType::IntegerLiteral, text, line, col);
    }

private:
    ErrorHandler onError_;
    
    void scanSuffix(std::string& text, SourceReader& reader) {
        while (reader.peek() == 'u' || reader.peek() == 'U' || 
               reader.peek() == 'l' || reader.peek() == 'L')
            text += reader.advance();
    }
};
