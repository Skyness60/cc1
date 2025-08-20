// ----------------------------
// File: src/front/parser/stream.rs
// ----------------------------
//! TokenStream: wrapper autour du lexer avec lookahead et erreurs

use crate::front::lexer::Lexer;
use crate::front::token::{Token, TokenKind as K, LexError, Span};

pub struct TokenStream<'a> {
    it: Lexer<'a>,
    buf: Vec<Token>,
    done: bool,
}

impl<'a> TokenStream<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self { Self { it: lexer, buf: Vec::new(), done: false } }
    pub fn peek(&mut self, n: usize) -> Result<&Token, LexError> {
        while self.buf.len() <= n && !self.done {
            match self.it.next() {
                Some(Ok(t)) => { if matches!(t.kind, K::Eof) { self.done = true; } self.buf.push(t); }
                Some(Err(e)) => return Err(e),
                None => { self.done = true; break; }
            }
        }
        if n < self.buf.len() {
            return Ok(&self.buf[n]);
        }
        // Si le buffer est vide, synthétiser un EOF sûr
        if self.buf.is_empty() {
            self.buf.push(Token { kind: K::Eof, lexeme: String::new(), span: Span { lo: 0, hi: 0, line: 1, col: 1 } });
        }
        Ok(self.buf.last().unwrap())
    }
    pub fn bump(&mut self) -> Result<Token, LexError> {
        self.peek(0)?; // ensure buffered
        Ok(self.buf.remove(0))
    }
    pub fn expect_kind(&mut self, k: K) -> Result<Token, LexError> {
        let t = self.peek(0)?; if t.kind == k { self.bump() } else { Err(LexError { file: String::new(), msg: format!("expected {:?}", k), span: t.span }) }
    }

    // Helpers
    pub fn check(&mut self, k: K) -> Result<bool, LexError> { Ok(self.peek(0)?.kind == k) }
    pub fn matches(&mut self, k: K) -> Result<bool, LexError> {
        if self.check(k)? { let _ = self.bump()?; Ok(true) } else { Ok(false) }
    }
}
