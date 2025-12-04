#pragma once

#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <lexer/KeywordMatcher.hpp>

class IdentifierScanner final : public IScanner {
    public:
        bool canScan(char c) const override { return CharClassifier::isAlpha(c); }
        
        Token scan(SourceReader& reader) override {
            int line = reader.line();
            int col = reader.column();
            std::string text;
            
            while (CharClassifier::isAlphaNumeric(reader.peek()))
                text += reader.advance();
            
            return Token(KeywordMatcher::match(text), text, line, col);
        }
};
