// ----------------------------
// File: src/front/lexer/ident.rs
// ----------------------------
use super::*;

impl<'a> Lexer<'a> {
    pub(super) fn read_ident_or_kw(&mut self) -> Token {
        let start = (self.i, self.line, self.col);
        self.bump();
        while let Some(b) = self.cur() { if Lexer::is_ident_continue(b) { self.bump(); } else { break; } }
        let lexeme = &self.src[start.0..self.i];
        let kind = super::super::token::TokenKind::from_keyword(lexeme).unwrap_or(K::Ident);
        Token { kind, lexeme: lexeme.to_string(), span: self.span_from(start) }
    }
}