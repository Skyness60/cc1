#pragma once

#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <utils/SourceLocation.hpp>
#include <functional>

class CharScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&, const SourceLocation&)>;
    
    CharScanner(const std::string& filename, ErrorHandler onError)
        : filename_(filename), onError_(std::move(onError)) {}
    
    bool canScan(char c) const override { return c == '\''; }
    
    Token scan(SourceReader& reader) override {
        SourceLocation start(filename_, reader.line(), reader.column() - 1);
        std::string text = "'";
        
        if (reader.peek() == '\'')
            onError_("empty character constant", start);
        
        if (reader.peek() == '\0' || reader.peek() == '\n')
            onError_("missing terminating ' character", start);
        
        scanCharContent(reader, text, start);
        
        // Multi-char constants
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
