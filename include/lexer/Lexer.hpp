#pragma once

#include <string>
#include <vector>
#include <memory>
#include <lexer/Token.hpp>
#include <lexer/SourceReader.hpp>
#include <lexer/SourceExtractor.hpp>
#include <lexer/IScanner.hpp>
#include <utils/SourceLocation.hpp>

class Lexer {
public:
    Lexer(const std::string& source, const std::string& filename = "");
    
    std::vector<Token> tokenize();

private:
    enum ScannerType {
        IDENTIFIER = 0,
        NUMBER,
        STRING,
        CHAR,
        SCANNER_COUNT
    };
    
    std::string filename_;
    std::unique_ptr<SourceReader> reader_;
    std::unique_ptr<SourceExtractor> extractor_;
    std::unique_ptr<IScanner> scanners_[SCANNER_COUNT];
    
    void skipWhitespaceAndComments();
    void skipBlockComment();
    Token scanToken();
    
    [[noreturn]] void error(const std::string& msg);
    [[noreturn]] void errorAt(const std::string& msg, const SourceLocation& loc);
};
