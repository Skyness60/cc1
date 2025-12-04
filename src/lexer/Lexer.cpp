#include <lexer/Lexer.hpp>
#include <unordered_map>
#include <iostream>

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::Identifier: return "Identifier";
        case TokenType::IntegerLiteral: return "IntegerLiteral";
        case TokenType::FloatLiteral: return "FloatLiteral";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::CharLiteral: return "CharLiteral";
        case TokenType::Auto: return "Auto";
        case TokenType::Break: return "Break";
        case TokenType::Case: return "Case";
        case TokenType::Char: return "Char";
        case TokenType::Const: return "Const";
        case TokenType::Continue: return "Continue";
        case TokenType::Default: return "Default";
        case TokenType::Do: return "Do";
        case TokenType::Double: return "Double";
        case TokenType::Else: return "Else";
        case TokenType::Enum: return "Enum";
        case TokenType::Extern: return "Extern";
        case TokenType::Float: return "Float";
        case TokenType::For: return "For";
        case TokenType::Goto: return "Goto";
        case TokenType::If: return "If";
        case TokenType::Int: return "Int";
        case TokenType::Long: return "Long";
        case TokenType::Register: return "Register";
        case TokenType::Return: return "Return";
        case TokenType::Short: return "Short";
        case TokenType::Signed: return "Signed";
        case TokenType::Sizeof: return "Sizeof";
        case TokenType::Static: return "Static";
        case TokenType::Struct: return "Struct";
        case TokenType::Switch: return "Switch";
        case TokenType::Typedef: return "Typedef";
        case TokenType::Union: return "Union";
        case TokenType::Unsigned: return "Unsigned";
        case TokenType::Void: return "Void";
        case TokenType::Volatile: return "Volatile";
        case TokenType::While: return "While";
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Star: return "Star";
        case TokenType::Slash: return "Slash";
        case TokenType::Percent: return "Percent";
        case TokenType::PlusPlus: return "PlusPlus";
        case TokenType::MinusMinus: return "MinusMinus";
        case TokenType::Equal: return "Equal";
        case TokenType::EqualEqual: return "EqualEqual";
        case TokenType::NotEqual: return "NotEqual";
        case TokenType::Less: return "Less";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::Greater: return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::Ampersand: return "Ampersand";
        case TokenType::Pipe: return "Pipe";
        case TokenType::Caret: return "Caret";
        case TokenType::Tilde: return "Tilde";
        case TokenType::Exclamation: return "Exclamation";
        case TokenType::AmpersandAmpersand: return "AmpersandAmpersand";
        case TokenType::PipePipe: return "PipePipe";
        case TokenType::LeftShift: return "LeftShift";
        case TokenType::RightShift: return "RightShift";
        case TokenType::PlusEqual: return "PlusEqual";
        case TokenType::MinusEqual: return "MinusEqual";
        case TokenType::StarEqual: return "StarEqual";
        case TokenType::SlashEqual: return "SlashEqual";
        case TokenType::PercentEqual: return "PercentEqual";
        case TokenType::AmpersandEqual: return "AmpersandEqual";
        case TokenType::PipeEqual: return "PipeEqual";
        case TokenType::CaretEqual: return "CaretEqual";
        case TokenType::LeftShiftEqual: return "LeftShiftEqual";
        case TokenType::RightShiftEqual: return "RightShiftEqual";
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";
        case TokenType::LeftBracket: return "LeftBracket";
        case TokenType::RightBracket: return "RightBracket";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Comma: return "Comma";
        case TokenType::Dot: return "Dot";
        case TokenType::Arrow: return "Arrow";
        case TokenType::Colon: return "Colon";
        case TokenType::Question: return "Question";
        case TokenType::Hash: return "Hash";
        case TokenType::HashHash: return "HashHash";
        case TokenType::Ellipsis: return "Ellipsis";
        default: return "Unknown";
    }
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(" << tokenTypeToString(token.type) << ", \"" << token.value << "\", " << token.line << ":" << token.column << ")";
    return os;
}

