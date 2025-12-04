#include <lexer/Lexer.hpp>
#include <lexer/LexerError.hpp>
#include <lexer/CharClassifier.hpp>
#include <lexer/scanners/IdentifierScanner.hpp>
#include <lexer/scanners/NumberScanner.hpp>
#include <lexer/scanners/StringScanner.hpp>
#include <lexer/scanners/CharScanner.hpp>
#include <lexer/scanners/OperatorScanner.hpp>

Lexer::Lexer(const std::string& source, const std::string& filename)
    : filename_(filename),
      reader_(new SourceReader(source)),
      extractor_(new SourceExtractor(source)) {
    
    scanners_[IDENTIFIER].reset(new IdentifierScanner());
    scanners_[NUMBER].reset(new NumberScanner(filename_, 
        [this](const std::string& msg) { error(msg); }));
    scanners_[STRING].reset(new StringScanner(filename_,
        [this](const std::string& msg, const SourceLocation& loc) { errorAt(msg, loc); }));
    scanners_[CHAR].reset(new CharScanner(filename_,
        [this](const std::string& msg, const SourceLocation& loc) { errorAt(msg, loc); }));
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!reader_->isAtEnd()) {
        Token tok = scanToken();
        if (tok.type != TokenType::EndOfFile)
            tokens.push_back(tok);
    }
    tokens.push_back(Token(TokenType::EndOfFile, "", reader_->line(), reader_->column()));
    return tokens;
}

void Lexer::skipWhitespaceAndComments() {
    while (!reader_->isAtEnd()) {
        char c = reader_->peek();
        if (CharClassifier::isWhitespace(c)) {
            reader_->advance();
        } else if (c == '/' && reader_->peekNext() == '/') {
            while (!reader_->isAtEnd() && reader_->peek() != '\n') reader_->advance();
        } else if (c == '/' && reader_->peekNext() == '*') {
            skipBlockComment();
        } else {
            return;
        }
    }
}

void Lexer::skipBlockComment() {
    SourceLocation start(filename_, reader_->line(), reader_->column());
    reader_->advance(); // '/'
    reader_->advance(); // '*'
    
    while (!reader_->isAtEnd()) {
        if (reader_->peek() == '*' && reader_->peekNext() == '/') {
            reader_->advance(); // '*'
            reader_->advance(); // '/'
            return;
        }
        reader_->advance();
    }
    errorAt("unterminated /* comment", start);
}

Token Lexer::scanToken() {
    skipWhitespaceAndComments();
    if (reader_->isAtEnd()) return Token(TokenType::EndOfFile, "", reader_->line(), reader_->column());
    
    int line = reader_->line(), col = reader_->column();
    char c = reader_->advance();
    
    // String and char literals (opening quote already consumed)
    if (c == '"') return scanners_[STRING]->scan(*reader_);
    if (c == '\'') return scanners_[CHAR]->scan(*reader_);
    
    // Identifier or keyword
    if (scanners_[IDENTIFIER]->canScan(c)) {
        reader_->retreat();
        return scanners_[IDENTIFIER]->scan(*reader_);
    }
    
    // Number literal
    if (scanners_[NUMBER]->canScan(c)) {
        reader_->retreat();
        return scanners_[NUMBER]->scan(*reader_);
    }
    
    // Operators and punctuation
    Token tok(TokenType::EndOfFile, "", line, col);
    if (OperatorScanner::scan(c, *reader_, line, col, tok)) return tok;
    
    errorAt("expected identifier or '('", SourceLocation(filename_, line, col));
    return Token(TokenType::EndOfFile, "", line, col); // unreachable
}

void Lexer::error(const std::string& msg) {
    throw LexerError(msg, SourceLocation(filename_, reader_->line(), reader_->column()), extractor_->getLine(reader_->line()));
}

void Lexer::errorAt(const std::string& msg, const SourceLocation& loc) {
    throw LexerError(msg, loc, extractor_->getLine(loc.line));
}
