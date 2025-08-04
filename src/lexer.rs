use logos::Logos;

#[derive(Logos, Debug, PartialEq, Clone)]
pub enum Token {
    // C89 Keywords
    #[token("int")]
    Int,
    #[token("char")]
    Char,
    #[token("void")]
    Void,
    #[token("return")]
    Return,
    #[token("if")]
    If,
    #[token("else")]
    Else,
    #[token("while")]
    While,
    #[token("for")]
    For,
    #[token("do")]
    Do,
    #[token("break")]
    Break,
    #[token("continue")]
    Continue,
    #[token("switch")]
    Switch,
    #[token("case")]
    Case,
    #[token("default")]
    Default,
    #[token("goto")]
    Goto,
    #[token("sizeof")]
    Sizeof,
    #[token("typedef")]
    Typedef,
    #[token("static")]
    Static,
    #[token("extern")]
    Extern,
    #[token("const")]
    Const,
    #[token("volatile")]
    Volatile,
    #[token("struct")]
    Struct,
    #[token("union")]
    Union,
    #[token("enum")]
    Enum,
    #[token("auto")]
    Auto,
    #[token("register")]
    Register,
    #[token("signed")]
    Signed,
    #[token("unsigned")]
    Unsigned,
    #[token("short")]
    Short,
    #[token("long")]
    Long,
    #[token("float")]
    Float,
    #[token("double")]
    Double,

    // Operators
    #[token("+=")]
    PlusAssign,
    #[token("-=")]
    MinusAssign,
    #[token("*=")]
    StarAssign,
    #[token("/=")]
    SlashAssign,
    #[token("%=")]
    PercentAssign,
    #[token("&=")]
    AmpersandAssign,
    #[token("|=")]
    PipeAssign,
    #[token("^=")]
    CaretAssign,
    #[token("<<=")]
    LeftShiftAssign,
    #[token(">>=")]
    RightShiftAssign,
    #[token("<<")]
    LeftShift,
    #[token(">>")]
    RightShift,
    #[token("+")]
    Plus,
    #[token("-")]
    Minus,
    #[token("*")]
    Star,
    #[token("/")]
    Slash,
    #[token("%")]
    Percent,
    #[token("=")]
    Assign,
    #[token("==")]
    Equal,
    #[token("!=")]
    NotEqual,
    #[token("<=")]
    LessEqual,
    #[token(">=")]
    GreaterEqual,
    #[token("<")]
    Less,
    #[token(">")]
    Greater,
    #[token("&&")]
    And,
    #[token("||")]
    Or,
    #[token("!")]
    Not,
    #[token("&")]
    Ampersand,
    #[token("|")]
    Pipe,
    #[token("^")]
    Caret,
    #[token("~")]
    Tilde,
    #[token("++")]
    Increment,
    #[token("--")]
    Decrement,
    #[token("->")]
    Arrow,
    #[token(".")]
    Dot,

    // Delimiters
    #[token("(")]
    LeftParen,
    #[token(")")]
    RightParen,
    #[token("{")]
    LeftBrace,
    #[token("}")]
    RightBrace,
    #[token("[")]
    LeftBracket,
    #[token("]")]
    RightBracket,
    #[token(";")]
    Semicolon,
    #[token(",")]
    Comma,
    #[token(":")]
    Colon,
    #[token("?")]
    Question,
    #[token("#")]
    Hash,

    // Literals
    // Nombres hexadécimaux (0x ou 0X)
    #[regex(r"0[xX][0-9a-fA-F]+[uUlL]*", |lex| lex.slice().to_owned())]
    HexLiteral(String),
    
    // Nombres octaux (commencent par 0)
    #[regex(r"0[0-7]+[uUlL]*", |lex| lex.slice().to_owned())]
    OctalLiteral(String),
    
    // Nombres flottants
    #[regex(r"[0-9]*\.[0-9]+([eE][+-]?[0-9]+)?[fFlL]?", |lex| lex.slice().to_owned())]
    FloatLiteral(String),
    
    // Nombres flottants avec exposant sans point
    #[regex(r"[0-9]+[eE][+-]?[0-9]+[fFlL]?", |lex| lex.slice().to_owned())]
    ScientificLiteral(String),
    
    // Nombres entiers décimaux (avec ou sans suffixes)
    #[regex(r"[1-9][0-9]*[uUlL]*", |lex| lex.slice().to_owned())]
    IntLiteral(String),
    
    // Zéro seul
    #[regex(r"0[uUlL]*", |lex| lex.slice().to_owned())]
    ZeroLiteral(String),
    
    // Chaînes de caractères
    #[regex(r#""([^"\\]|\\.)*""#, |lex| lex.slice().to_owned())]
    StringLiteral(String),
    
    // Caractères
    #[regex(r"'([^'\\]|\\.)'", |lex| lex.slice().to_owned())]
    CharLiteral(String),

    // Identifiers
    #[regex(r"[a-zA-Z_][a-zA-Z0-9_]*", |lex| lex.slice().to_owned())]
    Identifier(String),

    // Comments (skip)
    #[regex(r"/\*([^*]|\*[^/])*\*/", logos::skip)]
    
    // Whitespace (skip)
    #[regex(r"[ \t\n\r]+", logos::skip)]
    Whitespace,
}


pub fn tokenize(input: &str) -> Result<Vec<Token>, String> {
    let mut tokens = Vec::new();
    let mut lexer = Token::lexer(input);
    
    while let Some(token_result) = lexer.next() {
        match token_result {
            Ok(token) => tokens.push(token),
            Err(_) => {
                return Err(format!("Lexical error at position {}", lexer.span().start));
            }
        }
    }
    
    Ok(tokens)
}