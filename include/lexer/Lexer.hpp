#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <lexer/Token.hpp>
#include <utils/color.hpp>

class LexerError : public std::runtime_error {
public:
    LexerError(const std::string& message, const std::string& filename, int line, int column,
               const std::string& sourceLine = "")
        : std::runtime_error(formatError(message, filename, line, column, sourceLine)),
          filename_(filename), line_(line), column_(column), message_(message) {}

    const std::string& getFilename() const { return filename_; }
    int getLine() const { return line_; }
    int getColumn() const { return column_; }
    const std::string& getMessage() const { return message_; }

private:
    std::string filename_;
    int line_;
    int column_;
    std::string message_;

    static std::string formatError(const std::string& message, const std::string& filename,
                                   int line, int column, const std::string& sourceLine) {
        // Format like clang with colors:
        // file:line:col: error: message
        std::string result = WHITE + filename + ":" + std::to_string(line) + ":" + std::to_string(column) + ": " + RESET;
        result += RED "error: " RESET;
        result += WHITE + message + RESET "\n";
        
        if (!sourceLine.empty()) {
            // Add line number prefix like clang (4 spaces + line number + " | ")
            std::string lineNumStr = std::to_string(line);
            result += "    " + lineNumStr + " | " + sourceLine + "\n";
            // Add caret with green color (4 spaces + padding for line number + " | " + spaces to column + "^")
            result += "    " + std::string(lineNumStr.length(), ' ') + " | ";
            result += GREEN + std::string(column > 0 ? column - 1 : 0, ' ') + "^" + RESET;
        }
        return result;
    }
};

class Lexer {
public:
    Lexer(const std::string& source, const std::string& filename = "");
    
    std::vector<Token> tokenize();

private:
    std::string source_;
    std::string filename_;
    size_t pos_ = 0;
    int line_ = 1;
    int column_ = 1;

    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);
    void skipWhitespace();
    
    Token scanToken();
    Token identifier();
    Token number();
    Token string();
    Token character();
    
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isHexDigit(char c) const;
    bool isOctalDigit(char c) const;
    bool isAlphaNumeric(char c) const;

    std::string getCurrentLine() const;
    std::string getLineAt(int targetLine) const;
    [[noreturn]] void error(const std::string& message);
    [[noreturn]] void errorAt(const std::string& message, int line, int column);
};
