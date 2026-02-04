#include <lexer/Lexer.hpp>
#include <lexer/LexerError.hpp>
#include <lexer/CharClassifier.hpp>
#include <lexer/scanners/IdentifierScanner.hpp>
#include <lexer/scanners/NumberScanner.hpp>
#include <lexer/scanners/StringScanner.hpp>
#include <lexer/scanners/CharScanner.hpp>
#include <lexer/scanners/OperatorScanner.hpp>

#include <utils/Diagnostic.hpp>

// EN: Builds lexer components and scanner set for the given source.
// FR: Construit les composants du lexer et les scanners pour la source.
Lexer::Lexer(const std::string& source, const std::string& filename)
    : filename_(filename),
      extractor_source_(source),
      reader_(new SourceReader(source)),
      extractor_(new SourceExtractor(extractor_source_)) {
    
    scanners_[IDENTIFIER].reset(new IdentifierScanner());
    scanners_[NUMBER].reset(new NumberScanner(filename_, 
        [this](const std::string& msg) { error(msg); }));
    scanners_[STRING].reset(new StringScanner(filename_,
        [this](const std::string& msg, const SourceLocation& loc) { errorAt(msg, loc); }));
    scanners_[CHAR].reset(new CharScanner(filename_,
        [this](const std::string& msg, const SourceLocation& loc) { errorAt(msg, loc); }));
}

// EN: Tokenizes the entire input and appends EOF token.
// FR: Tokenise toute l entree et ajoute le token EOF.
std::vector<Token> Lexer::tokenize() {
    if (!display_source_override_.empty()) {
        extractor_.reset(new SourceExtractor(display_source_override_));
    }

    std::vector<Token> tokens;

    // We want clang-like behavior: emit a diagnostic for the bad token,
    // then continue lexing so the parser can issue follow-up errors.
    while (!reader_->isAtEnd()) {
        try {
            Token tok = scanToken();
            if (tok.type != TokenType::EndOfFile)
                tokens.push_back(tok);
        } catch (const LexerError& e) {
            // Print the formatted lexer diagnostic (already includes source + caret).
            std::cerr << e.what() << std::endl;

            // Best-effort recovery: advance at least one character to avoid infinite loops.
            if (!reader_->isAtEnd()) {
                reader_->advance();
            }
            // Continue scanning.
        }
    }

    tokens.push_back(Token(TokenType::EndOfFile, "", reader_->line(), reader_->column()));
    return tokens;
}

// EN: Skips whitespace and both line/block comments.
// FR: Saute les espaces et commentaires ligne/bloc.
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

// EN: Consumes a block comment, reporting an error if unterminated.
// FR: Consomme un commentaire bloc et signale une erreur s il n est pas termine.
void Lexer::skipBlockComment() {
    SourceLocation start(filename_, reader_->line(), reader_->column());
    reader_->advance(); 
    reader_->advance(); 
    
    while (!reader_->isAtEnd()) {
        if (reader_->peek() == '*' && reader_->peekNext() == '/') {
            reader_->advance(); 
            reader_->advance(); 
            return;
        }
        reader_->advance();
    }
    errorAt("unterminated /* comment", start);
}

// EN: Scans the next token by dispatching to the correct scanner.
// FR: Scanne le prochain token en dispatchant vers le bon scanner.
Token Lexer::scanToken() {
    skipWhitespaceAndComments();
    if (reader_->isAtEnd()) return Token(TokenType::EndOfFile, "", reader_->line(), reader_->column());
    
    int line = reader_->line(), col = reader_->column();
    char c = reader_->advance();
    
    
    if (c == '"') return scanners_[STRING]->scan(*reader_);
    if (c == '\'') return scanners_[CHAR]->scan(*reader_);
    
    
    if (scanners_[IDENTIFIER]->canScan(c)) {
        reader_->retreat();
        return scanners_[IDENTIFIER]->scan(*reader_);
    }
    
    
    if (scanners_[NUMBER]->canScan(c)) {
        reader_->retreat();
        return scanners_[NUMBER]->scan(*reader_);
    }
    
    
    Token tok(TokenType::EndOfFile, "", line, col);
    if (OperatorScanner::scan(c, *reader_, line, col, tok)) return tok;
    
    errorAt("expected identifier or '('", SourceLocation(filename_, line, col));
    return Token(TokenType::EndOfFile, "", line, col); 
}

// EN: Throws a lexer error at the current reader location.
// FR: Lance une erreur de lexer a la position courante.
void Lexer::error(const std::string& msg) {
    throw LexerError(msg, SourceLocation(filename_, reader_->line(), reader_->column()), extractor_->getLine(reader_->line()));
}

// EN: Throws a lexer error at a specific location.
// FR: Lance une erreur de lexer a une position precise.
void Lexer::errorAt(const std::string& msg, const SourceLocation& loc) {
    throw LexerError(msg, loc, extractor_->getLine(loc.line));
}

// TODO(cc1) EN: Improve error recovery to continue tokenization after errors.
// FR: Ameliorer la recuperation d erreurs pour continuer la tokenisation.
