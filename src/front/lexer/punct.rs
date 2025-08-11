// ----------------------------
// File: src/front/lexer/punct.rs
// ----------------------------
use super::*;

impl<'a> Lexer<'a> {
    pub(super) fn read_punct(&mut self) -> Result<Token, LexError> {
        let start = (self.i, self.line, self.col);
        let k = match (self.cur(), self.peek(1), self.peek(2)) {
            // longest matches first
            (Some(b'.'), Some(b'.'), Some(b'.')) => { self.bump(); self.bump(); self.bump(); K::Ellipsis }
            (Some(b'<'), Some(b'<'), Some(b'=')) => { self.bump(); self.bump(); self.bump(); K::ShlAssign }
            (Some(b'>'), Some(b'>'), Some(b'=')) => { self.bump(); self.bump(); self.bump(); K::ShrAssign }
            (Some(b'-'), Some(b'>'), _) => { self.bump(); self.bump(); K::Arrow }
            (Some(b'+'), Some(b'+'), _) => { self.bump(); self.bump(); K::Inc }
            (Some(b'-'), Some(b'-'), _) => { self.bump(); self.bump(); K::Dec }
            (Some(b'='), Some(b'='), _) => { self.bump(); self.bump(); K::Eq }
            (Some(b'!'), Some(b'='), _) => { self.bump(); self.bump(); K::Ne }
            (Some(b'<'), Some(b'='), _) => { self.bump(); self.bump(); K::Le }
            (Some(b'>'), Some(b'='), _) => { self.bump(); self.bump(); K::Ge }
            (Some(b'&'), Some(b'&'), _) => { self.bump(); self.bump(); K::And }
            (Some(b'|'), Some(b'|'), _) => { self.bump(); self.bump(); K::Or }
            (Some(b'<'), Some(b'<'), _) => { self.bump(); self.bump(); K::Shl }
            (Some(b'>'), Some(b'>'), _) => { self.bump(); self.bump(); K::Shr }
            (Some(b'+'), Some(b'='), _) => { self.bump(); self.bump(); K::PlusAssign }
            (Some(b'-'), Some(b'='), _) => { self.bump(); self.bump(); K::MinusAssign }
            (Some(b'*'), Some(b'='), _) => { self.bump(); self.bump(); K::StarAssign }
            (Some(b'/'), Some(b'='), _) => { self.bump(); self.bump(); K::SlashAssign }
            (Some(b'%'), Some(b'='), _) => { self.bump(); self.bump(); K::PercentAssign }
            (Some(b'&'), Some(b'='), _) => { self.bump(); self.bump(); K::AmpAssign }
            (Some(b'|'), Some(b'='), _) => { self.bump(); self.bump(); K::PipeAssign }
            (Some(b'^'), Some(b'='), _) => { self.bump(); self.bump(); K::CaretAssign }
            (Some(b'+'), _, _) => { self.bump(); K::Plus }
            (Some(b'-'), _, _) => { self.bump(); K::Minus }
            (Some(b'*'), _, _) => { self.bump(); K::Star }
            (Some(b'/'), _, _) => { self.bump(); K::Slash }
            (Some(b'%'), _, _) => { self.bump(); K::Percent }
            (Some(b'='), _, _) => { self.bump(); K::Assign }
            (Some(b'!'), _, _) => { self.bump(); K::Not }
            (Some(b'<'), _, _) => { self.bump(); K::Lt }
            (Some(b'>'), _, _) => { self.bump(); K::Gt }
            (Some(b'&'), _, _) => { self.bump(); K::Amp }
            (Some(b'|'), _, _) => { self.bump(); K::Pipe }
            (Some(b'^'), _, _) => { self.bump(); K::Caret }
            (Some(b'~'), _, _) => { self.bump(); K::Tilde }
            (Some(b'.'), _, _) => { self.bump(); K::Dot }
            (Some(b'('), _, _) => { self.bump(); K::LParen }
            (Some(b')'), _, _) => { self.bump(); K::RParen }
            (Some(b'{'), _, _) => { self.bump(); K::LBrace }
            (Some(b'}'), _, _) => { self.bump(); K::RBrace }
            (Some(b'['), _, _) => { self.bump(); K::LBracket }
            (Some(b']'), _, _) => { self.bump(); K::RBracket }
            (Some(b';'), _, _) => { self.bump(); K::Semicolon }
            (Some(b','), _, _) => { self.bump(); K::Comma }
            (Some(b':'), _, _) => { self.bump(); K::Colon }
            (Some(b'?'), _, _) => { self.bump(); K::Question }
            (Some(b'#'), _, _) => { self.bump(); K::Hash }
            _ => { return self.make_err(start, "unknown character"); }
        };
        let lexeme = &self.src[start.0..self.i];
        Ok(Token { kind: k, lexeme: lexeme.to_string(), span: self.span_from(start) })
    }
}
