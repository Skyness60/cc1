// ----------------------------
// File: src/front/lexer/mod.rs
// ----------------------------
use super::token::{LexError, Span, Token, TokenKind as K};

mod ident;   // read_ident_or_kw
mod number;  // read_number
mod string;  // read_string
mod chlit;   // read_char_lit
mod punct;   // read_punct

pub struct Lexer<'a> {
    src: &'a str,
    bytes: &'a [u8],
    i: usize,
    line: u32,
    col: u32,
    file: String,
    done_eof: bool,
}

impl<'a> Lexer<'a> {
    pub fn new(src: &'a str, file: &str) -> Self {
        Self { src, bytes: src.as_bytes(), i: 0, line: 1, col: 1, file: file.to_string(), done_eof: false }
    }

    // --- low-level helpers (accessibles aux sous-modules) ---
    fn eof(&self) -> bool { self.i >= self.bytes.len() }
    fn cur(&self) -> Option<u8> { self.bytes.get(self.i).copied() }
    fn peek(&self, n: usize) -> Option<u8> { self.bytes.get(self.i + n).copied() }
    fn bump(&mut self) -> Option<u8> {
        let b = self.bytes.get(self.i).copied();
        if let Some(c) = b { self.i += 1; if c == b'\n' { self.line += 1; self.col = 1; } else { self.col += 1; } }
        b
    }
    fn span_from(&self, start: (usize, u32, u32)) -> Span { Span { lo: start.0, hi: self.i, line: start.1, col: start.2 } }
    fn make_err<T>(&self, start: (usize, u32, u32), msg: &str) -> Result<T, LexError> { Err(LexError { file: self.file.clone(), msg: msg.to_string(), span: Span { lo: start.0, hi: self.i, line: start.1, col: start.2 } }) }

    // predicates
    fn is_ident_start(b: u8) -> bool { (b'A'..=b'Z').contains(&b) || (b'a'..=b'z').contains(&b) || b == b'_' }
    fn is_ident_continue(b: u8) -> bool { Self::is_ident_start(b) || (b'0'..=b'9').contains(&b) }
    fn is_dec_digit(b: u8) -> bool { (b'0'..=b'9').contains(&b) }
    fn is_hex_digit(b: u8) -> bool { (b'0'..=b'9').contains(&b) || (b'a'..=b'f').contains(&b) || (b'A'..=b'F').contains(&b) }

    fn skip_ws_and_comments(&mut self) -> Result<(), LexError> {
        loop {
            // whitespace
            while let Some(b) = self.cur() { if matches!(b, b' ' | b'\t' | b'\n' | b'\r') { self.bump(); } else { break; } }
            // block comments /* ... */
            if self.cur() == Some(b'/') && self.peek(1) == Some(b'*') {
                self.bump(); self.bump();
                let start = (self.i, self.line, self.col);
                let mut closed = false;
                while let Some(c) = self.cur() {
                    if c == b'*' && self.peek(1) == Some(b'/') { self.bump(); self.bump(); closed = true; break; }
                    else { self.bump(); }
                }
                if !closed { return self.make_err(start, "unterminated block comment"); }
                continue; // continue stripping
            }
            // C89 strict: forbid // line comments
            if self.cur() == Some(b'/') && self.peek(1) == Some(b'/') {
                let start = (self.i, self.line, self.col);
                return self.make_err(start, "'//' line comments are not allowed in C89");
            }
            break;
        }
        Ok(())
    }

    fn read_string_concat(&mut self) -> Result<Token, LexError> {
        let start = (self.i, self.line, self.col);
        // read at least one string
        let _first = self.read_string()?;
        loop {
            // skip ws/comments between adjacent strings
            self.skip_ws_and_comments()?;
            if self.cur() == Some(b'"') {
                // another adjacent string
                let _ = self.read_string()?;
                continue;
            } else {
                break;
            }
        }
        let lexeme = &self.src[start.0..self.i];
        Ok(Token { kind: K::StrLit, lexeme: lexeme.to_string(), span: self.span_from(start) })
    }

    fn next_token_core(&mut self) -> Result<Token, LexError> {
        self.skip_ws_and_comments()?;
        if self.eof() { let sp = Span { lo: self.i, hi: self.i, line: self.line, col: self.col }; return Ok(Token { kind: K::Eof, lexeme: String::new(), span: sp }); }
        let b = self.cur().unwrap();

        // C89 strict: no wide string/char prefixes (L".." or L'..')
        if b == b'L' && matches!(self.peek(1), Some(b'\''|b'"')) {
            let start = (self.i, self.line, self.col);
            return self.make_err(start, "wide strings/chars (L prefix) are not supported in C89");
        }

        if Self::is_ident_start(b) { return Ok(self.read_ident_or_kw()); }
        if Self::is_dec_digit(b) { return self.read_number(); }
        if b == b'.' {
            if self.peek(1) == Some(b'.') { return self.read_punct(); }
            if self.peek(1).map(Self::is_dec_digit).unwrap_or(false) { return self.read_number(); }
            return self.read_punct();
        }
        match b { b'"' => return self.read_string_concat(), b'\'' => return self.read_char_lit(), _ => {} }
        self.read_punct()
    }
}

impl<'a> Iterator for Lexer<'a> {
    type Item = Result<Token, LexError>;
    fn next(&mut self) -> Option<Self::Item> {
        if self.done_eof { return None; }
        let res = self.next_token_core();
        if let Ok(tok) = &res { if matches!(tok.kind, K::Eof) { self.done_eof = true; } }
        Some(res)
    }
}

// Tests moved to external .c files under tests/and exercised via running ./cc1
