// ----------------------------
// File: src/front/parser/expr.rs
// ----------------------------
//! Pratt parser des expressions (C89, sous-ensemble initial)

use super::*;
use crate::front::token::TokenKind as K;

#[derive(Debug, Clone, Copy, PartialEq, PartialOrd)]
pub enum Prec {
    Lowest,
    Comma,
    Assign,
    Cond,
    LogOr,
    LogAnd,
    BitOr,
    BitXor,
    BitAnd,
    Eq,
    Rel,
    Shift,
    Add,
    Mul,
    Postfix,
}

impl Prec { fn next(self) -> Prec { use Prec::*; match self { Lowest => Comma, Comma => Assign, Assign => Cond, Cond => LogOr, LogOr => LogAnd, LogAnd => BitOr, BitOr => BitXor, BitXor => BitAnd, BitAnd => Eq, Eq => Rel, Rel => Shift, Shift => Add, Add => Mul, Mul => Postfix, Postfix => Postfix } } }

impl<'a> Parser<'a> {
    pub fn parse_expr(&mut self) -> PResult<Expr> { self.parse_prec(Prec::Comma) }
    
    pub fn parse_assign_expr(&mut self) -> PResult<Expr> { 
        // Parse expression excluding comma operator (for enum constants, array sizes, etc.)
        self.parse_prec(Prec::Cond) 
    }

    fn parse_prec(&mut self, prec: Prec) -> PResult<Expr> {
        if prec == Prec::Postfix { return self.parse_postfix(); }
        let mut lhs = self.parse_prec(prec.next())?;
        loop {
            let t = match self.ts.peek(0) { Ok(t) => t.clone(), Err(e) => return Err(ParseError{ msg: e.msg, span: e.span }) };
            let expr = match t.kind {
                // , séquence
                K::Comma if prec <= Prec::Comma => { 
                    let _ = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; 
                    let rhs = self.parse_prec(Prec::Comma.next())?; 
                    let span = join(lhs.span(), rhs.span()); 
                    Expr::Binary { op: BinaryOp::Comma, lhs: Box::new(lhs), rhs: Box::new(rhs), span } 
                }
                // ?: conditionnel
                K::Question if prec <= Prec::Cond => {
                    let _ = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                    let texpr = self.parse_expr()?;
                    let _ = self.ts.expect_kind(K::Colon).map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                    let eexpr = self.parse_prec(Prec::Cond)?;
                    let span = join(lhs.span(), eexpr.span());
                    Expr::Cond { c: Box::new(lhs), t: Box::new(texpr), e: Box::new(eexpr), span }
                }
                // Assignations
                k @ (K::Assign | K::PlusAssign | K::MinusAssign | K::StarAssign | K::SlashAssign | K::PercentAssign | K::AmpAssign | K::PipeAssign | K::CaretAssign | K::ShlAssign | K::ShrAssign) if prec <= Prec::Assign => {
                    let _ = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                    let rhs = self.parse_prec(Prec::Assign.next())?;
                    let span = join(lhs.span(), rhs.span());
                    let aop = match k {
                        K::Assign => AssignOp::Assign,
                        K::PlusAssign => AssignOp::Add,
                        K::MinusAssign => AssignOp::Sub,
                        K::StarAssign => AssignOp::Mul,
                        K::SlashAssign => AssignOp::Div,
                        K::PercentAssign => AssignOp::Mod,
                        K::AmpAssign => AssignOp::And,
                        K::PipeAssign => AssignOp::Or,
                        K::CaretAssign => AssignOp::Xor,
                        K::ShlAssign => AssignOp::Shl,
                        K::ShrAssign => AssignOp::Shr,
                        _ => unreachable!(),
                    };
                    Expr::Assign { op: aop, lhs: Box::new(lhs), rhs: Box::new(rhs), span }
                }
                // || && | ^ &
                K::Or if prec <= Prec::LogOr => bin!(self, lhs, Prec::LogOr, BinaryOp::LogOr),
                K::And if prec <= Prec::LogAnd => bin!(self, lhs, Prec::LogAnd, BinaryOp::LogAnd),
                K::Pipe if prec <= Prec::BitOr => bin!(self, lhs, Prec::BitOr, BinaryOp::BitOr),
                K::Caret if prec <= Prec::BitXor => bin!(self, lhs, Prec::BitXor, BinaryOp::BitXor),
                K::Amp if prec <= Prec::BitAnd => bin!(self, lhs, Prec::BitAnd, BinaryOp::BitAnd),
                // == != < <= > >=
                K::Eq if prec <= Prec::Eq => bin!(self, lhs, Prec::Eq, BinaryOp::Eq),
                K::Ne if prec <= Prec::Eq => bin!(self, lhs, Prec::Eq, BinaryOp::Ne),
                K::Lt if prec <= Prec::Rel => bin!(self, lhs, Prec::Rel, BinaryOp::Lt),
                K::Le if prec <= Prec::Rel => bin!(self, lhs, Prec::Rel, BinaryOp::Le),
                K::Gt if prec <= Prec::Rel => bin!(self, lhs, Prec::Rel, BinaryOp::Gt),
                K::Ge if prec <= Prec::Rel => bin!(self, lhs, Prec::Rel, BinaryOp::Ge),
                // << >> + - * / %
                K::Shl if prec <= Prec::Shift => bin!(self, lhs, Prec::Shift, BinaryOp::Shl),
                K::Shr if prec <= Prec::Shift => bin!(self, lhs, Prec::Shift, BinaryOp::Shr),
                K::Plus if prec <= Prec::Add => bin!(self, lhs, Prec::Add, BinaryOp::Add),
                K::Minus if prec <= Prec::Add => bin!(self, lhs, Prec::Add, BinaryOp::Sub),
                K::Star if prec <= Prec::Mul => bin!(self, lhs, Prec::Mul, BinaryOp::Mul),
                K::Slash if prec <= Prec::Mul => bin!(self, lhs, Prec::Mul, BinaryOp::Div),
                K::Percent if prec <= Prec::Mul => bin!(self, lhs, Prec::Mul, BinaryOp::Mod),
                _ => break,
            };
            lhs = expr;
        }
        Ok(lhs)
    }

