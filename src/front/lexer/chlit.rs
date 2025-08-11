// ----------------------------
// File: src/front/lexer/chlit.rs
// ----------------------------
use super::*;

#[allow(non_snake_case)]
impl<'a> Lexer<'a> {
    pub(super) fn read_char_lit(&mut self) -> Result<Token, LexError> {
        let start = (self.i, self.line, self.col);
        self.bump(); // opening '
        let mut count = 0u8;
        loop {
            if self.cur().is_none() { return self.make_err(start, "unterminated char literal"); }
            match self.cur().unwrap() {
                b'\'' => { self.bump(); break; }
                b'\\' => {
                    self.bump();
                    if self.cur().is_none() { return self.make_err(start, "unterminated char literal"); }
                    match self.cur().unwrap() {
                        b'\'' | b'"' | b'\\' | b'?' |
                        b'a' | b'b' | b'f' | b'n' | b'r' | b't' | b'v' | b'\n' => { self.bump(); count = count.saturating_add(1); }
                        b'x' => {
                            self.bump();
                            let mut saw = false;
                            while let Some(c) = self.cur() {
                                if Lexer::is_hex_digit(c) { self.bump(); saw = true; } else { break; }
                            }
                            if !saw { return self.make_err(start, "\\x escape requires at least one hex digit"); }
                            count = count.saturating_add(1);
                        }
                        c if (b'0'..=b'7').contains(&c) => { let mut k = 0; while k < 3 { if let Some(d) = self.cur() { if (b'0'..=b'7').contains(&d) { self.bump(); k += 1; } else { break; } } else { break; } } count = count.saturating_add(1); }
                        _ => { return self.make_err(start, "invalid escape sequence in char literal"); }
                    }
                }
                b'\n' => { return self.make_err(start, "unterminated char literal"); }
                _ => { self.bump(); count = count.saturating_add(1); }
            }
        }
        let lexeme = &self.src[start.0..self.i];
        Ok(Token { kind: K::CharLit, lexeme: lexeme.to_string(), span: self.span_from(start) })
    }
}
