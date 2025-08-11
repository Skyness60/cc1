// ----------------------------
// File: src/front/lexer/number.rs
// ----------------------------
use super::*;

impl<'a> Lexer<'a> {
    pub(super) fn read_number(&mut self) -> Result<Token, LexError> {
        let start = (self.i, self.line, self.col);
        let mut is_float = false;
        let mut saw_digits = false; // digits in mantissa (either side)
        let mut invalid_octal = false;
        let mut first_zero = false;

        // Hexadecimal 0x... (integer constants only in C89)
        if self.cur() == Some(b'0') && matches!(self.peek(1), Some(b'x'|b'X')) {
            self.bump(); self.bump();
            let mut saw_hex = false;
            while let Some(b) = self.cur() { if Lexer::is_hex_digit(b) { self.bump(); saw_hex = true; } else { break; } }
            if !saw_hex {
                return self.make_err(start, "malformed hexadecimal constant: missing digits after 0x");
            }
            // Suffixes handled below (U/L)
        } else {
            if self.cur() == Some(b'.') {
                // cases like .5 (float)
                is_float = true; self.bump();
                while let Some(b) = self.cur() { if Lexer::is_dec_digit(b) { self.bump(); saw_digits = true; } else { break; } }
            } else {
                // leading digits
                if self.cur() == Some(b'0') { first_zero = true; }
                while let Some(b) = self.cur() {
                    if Lexer::is_dec_digit(b) {
                        if first_zero && matches!(b, b'8'|b'9') { invalid_octal = true; }
                        self.bump();
                        saw_digits = true;
                    } else { break; }
                }
                // optional fractional part: allow 1. and 1.e10
                if self.cur() == Some(b'.') {
                    is_float = true; self.bump();
                    while let Some(b) = self.cur() { if Lexer::is_dec_digit(b) { self.bump(); saw_digits = true; } else { break; } }
                }
            }
            // exponent part (decimal floats)
            if matches!(self.cur(), Some(b'e'|b'E')) {
                is_float = true; self.bump(); if matches!(self.cur(), Some(b'+'|b'-')) { self.bump(); }
                let mut saw_exp_digit = false; while let Some(b) = self.cur() { if Lexer::is_dec_digit(b) { self.bump(); saw_exp_digit = true; } else { break; } }
                if !saw_exp_digit { return self.make_err(start, "malformed floating constant: missing exponent digits"); }
            }
        }

        // Suffixes
        let mut u = false; let mut l = 0u8; let mut f = false;
        loop {
            match self.cur() {
                Some(b'u') | Some(b'U') => { u = true; self.bump(); },
                Some(b'l') | Some(b'L') => { if l < 2 { l += 1; } self.bump(); },
                Some(b'f') | Some(b'F') => { f = true; self.bump(); },
                _ => break,
            }
        }

        // Validate mantissa presence for floats starting with '.'
        if is_float && !saw_digits {
            return self.make_err(start, "malformed floating constant: no digits in mantissa");
        }

        // Validate suffixes per C89
        if is_float {
            if u { return self.make_err(start, "invalid suffix for floating constant: 'u'"); }
            if l > 1 { return self.make_err(start, "invalid floating suffix: 'LL' not allowed"); }
            if f && l > 0 { return self.make_err(start, "invalid floating suffix combination: both 'f' and 'l'"); }
            // f alone or l alone is fine
        } else {
            // integer
            if f { return self.make_err(start, "invalid suffix for integer constant: 'f'"); }
            if l > 1 { return self.make_err(start, "invalid integer suffix: 'LL' is not C89"); }
            if invalid_octal { return self.make_err(start, "invalid digit in octal constant"); }
        }

        let lexeme = &self.src[start.0..self.i];
        let kind = if is_float { K::FloatLit } else { K::IntLit };
        Ok(Token { kind, lexeme: lexeme.to_string(), span: self.span_from(start) })
    }
}
