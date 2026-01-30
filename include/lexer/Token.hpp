#pragma once

#include <string>
#include <ostream>




// EN: Enumerates all token kinds produced by the lexer.
// FR: Enumere tous les types de tokens produits par le lexer.
enum class TokenType {
    
    EndOfFile,

    
    Identifier,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,
    CharLiteral,

    
    Auto, Break, Case, Char, Const, Continue, Default, Do, Double, Else, Enum, Extern,
    Float, For, Goto, If, Int, Long, Register, Return, Short, Signed, Sizeof, Static,
    Struct, Switch, Typedef, Union, Unsigned, Void, Volatile, While,

    
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
    
    
    Hash, HashHash
};


// EN: Represents a lexical token with type, value, and source position.
// FR: Represente un token lexical avec type, valeur et position source.
struct Token {
    TokenType type;
    std::string value; 
    int line;
    int column;

    // EN: Constructs a token with type, value, and location.
    // FR: Construit un token avec type, valeur et position.
    Token(TokenType t, std::string v, int l, int c)
        : type(t), value(std::move(v)), line(l), column(c) {}
};


// EN: Converts a token type to a human-readable string.
// FR: Convertit un type de token en chaine lisible.
std::string tokenTypeToString(TokenType type);


// EN: Streams a token for debugging/diagnostics.
// FR: Ecrit un token dans un flux pour debug/diagnostics.
std::ostream& operator<<(std::ostream& os, const Token& token);

// TODO(cc1) EN: Add end position to tokens for better error ranges.
// FR: Ajouter la position de fin aux tokens pour de meilleurs ranges.