    fn parse_postfix(&mut self) -> PResult<Expr> {
        let mut expr = self.parse_unary()?;
        loop {
            let t = match self.ts.peek(0) { Ok(t) => t.clone(), Err(e) => return Err(ParseError{ msg: e.msg, span: e.span }) };
            expr = match t.kind {
                K::LParen => { // call
                    let _ = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                    let mut args = Vec::new();
                    if !self.ts.check(K::RParen).map_err(|e| ParseError{ msg: e.msg, span: e.span })? {
                        loop {
                            // Arguments are assignment-expressions (no top-level comma operator)
                            args.push(self.parse_prec(Prec::Assign)?);
                            if self.ts.matches(K::Comma).map_err(|e| ParseError{ msg: e.msg, span: e.span })? { continue; }
                            break;
                        }
                    }
                    let r = self.ts.expect_kind(K::RParen).map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                    let span = join(expr.span(), r.span);
                    Expr::Call { callee: Box::new(expr), args, span }
                }
                K::LBracket => {
                    let _ = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                    let idx = self.parse_expr()?;
                    let r = self.ts.expect_kind(K::RBracket).map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                    let span = join(expr.span(), r.span);
                    Expr::Index { base: Box::new(expr), index: Box::new(idx), span }
                }
                K::Dot => { let _ = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let (name, sp) = self.parse_ident()?; let span = join(expr.span(), sp); Expr::Member { base: Box::new(expr), field: name, span } }
                K::Arrow => { let _ = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let (name, sp) = self.parse_ident()?; let span = join(expr.span(), sp); Expr::PtrMember { base: Box::new(expr), field: name, span } }
                K::Inc => { let _ = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let span = join(expr.span(), t.span); Expr::PostInc { expr: Box::new(expr), span } }
                K::Dec => { let _ = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let span = join(expr.span(), t.span); Expr::PostDec { expr: Box::new(expr), span } }
                _ => break,
            };
        }
        Ok(expr)
    }