Lexer::Lexer(const std::string& source, const std::string& filename)
    : source_(source), filename_(filename) {}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos_ < source_.length()) {
        skipWhitespace();
        if (pos_ >= source_.length()) break;
        
        Token token = scanToken();
        if (token.type != TokenType::EndOfFile) {
            tokens.push_back(token);
        }
    }
    tokens.emplace_back(TokenType::EndOfFile, "", line_, column_);
    return tokens;
}

char Lexer::peek() const {
    if (pos_ >= source_.length()) return '\0';
    return source_[pos_];
}

char Lexer::peekNext() const {
    if (pos_ + 1 >= source_.length()) return '\0';
    return source_[pos_ + 1];
}

char Lexer::advance() {
    if (pos_ >= source_.length()) return '\0';
    char c = source_[pos_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

std::string Lexer::getCurrentLine() const {
    // Find start of current line
    size_t lineStart = pos_;
    while (lineStart > 0 && source_[lineStart - 1] != '\n') {
        lineStart--;
    }
    // Find end of current line
    size_t lineEnd = pos_;
    while (lineEnd < source_.length() && source_[lineEnd] != '\n') {
        lineEnd++;
    }
    return source_.substr(lineStart, lineEnd - lineStart);
}

std::string Lexer::getLineAt(int targetLine) const {
    size_t lineStart = 0;
    int currentLine = 1;
    
    // Find start of target line
    while (currentLine < targetLine && lineStart < source_.length()) {
        if (source_[lineStart] == '\n') {
            currentLine++;
        }
        lineStart++;
    }
    
    // Find end of target line
    size_t lineEnd = lineStart;
    while (lineEnd < source_.length() && source_[lineEnd] != '\n') {
        lineEnd++;
    }
    
    return source_.substr(lineStart, lineEnd - lineStart);
}

void Lexer::error(const std::string& message) {
    throw LexerError(message, filename_, line_, column_, getCurrentLine());
}

void Lexer::errorAt(const std::string& message, int line, int column) {
    throw LexerError(message, filename_, line, column, getLineAt(line));
}

void Lexer::skipWhitespace() {
    while (true) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '/') {
            if (peekNext() == '/') {
                // Single line comment
                while (peek() != '\n' && peek() != '\0') advance();
            } else if (peekNext() == '*') {
                // Multi line comment
                int commentLine = line_;
                int commentCol = column_;
                std::string commentSourceLine = getCurrentLine();
                advance(); advance(); // consume /*
                bool closed = false;
                while (peek() != '\0') {
                    if (peek() == '*' && peekNext() == '/') {
                        advance(); advance(); // consume */
                        closed = true;
                        break;
                    }
                    advance();
                }
                if (!closed) {
                    throw LexerError("unterminated /* comment", filename_, commentLine, commentCol, commentSourceLine);
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isHexDigit(char c) const {
    return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool Lexer::isOctalDigit(char c) const {
    return c >= '0' && c <= '7';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

Token Lexer::scanToken() {
    int startLine = line_;
    int startCol = column_;
    char c = advance();

    switch (c) {
        case '(': return Token(TokenType::LeftParen, "(", startLine, startCol);
        case ')': return Token(TokenType::RightParen, ")", startLine, startCol);
        case '{': return Token(TokenType::LeftBrace, "{", startLine, startCol);
        case '}': return Token(TokenType::RightBrace, "}", startLine, startCol);
        case '[': return Token(TokenType::LeftBracket, "[", startLine, startCol);
        case ']': return Token(TokenType::RightBracket, "]", startLine, startCol);
        case ';': return Token(TokenType::Semicolon, ";", startLine, startCol);
        case ',': return Token(TokenType::Comma, ",", startLine, startCol);
        case ':': return Token(TokenType::Colon, ":", startLine, startCol);
        case '?': return Token(TokenType::Question, "?", startLine, startCol);
        case '~': return Token(TokenType::Tilde, "~", startLine, startCol);
        case '#': 
            if (match('#')) return Token(TokenType::HashHash, "##", startLine, startCol);
            return Token(TokenType::Hash, "#", startLine, startCol);

        case '.':
            if (peek() == '.' && peekNext() == '.') {
                advance(); advance();
                return Token(TokenType::Ellipsis, "...", startLine, startCol);
            }
            if (isDigit(peek())) {
                // Float starting with .
                pos_--; column_--; // go back
                return number();
            }
            return Token(TokenType::Dot, ".", startLine, startCol);
        
        case '!':
            if (match('=')) return Token(TokenType::NotEqual, "!=", startLine, startCol);
            return Token(TokenType::Exclamation, "!", startLine, startCol);
        case '=':
            if (match('=')) return Token(TokenType::EqualEqual, "==", startLine, startCol);
            return Token(TokenType::Equal, "=", startLine, startCol);
        case '<': 
            if (match('=')) return Token(TokenType::LessEqual, "<=", startLine, startCol);
            if (match('<')) {
                if (match('=')) return Token(TokenType::LeftShiftEqual, "<<=", startLine, startCol);
                return Token(TokenType::LeftShift, "<<", startLine, startCol);
            }
            return Token(TokenType::Less, "<", startLine, startCol);
        case '>':
            if (match('=')) return Token(TokenType::GreaterEqual, ">=", startLine, startCol);
            if (match('>')) {
                if (match('=')) return Token(TokenType::RightShiftEqual, ">>=", startLine, startCol);
                return Token(TokenType::RightShift, ">>", startLine, startCol);
            }
            return Token(TokenType::Greater, ">", startLine, startCol);
        case '+':
            if (match('+')) return Token(TokenType::PlusPlus, "++", startLine, startCol);
            if (match('=')) return Token(TokenType::PlusEqual, "+=", startLine, startCol);
            return Token(TokenType::Plus, "+", startLine, startCol);
        case '-':
            if (match('-')) return Token(TokenType::MinusMinus, "--", startLine, startCol);
            if (match('=')) return Token(TokenType::MinusEqual, "-=", startLine, startCol);
            if (match('>')) return Token(TokenType::Arrow, "->", startLine, startCol);
            return Token(TokenType::Minus, "-", startLine, startCol);
        case '*':
            if (match('=')) return Token(TokenType::StarEqual, "*=", startLine, startCol);
            return Token(TokenType::Star, "*", startLine, startCol);
        case '/':
            if (match('=')) return Token(TokenType::SlashEqual, "/=", startLine, startCol);
            return Token(TokenType::Slash, "/", startLine, startCol);
        case '%':
            if (match('=')) return Token(TokenType::PercentEqual, "%=", startLine, startCol);
            return Token(TokenType::Percent, "%", startLine, startCol);
        case '&':
            if (match('&')) return Token(TokenType::AmpersandAmpersand, "&&", startLine, startCol);
            if (match('=')) return Token(TokenType::AmpersandEqual, "&=", startLine, startCol);
            return Token(TokenType::Ampersand, "&", startLine, startCol);
        case '|':
            if (match('|')) return Token(TokenType::PipePipe, "||", startLine, startCol);
            if (match('=')) return Token(TokenType::PipeEqual, "|=", startLine, startCol);
            return Token(TokenType::Pipe, "|", startLine, startCol);
        case '^':
            if (match('=')) return Token(TokenType::CaretEqual, "^=", startLine, startCol);
            return Token(TokenType::Caret, "^", startLine, startCol);
        
        case '"': return string();
        case '\'': return character();
        
        default:
            if (isDigit(c)) {
                pos_--; column_--; // Go back to re-read the digit
                return number();
            } else if (isAlpha(c)) {
                pos_--; column_--; // Go back to re-read the character
                return identifier();
            }
            // Unknown character - use clang-style message
            // Use startCol (before advance) for correct caret position
            errorAt("expected identifier or '('", startLine, startCol);
    }
    
    return Token(TokenType::EndOfFile, "", line_, column_);
}

Token Lexer::identifier() {
    int startLine = line_;
    int startCol = column_;
    std::string text;
    
    while (isAlphaNumeric(peek())) {
        text += advance();
    }
    
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"auto", TokenType::Auto}, {"break", TokenType::Break}, {"case", TokenType::Case},
        {"char", TokenType::Char}, {"const", TokenType::Const}, {"continue", TokenType::Continue},
        {"default", TokenType::Default}, {"do", TokenType::Do}, {"double", TokenType::Double},
        {"else", TokenType::Else}, {"enum", TokenType::Enum}, {"extern", TokenType::Extern},
        {"float", TokenType::Float}, {"for", TokenType::For}, {"goto", TokenType::Goto},
        {"if", TokenType::If}, {"int", TokenType::Int}, {"long", TokenType::Long},
        {"register", TokenType::Register}, {"return", TokenType::Return}, {"short", TokenType::Short},
        {"signed", TokenType::Signed}, {"sizeof", TokenType::Sizeof}, {"static", TokenType::Static},
        {"struct", TokenType::Struct}, {"switch", TokenType::Switch}, {"typedef", TokenType::Typedef},
        {"union", TokenType::Union}, {"unsigned", TokenType::Unsigned}, {"void", TokenType::Void},
        {"volatile", TokenType::Volatile}, {"while", TokenType::While}
    };
    
    auto it = keywords.find(text);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::Identifier;
    
    return Token(type, text, startLine, startCol);
}

Token Lexer::number() {
    int startLine = line_;
    int startCol = column_;
    std::string text;
    
    bool isFloat = false;
    char first = peek();

    // Check for hex (0x) or octal (0...)
    if (first == '0' && (peekNext() == 'x' || peekNext() == 'X')) {
        // Hexadecimal
        text += advance(); // '0'
        text += advance(); // 'x' or 'X'
        
        if (!isHexDigit(peek())) {
            error("invalid hexadecimal constant");
        }
        
        while (isHexDigit(peek())) {
            text += advance();
        }
        
        // Check for invalid suffix (letter that's not a valid hex digit or suffix)
        if (isAlpha(peek()) && !isHexDigit(peek())) {
            // Allow valid integer suffixes: u, U, l, L, ul, UL, etc.
            char suffix = peek();
            if (suffix != 'u' && suffix != 'U' && suffix != 'l' && suffix != 'L') {
                error("invalid suffix on integer constant");
            }
            while (peek() == 'u' || peek() == 'U' || peek() == 'l' || peek() == 'L') {
                text += advance();
            }
            if (isAlphaNumeric(peek())) {
                error("invalid suffix on integer constant");
            }
        }
        
        return Token(TokenType::IntegerLiteral, text, startLine, startCol);
    } else if (first == '0' && isOctalDigit(peekNext())) {
        // Octal
        text += advance(); // '0'
        
        while (isOctalDigit(peek())) {
            text += advance();
        }
        
        // Check for invalid octal digit (8 or 9)
        if (isDigit(peek())) {
            error("invalid digit in octal constant");
        }
        
        // Check for invalid suffix
        if (isAlpha(peek())) {
            char suffix = peek();
            if (suffix != 'u' && suffix != 'U' && suffix != 'l' && suffix != 'L') {
                error("invalid suffix on integer constant");
            }
            while (peek() == 'u' || peek() == 'U' || peek() == 'l' || peek() == 'L') {
                text += advance();
            }
            if (isAlphaNumeric(peek())) {
                error("invalid suffix on integer constant");
            }
        }
        
        return Token(TokenType::IntegerLiteral, text, startLine, startCol);
    }
    
    // Decimal or float
    while (isDigit(peek())) {
        text += advance();
    }
    
    // Check for invalid character after digits (like 123a1)
    if (isAlpha(peek()) && peek() != 'e' && peek() != 'E' && 
        peek() != 'f' && peek() != 'F' && peek() != 'l' && peek() != 'L' &&
        peek() != 'u' && peek() != 'U') {
        char invalidChar = peek();
        error(std::string("invalid digit '") + invalidChar + "' in decimal constant");
    }
    
    // Float with decimal point
    if (peek() == '.' && (isDigit(peekNext()) || peekNext() == 'e' || peekNext() == 'E' || !isAlpha(peekNext()))) {
        isFloat = true;
        text += advance(); // Consume .
        while (isDigit(peek())) {
            text += advance();
        }
    }
    
    // Exponent
    if (peek() == 'e' || peek() == 'E') {
        isFloat = true;
        text += advance();
        if (peek() == '+' || peek() == '-') {
            text += advance();
        }
        if (!isDigit(peek())) {
            error("exponent has no digits");
        }
        while (isDigit(peek())) {
            text += advance();
        }
    }
    
    // Float suffix
    if (isFloat && (peek() == 'f' || peek() == 'F' || peek() == 'l' || peek() == 'L')) {
        text += advance();
    }
    
    // Integer suffix
    if (!isFloat) {
        while (peek() == 'u' || peek() == 'U' || peek() == 'l' || peek() == 'L') {
            text += advance();
        }
    }
    
    // Final check for invalid characters
    if (isAlphaNumeric(peek())) {
        error("invalid suffix on " + std::string(isFloat ? "floating" : "integer") + " constant");
    }
    
    return Token(isFloat ? TokenType::FloatLiteral : TokenType::IntegerLiteral, text, startLine, startCol);
}

Token Lexer::string() {
    int startLine = line_;
    int startCol = column_ - 1; // -1 because we already consumed the opening quote
    std::string text;
    text += '"';
    
    while (peek() != '"' && peek() != '\0' && peek() != '\n') {
        if (peek() == '\\') {
            text += advance(); // Consume backslash
            if (peek() == '\0' || peek() == '\n') {
                // Allow line continuation in strings
                if (peek() == '\n') {
                    text += advance();
                    continue;
                }
                errorAt("missing terminating '\"' character", startLine, startCol);
            }
            text += advance(); // Consume escape character
        } else {
            text += advance();
        }
    }
    
    if (peek() != '"') {
        errorAt("missing terminating '\"' character", startLine, startCol);
    }
    
    text += advance(); // Consume closing quote
    
    return Token(TokenType::StringLiteral, text, startLine, startCol);
}

Token Lexer::character() {
    int startLine = line_;
    int startCol = column_ - 1; // -1 because we already consumed the opening quote
    std::string text;
    text += '\'';
    
    if (peek() == '\'') {
        errorAt("empty character constant", startLine, startCol);
    }
    
    if (peek() == '\0' || peek() == '\n') {
        errorAt("missing terminating ' character", startLine, startCol);
    }
    
    if (peek() == '\\') {
        text += advance(); // Consume backslash
        if (peek() == '\0' || peek() == '\n') {
            errorAt("missing terminating ' character", startLine, startCol);
        }
        // Handle escape sequences
        char escapeChar = peek();
        if (escapeChar == 'x') {
            // Hex escape
            text += advance();
            if (!isHexDigit(peek())) {
                error("\\x used with no following hex digits");
            }
            while (isHexDigit(peek())) {
                text += advance();
            }
        } else if (isOctalDigit(escapeChar)) {
            // Octal escape (up to 3 digits)
            for (int i = 0; i < 3 && isOctalDigit(peek()); i++) {
                text += advance();
            }
        } else {
            // Simple escape
            text += advance();
        }
    } else {
        text += advance();
    }
    
    // Multi-character constants are allowed in C (but usually a warning)
    while (peek() != '\'' && peek() != '\0' && peek() != '\n') {
        if (peek() == '\\') {
            text += advance();
            if (peek() == '\0' || peek() == '\n') {
                errorAt("missing terminating ' character", startLine, startCol);
            }
            text += advance();
        } else {
            text += advance();
        }
    }
    
    if (peek() != '\'') {
        errorAt("missing terminating ' character", startLine, startCol);
    }
    
    text += advance(); // Consume closing quote
    
    return Token(TokenType::CharLiteral, text, startLine, startCol);
}
