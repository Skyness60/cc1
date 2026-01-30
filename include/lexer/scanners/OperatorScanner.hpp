#pragma once



#include <lexer/Token.hpp>
#include <lexer/SourceReader.hpp>

// EN: Scans operators and punctuation tokens.
// FR: Scanne les operateurs et ponctuations.
class OperatorScanner {
public:
    // EN: Attempts to scan an operator starting with c and writes to out.
    // FR: Tente de scanner un operateur demarrant par c et ecrit dans out.
    static bool scan(char c, SourceReader& r, int l, int col, Token& out) {
        switch (c) {
            case '(': out = Token(TokenType::LeftParen, "(", l, col); return true;
            case ')': out = Token(TokenType::RightParen, ")", l, col); return true;
            case '{': out = Token(TokenType::LeftBrace, "{", l, col); return true;
            case '}': out = Token(TokenType::RightBrace, "}", l, col); return true;
            case '[': out = Token(TokenType::LeftBracket, "[", l, col); return true;
            case ']': out = Token(TokenType::RightBracket, "]", l, col); return true;
            case ';': out = Token(TokenType::Semicolon, ";", l, col); return true;
            case ',': out = Token(TokenType::Comma, ",", l, col); return true;
            case '~': out = Token(TokenType::Tilde, "~", l, col); return true;
            case '?': out = Token(TokenType::Question, "?", l, col); return true;
            case ':':
                out = Token(TokenType::Colon, ":", l, col); return true;
            
            case '+':
                if (r.match('+')) { out = Token(TokenType::PlusPlus, "++", l, col); return true; }
                if (r.match('=')) { out = Token(TokenType::PlusEqual, "+=", l, col); return true; }
                out = Token(TokenType::Plus, "+", l, col); return true;
            
            case '-':
                if (r.match('-')) { out = Token(TokenType::MinusMinus, "--", l, col); return true; }
                if (r.match('=')) { out = Token(TokenType::MinusEqual, "-=", l, col); return true; }
                if (r.match('>')) { out = Token(TokenType::Arrow, "->", l, col); return true; }
                out = Token(TokenType::Minus, "-", l, col); return true;
            
            case '*': out = r.match('=') ? Token(TokenType::StarEqual, "*=", l, col) : Token(TokenType::Star, "*", l, col); return true;
            case '/': out = r.match('=') ? Token(TokenType::SlashEqual, "/=", l, col) : Token(TokenType::Slash, "/", l, col); return true;
            case '%': out = r.match('=') ? Token(TokenType::PercentEqual, "%=", l, col) : Token(TokenType::Percent, "%", l, col); return true;
            case '=': out = r.match('=') ? Token(TokenType::EqualEqual, "==", l, col) : Token(TokenType::Equal, "=", l, col); return true;
            case '!': out = r.match('=') ? Token(TokenType::NotEqual, "!=", l, col) : Token(TokenType::Exclamation, "!", l, col); return true;
            case '^': out = r.match('=') ? Token(TokenType::CaretEqual, "^=", l, col) : Token(TokenType::Caret, "^", l, col); return true;
            case '#': out = r.match('#') ? Token(TokenType::HashHash, "##", l, col) : Token(TokenType::Hash, "#", l, col); return true;
            
            case '<':
                if (r.match('<')) { out = r.match('=') ? Token(TokenType::LeftShiftEqual, "<<=", l, col) : Token(TokenType::LeftShift, "<<", l, col); return true; }
                out = r.match('=') ? Token(TokenType::LessEqual, "<=", l, col) : Token(TokenType::Less, "<", l, col); return true;
            
            case '>':
                if (r.match('>')) { out = r.match('=') ? Token(TokenType::RightShiftEqual, ">>=", l, col) : Token(TokenType::RightShift, ">>", l, col); return true; }
                out = r.match('=') ? Token(TokenType::GreaterEqual, ">=", l, col) : Token(TokenType::Greater, ">", l, col); return true;
            
            case '&':
                if (r.match('&')) { out = Token(TokenType::AmpersandAmpersand, "&&", l, col); return true; }
                if (r.match('=')) { out = Token(TokenType::AmpersandEqual, "&=", l, col); return true; }
                out = Token(TokenType::Ampersand, "&", l, col); return true;
            
            case '|':
                if (r.match('|')) { out = Token(TokenType::PipePipe, "||", l, col); return true; }
                if (r.match('=')) { out = Token(TokenType::PipeEqual, "|=", l, col); return true; }
                out = Token(TokenType::Pipe, "|", l, col); return true;
            
            case '.':
                if (r.peek() == '.' && r.peekNext() == '.') {
                    r.advance(); r.advance();
                    out = Token(TokenType::Ellipsis, "...", l, col); return true;
                }
                out = Token(TokenType::Dot, ".", l, col); return true;
            
            default: return false;
        }
    }
};

// TODO(cc1) EN: Centralize operator definitions to avoid duplication with parser.
// FR: Centraliser les definitions d operateurs pour eviter les doublons parser.
