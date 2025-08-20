// ----------------------------
// File: src/front/parser/stmt.rs
// ----------------------------
//! Parsing statements: bloc, return, expression, if/else, while, do/while, for,
//! break/continue, labels, goto, et déclarations locales.

use super::*;
use crate::front::token::TokenKind as K;

impl<'a> Parser<'a> {
    pub fn parse_stmt(&mut self) -> PResult<Stmt> {
        let t0 = self.ts.peek(0).map_err(|e| ParseError::new(e.span , e.msg))?;
        match t0.kind {
            K::LBrace => self.parse_compound_stmt(),
            K::Return => {
                let t = self.ts.bump().map_err(|e| ParseError::new(e.span , e.msg))?;
                if self.ts.check(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))? {
                    let _ = self.ts.bump();
                    Ok(Stmt::Return(None, t.span))
                } else {
                    let e = self.parse_expr()?;
                    let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))?;
                    Ok(Stmt::Return(Some(e), t.span))
                }
            }
            K::If => self.parse_if_stmt(),
            K::While => self.parse_while_stmt(),
            K::Do => self.parse_do_while_stmt(),
            K::For => self.parse_for_stmt(),
            K::Switch => self.parse_switch_stmt(),
            K::Case => self.parse_case_stmt(),
            K::Default => self.parse_default_stmt(),
            K::Break => { let t = self.ts.bump().map_err(|e| ParseError::new(e.span , e.msg))?; let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))?; Ok(Stmt::Break(t.span)) }
            K::Continue => { let t = self.ts.bump().map_err(|e| ParseError::new(e.span , e.msg))?; let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))?; Ok(Stmt::Continue(t.span)) }
            K::Goto => { let t = self.ts.bump().map_err(|e| ParseError::new(e.span , e.msg))?; let (name, _) = self.parse_ident()?; let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))?; Ok(Stmt::Goto { name, span: t.span }) }
            K::Ident => {
                // label: ident : stmt (lookahead)
                let t1 = self.ts.peek(1).map_err(|e| ParseError::new(e.span , e.msg))?;
                if t1.kind == K::Colon {
                    let (name, sp) = self.parse_ident()?; // consume ident
                    let _ = self.ts.expect_kind(K::Colon).map_err(|e| ParseError::new(e.span , e.msg))?;
                    let stmt = self.parse_stmt()?;
                    Ok(Stmt::Label { name, stmt: Box::new(stmt), span: sp })
                } else {
                    // expression-statement générique
                    let e = self.parse_expr()?;
                    let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))?;
                    let sp = e.span();
                    Ok(Stmt::Expr(e, sp))
                }
            }
            K::Semicolon => { let t = self.ts.bump().map_err(|e| ParseError::new(e.span , e.msg))?; Ok(Stmt::Empty(t.span)) }
            _ => {
                let e = self.parse_expr()?;
                let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))?;
                let sp = e.span();
                Ok(Stmt::Expr(e, sp))
            }
        }
    }

    pub fn parse_compound_stmt(&mut self) -> PResult<Stmt> {
        let l = self.ts.expect_kind(K::LBrace).map_err(|e| ParseError::new(e.span , e.msg))?;
        self.push_scope();
        let mut items = Vec::new();
        let mut saw_stmt = false; // C89: déclarations avant statements
        while !self.ts.check(K::RBrace).map_err(|e| ParseError::new(e.span , e.msg))? {
            if self.is_type_start()? {
                if saw_stmt {
                    // capture le span puis libère l'emprunt avant pop_scope()
                    let err_span = { let t = self.ts.peek(0).map_err(|e| ParseError::new(e.span , e.msg))?; t.span };
                    self.pop_scope();
                    return Err(ParseError { msg: "en C89, les déclarations doivent précéder les statements dans un bloc".into(), span: err_span, is_fatal: false });
                }
                // Support multi-déclarateurs dans les blocs
                let decls = self.parse_declaration_many()?;
                for d in decls.into_iter() {
                    if matches!(d.storage, Some(StorageClass::Typedef)) { self.add_typedef(&d.name); }
                    items.push(BlockItem::Decl(d));
                }
            } else {
                let s = self.parse_stmt()?;
                items.push(BlockItem::Stmt(s));
                saw_stmt = true;
            }
        }
        let r = self.ts.expect_kind(K::RBrace).map_err(|e| ParseError::new(e.span , e.msg))?;
        self.pop_scope();
        Ok(Stmt::Compound { items, span: Span { lo: l.span.lo, hi: r.span.hi, line: l.span.line, col: l.span.col } })
    }

    fn parse_if_stmt(&mut self) -> PResult<Stmt> {
        let t_if = self.ts.expect_kind(K::If).map_err(|e| ParseError::new(e.span , e.msg))?;
        let _ = self.ts.expect_kind(K::LParen).map_err(|e| ParseError::new(e.span , e.msg))?;
        let c = self.parse_expr()?;
        let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError::new(e.span , e.msg))?;
        let t_stmt = self.parse_stmt()?;
        let e_stmt = if self.ts.matches(K::Else).map_err(|e| ParseError::new(e.span , e.msg))? { Some(Box::new(self.parse_stmt()?)) } else { None };
        Ok(Stmt::If { c, t: Box::new(t_stmt), e: e_stmt, span: t_if.span })
    }

    fn parse_while_stmt(&mut self) -> PResult<Stmt> {
        let t = self.ts.expect_kind(K::While).map_err(|e| ParseError::new(e.span , e.msg))?;
        let _ = self.ts.expect_kind(K::LParen).map_err(|e| ParseError::new(e.span , e.msg))?;
        let c = self.parse_expr()?;
        let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError::new(e.span , e.msg))?;
        let body = self.parse_stmt()?;
        Ok(Stmt::While { c, body: Box::new(body), span: t.span })
    }

    fn parse_do_while_stmt(&mut self) -> PResult<Stmt> {
        let t = self.ts.expect_kind(K::Do).map_err(|e| ParseError::new(e.span , e.msg))?;
        let body = self.parse_stmt()?;
        let _ = self.ts.expect_kind(K::While).map_err(|e| ParseError::new(e.span , e.msg))?;
        let _ = self.ts.expect_kind(K::LParen).map_err(|e| ParseError::new(e.span , e.msg))?;
        let c = self.parse_expr()?;
        let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError::new(e.span , e.msg))?;
        let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))?;
        Ok(Stmt::DoWhile { c, body: Box::new(body), span: t.span })
    }

    fn parse_for_stmt(&mut self) -> PResult<Stmt> {
        let t = self.ts.expect_kind(K::For).map_err(|e| ParseError::new(e.span , e.msg))?;
        let _ = self.ts.expect_kind(K::LParen).map_err(|e| ParseError::new(e.span , e.msg))?;
        // init: C89 n'autorise pas une déclaration ici
        if self.is_type_start()? {
            let t0 = self.ts.peek(0).map_err(|e| ParseError::new(e.span , e.msg))?;
            return Err(ParseError::new(t0.span , "déclaration dans 'for' non supportée en C89 (C99+)"));
        }
        let init_stmt = if self.ts.matches(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))? {
            None
        } else {
            let e = self.parse_expr()?;
            let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))?;
            Some(Box::new(Stmt::Expr(e.clone(), e.span())))
        };
        // condition optionnelle + ';'
        let cond = if self.ts.matches(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))? { None } else { let e = self.parse_expr()?; let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError::new(e.span , e.msg))?; Some(e) };
        // post optionnel + ')'
        let post = if self.ts.matches(K::RParen).map_err(|e| ParseError::new(e.span , e.msg))? { None } else { let e = self.parse_expr()?; let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError::new(e.span , e.msg))?; Some(e) };
        let body = self.parse_stmt()?;
        Ok(Stmt::For { init: init_stmt, c: cond, post, body: Box::new(body), span: t.span })
    }

    fn parse_switch_stmt(&mut self) -> PResult<Stmt> {
        let t = self.ts.expect_kind(K::Switch).map_err(|e| ParseError::new(e.span , e.msg))?;
        let _ = self.ts.expect_kind(K::LParen).map_err(|e| ParseError::new(e.span , e.msg))?;
        let e = self.parse_expr()?;
        let _ = self.ts.expect_kind(K::RParen).map_err(|e| ParseError::new(e.span , e.msg))?;
        let body = self.parse_stmt()?;
        Ok(Stmt::Switch { expr: e, body: Box::new(body), span: t.span })
    }

    fn parse_case_stmt(&mut self) -> PResult<Stmt> {
        let t = self.ts.expect_kind(K::Case).map_err(|e| ParseError::new(e.span , e.msg))?;
        let e = self.parse_expr()?;
        let _ = self.ts.expect_kind(K::Colon).map_err(|e| ParseError::new(e.span , e.msg))?;
        let s = self.parse_stmt()?;
        Ok(Stmt::Case { expr: e, stmt: Box::new(s), span: t.span })
    }

    fn parse_default_stmt(&mut self) -> PResult<Stmt> {
        let t = self.ts.expect_kind(K::Default).map_err(|e| ParseError::new(e.span , e.msg))?;
        let _ = self.ts.expect_kind(K::Colon).map_err(|e| ParseError::new(e.span , e.msg))?;
        let s = self.parse_stmt()?;
        Ok(Stmt::Default { stmt: Box::new(s), span: t.span })
    }

    // Parse une expression à partir d’un identifiant déjà lu (pour gérer le cas label/expr)
    fn parse_expr_from_prefix(&mut self, first: Expr) -> PResult<Expr> {
        // On triche: on ne réinjecte pas le token, on permet seulement l’expression réduite à l’ident (sans postfix/binaire) pour simplicité.
        // Pour une vraie implémentation, il faudrait un buffer/token unget ou une sous-API.
        Ok(first)
    }
}
