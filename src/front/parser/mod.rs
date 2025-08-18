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
use std::fmt;

#[derive(Debug, Clone)]
pub struct ParseError { 
    pub msg: String, 
    pub span: Span,
    pub is_fatal: bool,
}

impl ParseError {
    pub fn new(span: Span, msg: impl Into<String>) -> Self {
        Self { 
            msg: msg.into(), 
            span, 
            is_fatal: false 
        }
    }
    
    pub fn fatal(span: Span, msg: impl Into<String>) -> Self {
        Self { 
            msg: msg.into(), 
            span, 
            is_fatal: true 
        }
    }
}

impl fmt::Display for ParseError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.msg)
    }
}

// Compatibility constructor for old-style initialization  
impl From<(String, Span)> for ParseError {
    fn from((msg, span): (String, Span)) -> Self {
        Self { msg, span, is_fatal: false }
    }
}

/// Error recovery system for robust parsing
#[derive(Debug, Clone)]
pub struct ErrorRecovery {
    pub continue_on_error: bool,
    pub max_errors: usize,
    pub error_count: usize,
    pub recovered_errors: Vec<ParseError>,
}

impl ErrorRecovery {
    pub fn new(continue_on_error: bool, max_errors: usize) -> Self {
        Self {
            continue_on_error,
            max_errors,
            error_count: 0,
            recovered_errors: Vec::new(),
        }
    }
    
    pub fn report_error(&mut self, error: ParseError) -> bool {
        self.error_count += 1;
        
        if error.is_fatal || !self.continue_on_error || self.error_count >= self.max_errors {
            return false; // Stop parsing
        }
        
        self.recovered_errors.push(error);
        true // Continue parsing
    }
}

pub type PResult<T> = Result<T, ParseError>;

pub struct Parser<'a> {
    pub ts: TokenStream<'a>,
    // environnement de typedef-names par scope
    typedefs: Vec<HashSet<String>>, // pile de scopes
    // environnement de tags (struct/union/enum) par scope
    struct_tags: Vec<HashSet<String>>,
    union_tags: Vec<HashSet<String>>,
    enum_tags: Vec<HashSet<String>>,
    // error recovery system
    error_recovery: Option<ErrorRecovery>,
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
            error_recovery: None,
        }
    }
    
    pub fn new_with_recovery(src: &'a str, file: &str, error_recovery: ErrorRecovery) -> PResult<Self> {
        let lexer = Lexer::new(src, file);
        Ok(Self { 
            ts: TokenStream::new(lexer), 
            typedefs: vec![HashSet::new()],
            struct_tags: vec![HashSet::new()],
            union_tags: vec![HashSet::new()],
            enum_tags: vec![HashSet::new()],
            error_recovery: Some(error_recovery),
        })
    }
    
    pub fn has_recovered_errors(&self) -> bool {
        self.error_recovery.as_ref()
            .map(|er| !er.recovered_errors.is_empty())
            .unwrap_or(false)
    }

    #[inline]
    fn error<T>(&self, span: Span, msg: impl Into<String>) -> PResult<T> { 
        Err(ParseError::new(span, msg)) 
    }

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
            let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?;
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
            let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?;
            if t0.kind == K::Eof { break; }
            // ignorer les ';' vides éventuels
            while self.ts.matches(K::Semicolon).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })? {}
            let t0 = self.ts.peek(0).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?;
            if t0.kind == K::Eof { break; }
            let e = self.parse_expr()?;
            // requiert ';'
            let _ = self.ts.expect_kind(K::Semicolon).map_err(|e| ParseError { msg: e.msg.clone(), span: e.span, is_fatal: false })?;
            out.push(e);
        }
        Ok(out)
    }
}
