#pragma once

#include <string>
#include <ostream>

enum class TokenType {
    // End of File
    EndOfFile,

    // Identifiers and Literals
    Identifier,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    CharLiteral,

    // Keywords
    Auto, Break, Case, Char, Const, Continue, Default, Do, Double, Else, Enum, Extern,
    Float, For, Goto, If, Int, Long, Register, Return, Short, Signed, Sizeof, Static,
    Struct, Switch, Typedef, Union, Unsigned, Void, Volatile, While,

    // Operators and Punctuation
    Plus, Minus, Star, Slash, Percent,
    PlusPlus, MinusMinus,
    Equal, EqualEqual, NotEqual,
    Less, LessEqual, Greater, GreaterEqual,
    Ampersand, Pipe, Caret, Tilde, Exclamation,
    AmpersandAmpersand, PipePipe,
    LeftShift, RightShift,
    PlusEqual, MinusEqual, StarEqual, SlashEqual, PercentEqual,
    AmpersandEqual, PipeEqual, CaretEqual,
    LeftShiftEqual, RightShiftEqual,
    
    LeftParen, RightParen,
    LeftBrace, RightBrace,
    LeftBracket, RightBracket,
    
    Semicolon, Comma, Dot, Arrow, Colon, Question,
    Ellipsis,
    
    // Preprocessor (if needed, though subject says clang -E is used)
    Hash, HashHash
};

struct Token {
    TokenType type;
    std::string value; // The lexeme
    int line;
    int column;

    Token(TokenType t, std::string v, int l, int c)
        : type(t), value(std::move(v)), line(l), column(c) {}
};

std::string tokenTypeToString(TokenType type);
std::ostream& operator<<(std::ostream& os, const Token& token);
