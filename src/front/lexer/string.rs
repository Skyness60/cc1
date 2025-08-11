// ----------------------------
// File: src/front/lexer/string.rs
// ----------------------------
use super::*;

impl<'a> Lexer<'a> {
    pub(super) fn read_string(&mut self) -> Result<Token, LexError> {
        let start = (self.i, self.line, self.col);
        self.bump(); // opening quote
        loop {
            if self.cur().is_none() { return self.make_err(start, "unterminated string literal"); }
            match self.cur().unwrap() {
                b'"' => { self.bump(); break; }
                b'\\' => {
                    self.bump();
                    if self.cur().is_none() { return self.make_err(start, "unterminated string literal"); }
                    match self.cur().unwrap() {
                        b'\n' => { self.bump(); }
                        b'\'' | b'"' | b'\\' | b'?' |
                        b'a' | b'b' | b'f' | b'n' | b'r' | b't' | b'v' => { self.bump(); }
                        b'x' => { // \x[1+ hex]
                            self.bump();
                            let mut saw = false;
                            while let Some(c) = self.cur() { if Lexer::is_hex_digit(c) { self.bump(); saw = true; } else { break; } }
                            if !saw { return self.make_err(start, "\\x escape requires at least one hex digit"); }
                        }
                        c if (b'0'..=b'7').contains(&c) => { // up to 3 octal digits
                            let mut k = 0; while k < 3 { if let Some(d) = self.cur() { if (b'0'..=b'7').contains(&d) { self.bump(); k += 1; } else { break; } } else { break; } }
                        }
                        _ => { return self.make_err(start, "invalid escape sequence in string literal"); }
                    }
                }
                b'\n' => { return self.make_err(start, "unterminated string literal"); }
                _ => { self.bump(); }
            }
        }
        let lexeme = &self.src[start.0..self.i];
        Ok(Token { kind: K::StrLit, lexeme: lexeme.to_string(), span: self.span_from(start) })
    }
}