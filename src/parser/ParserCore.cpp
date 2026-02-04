#include <parser/Parser.hpp>
#include <sstream>
#include <iostream>
#include <utils/Diagnostic.hpp>

namespace cc1 {







// EN: Initializes parser state with token stream and source context.
// FR: Initialise l etat du parseur avec tokens et contexte source.
Parser::Parser(const std::vector<Token>& tokens,
               const std::string& filename,
               const std::string& source)
    : tokens_(tokens)
    , filename_(filename)
    , source_(source)
{
}





// EN: Parses the full translation unit, stopping on fatal parse errors.
// FR: Parse l unite de traduction complete et s arrete sur erreur fatale.
AST::Ptr<AST::TranslationUnit> Parser::parse() {
    auto unit = AST::make<AST::TranslationUnit>(1, 1);
    
    while (!isAtEnd()) {
        try {
            auto decl = parseExternalDeclaration();
            if (decl) {
                unit->declarations.push_back(std::move(decl));
            }
        } catch (const ParseError& e) {
            hadError_ = true;
            std::cerr << e.what() << std::endl;
            
            break;
        }
    }
    
    return unit;
}





// EN: Returns the current token, falling back to EOF if out of range.
// FR: Renvoie le token courant, ou EOF si hors limites.
const Token& Parser::current() const {
    if (currentIndex_ >= tokens_.size()) {
        return tokens_.back(); 
    }
    return tokens_[currentIndex_];
}

// EN: Peeks ahead by offset, returning EOF if out of range.
// FR: Regarde en avant d un offset, ou EOF si hors limites.
const Token& Parser::peek(int offset) const {
    size_t index = currentIndex_ + offset;
    if (index >= tokens_.size()) {
        return tokens_.back();
    }
    return tokens_[index];
}

// EN: Returns the previously consumed token.
// FR: Renvoie le token consomme precedent.
const Token& Parser::previous() const {
    if (currentIndex_ == 0) {
        return tokens_[0];
    }
    return tokens_[currentIndex_ - 1];
}

// EN: Checks whether the current token is EOF.
// FR: Verifie si le token courant est EOF.
bool Parser::isAtEnd() const {
    return current().type == TokenType::EndOfFile;
}

// EN: Tests whether the current token matches a type.
// FR: Teste si le token courant correspond a un type.
bool Parser::check(TokenType type) const {
    return current().type == type;
}

// EN: Tests whether the current token matches any of the given types.
// FR: Teste si le token courant correspond a un des types donnes.
bool Parser::checkAny(std::initializer_list<TokenType> types) const {
    for (TokenType type : types) {
        if (check(type)) return true;
    }
    return false;
}

// EN: Advances the token cursor and returns the consumed token.
// FR: Avance le curseur et renvoie le token consomme.
const Token& Parser::advance() {
    if (!isAtEnd()) {
        currentIndex_++;
    }
    return previous();
}

// EN: Consumes the current token if it matches the expected type.
// FR: Consomme le token courant s il correspond au type attendu.
bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

// EN: Consumes the current token if it matches any given type.
// FR: Consomme le token courant s il correspond a un des types donnes.
bool Parser::matchAny(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (match(type)) return true;
    }
    return false;
}

// EN: Requires a specific token type or raises a parse error.
// FR: Exige un type de token ou leve une erreur de parse.
const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    int colAdjust = current().column;
    if (colAdjust > 3 && message.find("expected ';'") != std::string::npos) {
        colAdjust -= 3;
    }
    errorAtPosition(current().line, colAdjust, message);
}

// EN: Synchronizes parsing after an error by skipping to a safe token.
// FR: Synchronise le parsing apres erreur en sautant a un token sur.
void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::Semicolon) return;
        
        switch (current().type) {
            case TokenType::Struct:
            case TokenType::Union:
            case TokenType::Enum:
            case TokenType::Typedef:
            case TokenType::Extern:
            case TokenType::Static:
            case TokenType::Auto:
            case TokenType::Register:
            case TokenType::Void:
            case TokenType::Char:
            case TokenType::Short:
            case TokenType::Int:
            case TokenType::Long:
            case TokenType::Float:
            case TokenType::Double:
            case TokenType::Signed:
            case TokenType::Unsigned:
            case TokenType::If:
            case TokenType::While:
            case TokenType::For:
            case TokenType::Return:
                return;
            default:
                break;
        }
        
        advance();
    }
}





// EN: Raises a parse error at the current token.
// FR: Leve une erreur de parse au token courant.
[[noreturn]] void Parser::error(const std::string& message) {
    errorAt(current(), message);
}

// EN: Raises a parse error at a specific token.
// FR: Leve une erreur de parse a un token specifique.
[[noreturn]] void Parser::errorAt(const Token& token, const std::string& message) {
    SourceLocation loc(filename_, token.line, token.column);
    throw ParseError(message, loc, getSourceLine(token.line));
}

[[noreturn]] void Parser::errorAtPosition(int line, int column, const std::string& message) {
    SourceLocation loc(filename_, line, column);
    throw ParseError(message, loc, getSourceLine(line));
}

// EN: Retrieves a specific source line for diagnostics.
// FR: Recupere une ligne source pour diagnostics.
std::string Parser::getSourceLine(int line) const {
    std::istringstream stream(source_);
    std::string currentLine;
    int currentNum = 0;
    
    while (std::getline(stream, currentLine)) {
        currentNum++;
        if (currentNum == line) {
            return currentLine;
        }
    }
    return "";
}

// EN: Builds a SourceLocation from a token.
// FR: Construit une SourceLocation depuis un token.
SourceLocation Parser::makeLocation(const Token& tok) const {
    return SourceLocation(filename_, tok.line, tok.column);
}

// TODO(cc1) EN: Improve error recovery to continue parsing after errors.
// FR: Ameliorer la recuperation d erreurs pour continuer le parsing.

} 
