// ----------------------------
// File: src/front/token.rs
// ----------------------------

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Span { pub lo: usize, pub hi: usize, pub line: u32, pub col: u32 }

impl Span {
    pub fn dummy() -> Self {
        Self { lo: 0, hi: 0, line: 1, col: 1 }
    }
}

impl Default for Span {
    fn default() -> Self {
        Self::dummy()
    }
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum TokenKind {
    // Keywords (C89)
    Int, Char, Void, Return, If, Else, While, For, Do, Break, Continue,
    Switch, Case, Default, Goto, Sizeof, Typedef, Static, Extern, Const,
    Volatile, Struct, Union, Enum, Auto, Register, Signed, Unsigned,
    Short, Long, Float, Double,
    // Ident & literals
    Ident, IntLit, FloatLit, StrLit, CharLit,
    // Operators
    PlusAssign, MinusAssign, StarAssign, SlashAssign, PercentAssign,
    AmpAssign, PipeAssign, CaretAssign, ShlAssign, ShrAssign,
    Shl, Shr,
    Plus, Minus, Star, Slash, Percent,
    Assign, Eq, Ne, Le, Ge, Lt, Gt,
    And, Or, Not,
    Amp, Pipe, Caret, Tilde,
    Inc, Dec,
    Arrow, Dot, Ellipsis,
    // Delimiters
    LParen, RParen, LBrace, RBrace, LBracket, RBracket,
    Semicolon, Comma, Colon, Question, Hash,
    // Special
    Eof,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Token { pub kind: TokenKind, pub lexeme: String, pub span: Span }

#[derive(Debug, Clone)]
pub struct LexError { pub file: String, pub msg: String, pub span: Span }

impl TokenKind {
    pub fn from_keyword(s: &str) -> Option<Self> {
        use TokenKind::*;
        Some(match s {
            "int" => Int, "char" => Char, "void" => Void, "return" => Return,
            "if" => If, "else" => Else, "while" => While, "for" => For, "do" => Do,
            "break" => Break, "continue" => Continue, "switch" => Switch,
            "case" => Case, "default" => Default, "goto" => Goto, "sizeof" => Sizeof,
            "typedef" => Typedef, "static" => Static, "extern" => Extern,
            "const" => Const, "volatile" => Volatile, "struct" => Struct,
            "union" => Union, "enum" => Enum, "auto" => Auto, "register" => Register,
            "signed" => Signed, "unsigned" => Unsigned, "short" => Short, "long" => Long,
            "float" => Float, "double" => Double,
            _ => return None,
        })
    }
}
