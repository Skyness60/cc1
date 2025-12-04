#pragma once

#include <string>

class SourceReader {
public:
    SourceReader(const std::string& source);
    
    // Character reading
    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);
    bool isAtEnd() const;
    
    // Position management
    int line() const { return line_; }
    int column() const { return column_; }
    size_t position() const { return pos_; }
    
    // Backtracking
    void retreat();
    
private:
    std::string source_;
    size_t pos_ = 0;
    int line_ = 1;
    int column_ = 1;
};
