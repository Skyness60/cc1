// ----------------------------
// File: src/front/parser/mod.rs
// ----------------------------
#![allow(dead_code, unused_imports)]
//! Squelette du parser C89

mod stream;
mod expr;
mod decl;
mod stmt;

pub use stream::TokenStream;

use crate::front::ast::*;
use crate::front::token::{Token, TokenKind as K, LexError, Span};
use crate::front::lexer::Lexer;
use std::collections::HashSet;

#[derive(Debug)]
pub struct ParseError { pub msg: String, pub span: Span }

pub type PResult<T> = Result<T, ParseError>;

pub struct Parser<'a> {
    pub ts: TokenStream<'a>,
    // environnement de typedef-names par scope
    typedefs: Vec<HashSet<String>>, // pile de scopes
    // environnement de tags (struct/union/enum) par scope
    struct_tags: Vec<HashSet<String>>,
    union_tags: Vec<HashSet<String>>,
    enum_tags: Vec<HashSet<String>>,
}

impl<'a> Parser<'a> {
    pub fn new(src: &'a str, file: &str) -> Self {
        let lexer = Lexer::new(src, file);
        Self { 
            ts: TokenStream::new(lexer), 
            typedefs: vec![HashSet::new()],
            struct_tags: vec![HashSet::new()],
            union_tags: vec![HashSet::new()],
            enum_tags: vec![HashSet::new()],
        }
    }

    #[inline]
    fn error<T>(&self, span: Span, msg: impl Into<String>) -> PResult<T> { Err(ParseError { msg: msg.into(), span }) }

    // typedef env helpers
    pub(crate) fn push_scope(&mut self) { 
        self.typedefs.push(HashSet::new());
        self.struct_tags.push(HashSet::new());
        self.union_tags.push(HashSet::new());
        self.enum_tags.push(HashSet::new());
    }
    
    pub(crate) fn pop_scope(&mut self) { 
        self.typedefs.pop(); 
        self.struct_tags.pop();
        self.union_tags.pop();
        self.enum_tags.pop();
        if self.typedefs.is_empty() { 
            self.typedefs.push(HashSet::new());
            self.struct_tags.push(HashSet::new());
            self.union_tags.push(HashSet::new());
            self.enum_tags.push(HashSet::new());
        }
    }
    
    pub(crate) fn add_typedef(&mut self, name: &str) { if let Some(top) = self.typedefs.last_mut() { top.insert(name.to_string()); } }
    pub(crate) fn is_typedef_name(&mut self, name: &str) -> bool { self.typedefs.iter().rev().any(|s| s.contains(name)) }
    
    // tag env helpers
    pub(crate) fn add_struct_tag(&mut self, name: &str) { if let Some(top) = self.struct_tags.last_mut() { top.insert(name.to_string()); } }
    pub(crate) fn add_union_tag(&mut self, name: &str) { if let Some(top) = self.union_tags.last_mut() { top.insert(name.to_string()); } }
    pub(crate) fn add_enum_tag(&mut self, name: &str) { if let Some(top) = self.enum_tags.last_mut() { top.insert(name.to_string()); } }
    
    pub(crate) fn is_struct_tag(&self, name: &str) -> bool { self.struct_tags.iter().rev().any(|s| s.contains(name)) }
    pub(crate) fn is_union_tag(&self, name: &str) -> bool { self.union_tags.iter().rev().any(|s| s.contains(name)) }
    pub(crate) fn is_enum_tag(&self, name: &str) -> bool { self.enum_tags.iter().rev().any(|s| s.contains(name)) }

    pub fn parse_translation_unit(&mut self) -> PResult<TranslationUnit> {
        let mut items = Vec::new();
        loop {
            let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span })?;
            if t0.kind == K::Eof { break; }
            let top = self.parse_top()?;
            items.push(top);
        }
        Ok(TranslationUnit { items })
    }

    // Debug: parser une suite d’expressions terminées par ';' jusqu’à EOF
    pub fn parse_exprs_semi(&mut self) -> PResult<Vec<Expr>> {
        let mut out = Vec::new();
        loop {
            let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span })?;
            if t0.kind == K::Eof { break; }
            // ignorer les ';' vides éventuels
            while self.ts.matches(K::Semicolon).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span })? {}
            let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span })?;
            if t0.kind == K::Eof { break; }
            let e = self.parse_expr()?;
            // requiert ';'
            let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span })?;
            out.push(e);
        }
        Ok(out)
    }
}
