// ----------------------------
// File: src/front/ast.rs
// ----------------------------
#![allow(dead_code)]
//! AST C89 minimal (déclarations, types, expressions, statements)

use crate::front::token::Span;

#[derive(Debug, Clone)]
pub struct NodeId(pub u32);

#[derive(Debug, Clone)]
pub struct WithSpan<T> { pub node: T, pub span: Span }

// Types C89 (simplifié)
#[derive(Debug, Clone)]
pub enum CType {
    Void,
    Char { signed: Option<bool> },
    Int { signed: Option<bool>, long: bool, short: bool }, // supporte short/long
    Float,
    Double { long: bool }, // long == true => long double
    // Struct/Union/Enum basiques
    Struct { tag: Option<String>, fields: Option<Vec<Decl>> },
    Union  { tag: Option<String>, fields: Option<Vec<Decl>> },
    Enum   { tag: Option<String>, items: Option<Vec<(String, Option<Expr>)>> },
    // Nom de type (typedef-name)
    Named(String),
    // Pointeurs, tableaux et fonctions via wrappers
    Pointer { of: Box<CType>, qualifiers: Vec<TypeQualifier> },
    Array { of: Box<CType>, size: Option<usize> },
    // Add 'prototype' to distinguish f(void) from f()
    Function { ret: Box<CType>, params: Vec<Param>, varargs: bool, prototype: bool },
}

#[derive(Debug, Clone)]
pub struct Param { pub name: Option<String>, pub ty: CType }

// Initializers (C89, sans designators)
#[derive(Debug, Clone)]
pub enum Initializer {
    Expr(Expr),
    List(Vec<Initializer>),
    Designated {
        designators: Vec<Designator>,
        init: Box<Initializer>,
    },
}

#[derive(Debug, Clone)]
pub enum Designator {
    Field(String),           // .field_name
    Index(Box<Expr>),       // [expression]
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum StorageClass { Typedef, Extern, Static, Auto, Register }

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TypeQualifier { Const, Volatile }

// Déclarations au niveau bloc/fichier
#[derive(Debug, Clone)]
pub struct Decl {
    pub name: String,
    pub ty: CType,
    pub init: Option<Initializer>,
    pub storage: Option<StorageClass>,
    pub qualifiers: Vec<TypeQualifier>,
    pub bit_width: Option<Expr>, // pour struct bit-fields (None sinon)
    pub span: Span,
}

#[derive(Debug, Clone)]
pub struct FuncDef {
    pub name: String,
    pub ty: CType,               // Function
    pub params: Vec<Param>,      // recopie pour commodité
    pub body: Stmt,              // Compound
    pub span: Span,
}

#[derive(Debug, Clone)]
pub enum Top {
    Decl(Decl),
    Decls(Vec<Decl>),
    Func(FuncDef),
    // Déclaration de tag seule: struct/union/enum avec ou sans définition, sans déclarateurs
    Tag(CType, Span),
}

#[derive(Debug, Clone)]
pub struct TranslationUnit { pub items: Vec<Top> }

// Expressions
#[derive(Debug, Clone)]
pub enum Expr {
    Ident(String, Span),
    IntLit(String, Span),
    FloatLit(String, Span),
    CharLit(String, Span),
    StrLit(String, Span),

    // Postfix
    Call { callee: Box<Expr>, args: Vec<Expr>, span: Span },
    Index { base: Box<Expr>, index: Box<Expr>, span: Span },
    Member { base: Box<Expr>, field: String, span: Span },
    PtrMember { base: Box<Expr>, field: String, span: Span },
    PostInc { expr: Box<Expr>, span: Span },
    PostDec { expr: Box<Expr>, span: Span },

    // Unaires
    PreInc { expr: Box<Expr>, span: Span },
    PreDec { expr: Box<Expr>, span: Span },
    Unary { op: UnaryOp, expr: Box<Expr>, span: Span },

    // Binaires
    Binary { op: BinaryOp, lhs: Box<Expr>, rhs: Box<Expr>, span: Span },

    // Ternary
    Cond { c: Box<Expr>, t: Box<Expr>, e: Box<Expr>, span: Span },

    // Assignations
    Assign { op: AssignOp, lhs: Box<Expr>, rhs: Box<Expr>, span: Span },

    // Casts (C89)
    Cast { ty: CType, expr: Box<Expr>, span: Span },

    // sizeof
    SizeOfExpr { expr: Box<Expr>, span: Span },
    SizeOfType { ty: CType, span: Span },
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum UnaryOp { Plus, Minus, BitNot, LogNot, AddrOf, Deref }

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum BinaryOp {
    Mul, Div, Mod,
    Add, Sub,
    Shl, Shr,
    Lt, Gt, Le, Ge,
    Eq, Ne,
    BitAnd, BitXor, BitOr,
    LogAnd, LogOr,
    Comma,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum AssignOp { Assign, Mul, Div, Mod, Add, Sub, Shl, Shr, And, Xor, Or }

// Statements
#[derive(Debug, Clone)]
pub enum Stmt {
    Empty(Span),
    Expr(Expr, Span),
    Decl(Decl),
    Compound { items: Vec<BlockItem>, span: Span },
    If { c: Expr, t: Box<Stmt>, e: Option<Box<Stmt>>, span: Span },
    While { c: Expr, body: Box<Stmt>, span: Span },
    DoWhile { c: Expr, body: Box<Stmt>, span: Span },
    For { init: Option<Box<Stmt>>, c: Option<Expr>, post: Option<Expr>, body: Box<Stmt>, span: Span },
    Switch { expr: Expr, body: Box<Stmt>, span: Span },
    Case { expr: Expr, stmt: Box<Stmt>, span: Span },
    Default { stmt: Box<Stmt>, span: Span },
    Return(Option<Expr>, Span),
    Break(Span),
    Continue(Span),
    Label { name: String, stmt: Box<Stmt>, span: Span },
    Goto { name: String, span: Span },
}

#[derive(Debug, Clone)]
pub enum BlockItem { Stmt(Stmt), Decl(Decl) }

// Helpers
impl Expr {
    pub fn span(&self) -> Span {
        match self {
            Expr::Ident(_, s)
            | Expr::IntLit(_, s)
            | Expr::FloatLit(_, s)
            | Expr::CharLit(_, s)
            | Expr::StrLit(_, s)
            | Expr::Call { span: s, .. }
            | Expr::Index { span: s, .. }
            | Expr::Member { span: s, .. }
            | Expr::PtrMember { span: s, .. }
            | Expr::PostInc { span: s, .. }
            | Expr::PostDec { span: s, .. }
            | Expr::PreInc { span: s, .. }
            | Expr::PreDec { span: s, .. }
            | Expr::Unary { span: s, .. }
            | Expr::Binary { span: s, .. }
            | Expr::Cond { span: s, .. }
            | Expr::Assign { span: s, .. }
            | Expr::Cast { span: s, .. }
            | Expr::SizeOfExpr { span: s, .. }
            | Expr::SizeOfType { span: s, .. }
            => *s,
        }
    }
}