    fn parse_unary(&mut self) -> PResult<Expr> {
        let t = match self.ts.peek(0) { Ok(t) => t.clone(), Err(e) => return Err(ParseError{ msg: e.msg, span: e.span }) };
        match t.kind {
            // sizeof
            K::Sizeof => {
                let tsz = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                if self.ts.matches(K::LParen).map_err(|e| ParseError{ msg: e.msg, span: e.span })? {
                    if self.is_type_start()? {
                        let ty = self.parse_type_name()?;
                        let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                        Ok(Expr::SizeOfType { ty, span: tsz.span })
                    } else {
                        let e = self.parse_expr()?;
                        let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                        Ok(Expr::SizeOfExpr { expr: Box::new(e), span: tsz.span })
                    }
                } else {
                    let e = self.parse_unary()?;
                    Ok(Expr::SizeOfExpr { expr: Box::new(e), span: tsz.span })
                }
            }
            // cast (type-name) unary
            K::LParen => {
                let _lp = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; // consume '('
                if self.is_type_start()? {
                    let ty = self.parse_type_name()?;
                    let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                    let e = self.parse_unary()?;
                    Ok(Expr::Cast { ty, expr: Box::new(e), span: _lp.span })
                } else {
                    // grouped expression
                    let e = self.parse_expr()?;
                    let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
                    Ok(e)
                }
            }
            // primaires
            K::Ident => { let (name, sp) = self.parse_ident()?; Ok(Expr::Ident(name, sp)) }
            K::IntLit => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; Ok(Expr::IntLit(t.lexeme, t.span)) }
            K::FloatLit => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; Ok(Expr::FloatLit(t.lexeme, t.span)) }
            K::CharLit => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; Ok(Expr::CharLit(t.lexeme, t.span)) }
            K::StrLit => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; Ok(Expr::StrLit(t.lexeme, t.span)) }
            // pré-unaires
            K::Inc => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let e = self.parse_unary()?; Ok(Expr::PreInc { expr: Box::new(e), span: t.span }) }
            K::Dec => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let e = self.parse_unary()?; Ok(Expr::PreDec { expr: Box::new(e), span: t.span }) }
            K::Plus => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let e = self.parse_unary()?; Ok(Expr::Unary { op: UnaryOp::Plus, expr: Box::new(e), span: t.span }) }
            K::Minus => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let e = self.parse_unary()?; Ok(Expr::Unary { op: UnaryOp::Minus, expr: Box::new(e), span: t.span }) }
            K::Tilde => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let e = self.parse_unary()?; Ok(Expr::Unary { op: UnaryOp::BitNot, expr: Box::new(e), span: t.span }) }
            K::Not => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let e = self.parse_unary()?; Ok(Expr::Unary { op: UnaryOp::LogNot, expr: Box::new(e), span: t.span }) }
            K::Amp => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let e = self.parse_unary()?; Ok(Expr::Unary { op: UnaryOp::AddrOf, expr: Box::new(e), span: t.span }) }
            K::Star => { let t = self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?; let e = self.parse_unary()?; Ok(Expr::Unary { op: UnaryOp::Deref, expr: Box::new(e), span: t.span }) }
            _ => Err(ParseError { msg: "expression attendue".into(), span: t.span }),
        }
    }

    pub(crate) fn parse_ident(&mut self) -> PResult<(String, Span)> {
        let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg, span: e.span })?;
        if t0.kind == K::Ident { let t = self.ts.bump().map_err(|e| ParseError { msg: e.msg, span: e.span })?; Ok((t.lexeme, t.span)) } else { Err(ParseError { msg: "identifiant attendu".into(), span: t0.span }) }
    }
}

fn join(a: Span, b: Span) -> Span { Span { lo: a.lo.min(b.lo), hi: a.hi.max(b.hi), line: a.line, col: a.col } }

macro_rules! bin {
    ($self:ident, $lhs:ident, $prec:expr, $op:expr) => {{
        let _ = $self.ts.bump().map_err(|e| ParseError{ msg: e.msg, span: e.span })?;
        let rhs = $self.parse_prec(($prec).next())?;
        let span = join($lhs.span(), rhs.span());
        Expr::Binary { op: $op, lhs: Box::new($lhs), rhs: Box::new(rhs), span }
    }};
}
use bin;
