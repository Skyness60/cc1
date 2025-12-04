#pragma once

#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <utils/SourceLocation.hpp>
#include <functional>

class StringScanner final : public IScanner {
public:
    using ErrorHandler = std::function<void(const std::string&, const SourceLocation&)>;
    
    StringScanner(const std::string& filename, ErrorHandler onError)
        : filename_(filename), onError_(std::move(onError)) {}
    
    bool canScan(char c) const override { return c == '"'; }
    
    Token scan(SourceReader& reader) override {
        SourceLocation start(filename_, reader.line(), reader.column() - 1);
        std::string text = "\"";
        
        while (reader.peek() != '"' && reader.peek() != '\0' && reader.peek() != '\n') {
            if (reader.peek() == '\\') {
                text += reader.advance();
                if (reader.peek() == '\n') { text += reader.advance(); continue; }
                if (reader.peek() == '\0') onError_("missing terminating '\"' character", start);
                text += reader.advance();
            } else {
                text += reader.advance();
            }
        }
        
        if (reader.peek() != '"')
            onError_("missing terminating '\"' character", start);
        
        text += reader.advance();
        return Token(TokenType::StringLiteral, text, start.line, start.column);
    }

private:
    std::string filename_;
    ErrorHandler onError_;
};
