#include <parser/Parser.hpp>
#include <sstream>
#include <iostream>

namespace cc1 {

// ============================================================================
// Constructor
// ============================================================================

Parser::Parser(const std::vector<Token>& tokens,
               const std::string& filename,
               const std::string& source)
    : tokens_(tokens)
    , filename_(filename)
    , source_(source)
{
}

// ============================================================================
// Main Entry Point
// ============================================================================

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
            // Stop after first error like clang does
            break;
        }
    }
    
    return unit;
}

// ============================================================================
// Token Stream Management
// ============================================================================

const Token& Parser::current() const {
    if (currentIndex_ >= tokens_.size()) {
        return tokens_.back(); // Should be EOF
    }
    return tokens_[currentIndex_];
}

const Token& Parser::peek(int offset) const {
    size_t index = currentIndex_ + offset;
    if (index >= tokens_.size()) {
        return tokens_.back();
    }
    return tokens_[index];
}

const Token& Parser::previous() const {
    if (currentIndex_ == 0) {
        return tokens_[0];
    }
    return tokens_[currentIndex_ - 1];
}

bool Parser::isAtEnd() const {
    return current().type == TokenType::EndOfFile;
}

bool Parser::check(TokenType type) const {
    return current().type == type;
}

bool Parser::checkAny(std::initializer_list<TokenType> types) const {
    for (TokenType type : types) {
        if (check(type)) return true;
    }
    return false;
}

const Token& Parser::advance() {
    if (!isAtEnd()) {
        currentIndex_++;
    }
    return previous();
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::matchAny(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (match(type)) return true;
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    error(message);
}

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

// ============================================================================
// Error Handling
// ============================================================================

[[noreturn]] void Parser::error(const std::string& message) {
    errorAt(current(), message);
}

[[noreturn]] void Parser::errorAt(const Token& token, const std::string& message) {
    SourceLocation loc(filename_, token.line, token.column);
    throw ParseError(message, loc, getSourceLine(token.line));
}

[[noreturn]] void Parser::errorAtPosition(int line, int column, const std::string& message) {
    SourceLocation loc(filename_, line, column);
    throw ParseError(message, loc, getSourceLine(line));
}

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

SourceLocation Parser::makeLocation(const Token& tok) const {
    return SourceLocation(filename_, tok.line, tok.column);
}

} // namespace cc1
