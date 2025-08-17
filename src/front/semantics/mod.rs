// ----------------------------
// File: src/front/semantics/mod.rs
// ----------------------------
#![allow(dead_code)]
//! Semantic analysis scaffolding for C89: symbol tables, const-eval, and basic type checks.

use crate::front::ast::*;
use crate::front::token::Span;

mod const_eval;
pub use const_eval::{eval_ice, eval_ice_with_env};
mod target;
pub use target::{TargetInfo, Arch};

#[derive(Debug, Clone)]
pub struct Diag { pub span: Span, pub msg: String }

#[derive(Debug, Default)]
pub struct Diagnostics { pub errors: Vec<Diag> }

impl Diagnostics {
    pub fn error<S: Into<String>>(&mut self, span: Span, msg: S) { self.errors.push(Diag { span, msg: msg.into() }); }
    pub fn is_empty(&self) -> bool { self.errors.is_empty() }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum SymKind { Object, Function, Typedef, EnumConst }

#[derive(Debug, Clone)]
pub struct Symbol {
    pub name: String,
    pub kind: SymKind,
    pub ty: Option<CType>,
    pub span: Span,
    pub const_value: Option<i64>, // For enum constants
}

#[derive(Default)]
pub struct Scope { pub syms: std::collections::HashMap<String, Symbol> }

#[derive(Default)]
pub struct TagScope { pub tags: std::collections::HashMap<String, CType> }

pub struct SemCtx {
    scopes: Vec<Scope>,
    tag_scopes: Vec<TagScope>,
    pub diags: Diagnostics,
    pub ti: TargetInfo,
}

impl SemCtx {
    pub fn new() -> Self { 
        Self { scopes: vec![Scope::default()], tag_scopes: vec![TagScope::default()], diags: Diagnostics::default(), ti: TargetInfo::i386() }
    }

    pub fn push_scope(&mut self) { self.scopes.push(Scope::default()); }
    pub fn pop_scope(&mut self) { self.scopes.pop(); }
    pub fn push_tag_scope(&mut self) { self.tag_scopes.push(TagScope::default()); }
    pub fn pop_tag_scope(&mut self) { self.tag_scopes.pop(); }

    pub fn declare(&mut self, sym: Symbol) {
        let scope = if let Some(scope) = self.scopes.last_mut() {
            scope
        } else {
            // Emergency: create global scope if somehow missing
            self.scopes.push(Scope::default());
            self.scopes.last_mut().expect("scope just pushed exists")
        };
        if let std::collections::hash_map::Entry::Occupied(_) = scope.syms.entry(sym.name.clone()) {
            self.diags.error(sym.span, "redefinition of symbol in the same scope");
        } else {
            scope.syms.insert(sym.name.clone(), sym);
        }
    }

    pub fn declare_tag(&mut self, name: &str, ty: &CType) {
        if let Some(scope) = self.tag_scopes.last_mut() { scope.tags.insert(name.to_string(), ty.clone()); }
    }

    pub fn lookup_tag(&self, name: &str) -> Option<CType> {
        for scope in self.tag_scopes.iter().rev() {
            if let Some(t) = scope.tags.get(name) { return Some(t.clone()); }
        }
        None
    }

    pub fn lookup(&self, name: &str) -> Option<&Symbol> {
        for scope in self.scopes.iter().rev() {
            if let Some(s) = scope.syms.get(name) { return Some(s); }
        }
        None
    }

    fn resolve_typedef(&self, name: &str) -> Option<CType> {
        for scope in self.scopes.iter().rev() {
            if let Some(sym) = scope.syms.get(name) { if matches!(sym.kind, SymKind::Typedef) { return sym.ty.clone(); } }
        }
        None
    }

    fn resolve_type(&self, ty: &CType) -> CType {
        match ty {
            CType::Named(n) => { if let Some(t) = self.resolve_typedef(n) { self.resolve_type(&t) } else { ty.clone() } }
            CType::Struct { tag: Some(n), fields: None } => { if let Some(t) = self.lookup_tag(n) { self.resolve_type(&t) } else { ty.clone() } }
            CType::Union { tag: Some(n), fields: None } => { if let Some(t) = self.lookup_tag(n) { self.resolve_type(&t) } else { ty.clone() } }
            CType::Enum { tag: Some(n), items: None } => { if let Some(t) = self.lookup_tag(n) { self.resolve_type(&t) } else { ty.clone() } }
            CType::Pointer { of, qualifiers } => CType::Pointer { of: Box::new(self.resolve_type(of)), qualifiers: qualifiers.clone() },
            CType::Array { of, size } => CType::Array { of: Box::new(self.resolve_type(of)), size: *size },
            CType::Function { ret, params, varargs, prototype } => CType::Function { ret: Box::new(self.resolve_type(ret)), params: params.clone(), varargs: *varargs, prototype: *prototype },
            _ => ty.clone(),
        }
    }

    fn sizeof_type(&self, ty: &CType) -> usize {
        let r = self.resolve_type(ty);
        self.ti.sizeof(&r).unwrap_or(0)
    }
}

// --- Expression typing helpers ---
#[derive(Debug, Clone)]
struct TyVal { ty: CType, lvalue: bool }

impl TyVal { fn rvalue(mut self) -> Self { self.lvalue = false; self } }

fn is_integer(ty: &CType) -> bool { matches!(ty, CType::Char{..} | CType::Int{..} | CType::Enum{..}) }
fn is_floating(ty: &CType) -> bool { matches!(ty, CType::Float | CType::Double{..}) }
fn is_arith(ty: &CType) -> bool { is_integer(ty) || is_floating(ty) }
fn is_pointer(ty: &CType) -> bool { matches!(ty, CType::Pointer{..}) }
fn is_void(ty: &CType) -> bool { matches!(ty, CType::Void) }
fn is_array(ty: &CType) -> bool { matches!(ty, CType::Array{..}) }
fn is_function(ty: &CType) -> bool { matches!(ty, CType::Function{..}) }

fn is_char_type(ty: &CType) -> bool { matches!(ty, CType::Char{..}) }

fn ptr_to(t: CType) -> CType { CType::Pointer { of: Box::new(t), qualifiers: Vec::new() } }

fn integer_promote(_ti: &TargetInfo, ty: &CType) -> CType {
    match ty {
        CType::Char{..} | CType::Int{ short: true, .. } => CType::Int { signed: Some(true), long: false, short: false },
        CType::Enum{..} => CType::Int { signed: Some(true), long: false, short: false },
        _ => ty.clone(),
    }
}

fn default_arg_promotion(ti: &TargetInfo, ty: &CType) -> CType {
    match ty { CType::Float => CType::Double { long: false }, _ => integer_promote(ti, ty) }
}

fn arg_decay_to_rvalue(tv: TyVal) -> TyVal { decay_if_needed(tv).rvalue() }

fn adjust_param_type(ty: &CType) -> CType {
    match ty { CType::Array { of, .. } => ptr_to((**of).clone()), CType::Function { .. } => ptr_to(ty.clone()), _ => ty.clone() }
}

fn is_object_pointer(ty: &CType) -> bool { if let CType::Pointer { of, .. } = ty { !matches!(**of, CType::Function { .. }) } else { false } }

pub struct SemanticAnalyzer<'a> { pub ctx: SemCtx, input: &'a TranslationUnit, current_ret: Option<CType> }

impl<'a> SemanticAnalyzer<'a> {
    pub fn new(input: &'a TranslationUnit) -> Self { Self { ctx: SemCtx::new(), input, current_ret: None } }

    pub fn analyze(mut self) -> Diagnostics { for item in &self.input.items { self.visit_top(item); } self.ctx.diags }

    fn visit_top(&mut self, item: &Top) {
        match item {
            Top::Decl(d) => self.visit_decl(d),
            Top::Decls(ds) => for d in ds { self.visit_decl(d); },
            Top::Func(f) => self.visit_func(f),
            Top::Tag(ty, _) => {
                match ty { CType::Struct { tag: Some(n), .. } | CType::Union { tag: Some(n), .. } | CType::Enum { tag: Some(n), .. } => self.ctx.declare_tag(n, ty), _ => {} }
                // Also process type definitions (e.g., enum constants)
                self.process_type_defs(ty);
            }
        }
    }

    fn visit_func(&mut self, f: &FuncDef) {
        self.ctx.declare(Symbol { name: f.name.clone(), kind: SymKind::Function, ty: Some(f.ty.clone()), span: f.span, const_value: None });
        let ret_ty = if let CType::Function{ ret, .. } = &f.ty { *ret.clone() } else { CType::Int { signed: Some(true), long: false, short: false } };
        self.current_ret = Some(ret_ty);
        self.ctx.push_scope();
        for p in &f.params { if let Some(n) = &p.name { self.ctx.declare(Symbol { name: n.clone(), kind: SymKind::Object, ty: Some(p.ty.clone()), span: f.span, const_value: None }); } }
        self.visit_stmt(&f.body);
        self.ctx.pop_scope();
        self.current_ret = None;
    }

    fn visit_decl(&mut self, d: &Decl) {
        let kind = match d.storage { Some(StorageClass::Typedef) => SymKind::Typedef, _ => SymKind::Object };
        if matches!(d.storage, Some(StorageClass::Extern)) && d.init.is_some() { self.ctx.diags.error(d.span, "extern declaration cannot have an initializer"); }
        let inferred_ty = if let Some(init) = &d.init { self.infer_array_size(&d.ty, init) } else { self.ctx.resolve_type(&d.ty) };
        self.check_zero_sized_arrays(&inferred_ty, d.span);
        self.ctx.declare(Symbol { name: d.name.clone(), kind, ty: Some(inferred_ty.clone()), span: d.span, const_value: None });
        if let Some(init) = &d.init {
            self.check_initializer(&inferred_ty, init, d.span);
            if self.has_static_storage_duration(d) { if let Err(msg) = self.require_const_initializer(init) { self.ctx.diags.error(d.span, format!("static-storage object requires constant initializer: {}", msg)); } }
        }
        self.check_bit_fields_in_type(&inferred_ty);
    }

    fn visit_stmt(&mut self, s: &Stmt) {
        match s {
            Stmt::Empty(_) | Stmt::Break(_) | Stmt::Continue(_) | Stmt::Goto { .. } => {}
            Stmt::Return(eopt, sp) => {
                if let Some(e) = eopt {
                    let tv = self.ty_of_expr(e);
                    if let Some(ret) = &self.current_ret {
                        if is_void(ret) { self.ctx.diags.error(*sp, "return with a value in function returning void"); }
                        else if !self.can_assign(ret, &tv.ty) { self.ctx.diags.error(*sp, "incompatible return type"); }
                    }
                } else {
                    if let Some(ret) = &self.current_ret { if !is_void(ret) { self.ctx.diags.error(*sp, "return with no value in function returning non-void"); } }
                }
            }
            Stmt::Expr(e, _) => { let _ = self.ty_of_expr(e); }
            Stmt::Decl(d) => { self.process_type_defs(&d.ty); self.visit_decl(d); }
            Stmt::Compound { items, .. } => { self.ctx.push_scope(); for it in items { match it { BlockItem::Stmt(ss) => self.visit_stmt(ss), BlockItem::Decl(dd) => { self.process_type_defs(&dd.ty); self.visit_decl(dd); } } } self.ctx.pop_scope(); }
            Stmt::If { c, t, e, .. } => { let ct = self.ty_of_expr(c); if !is_arith(&ct.ty) && !is_pointer(&ct.ty) { self.ctx.diags.error(c.span(), "condition must be scalar"); } self.visit_stmt(t); if let Some(ee) = e { self.visit_stmt(ee); } }
            Stmt::While { c, body, .. } => { let ct = self.ty_of_expr(c); if !is_arith(&ct.ty) && !is_pointer(&ct.ty) { self.ctx.diags.error(c.span(), "condition must be scalar"); } self.visit_stmt(body); }
            Stmt::DoWhile { c, body, .. } => { self.visit_stmt(body); let ct = self.ty_of_expr(c); if !is_arith(&ct.ty) && !is_pointer(&ct.ty) { self.ctx.diags.error(c.span(), "condition must be scalar"); } }
            Stmt::For { init, c, post, body, .. } => { if let Some(i) = init { self.visit_stmt(i); } if let Some(cc) = c { let ct = self.ty_of_expr(cc); if !is_arith(&ct.ty) && !is_pointer(&ct.ty) { self.ctx.diags.error(cc.span(), "condition must be scalar"); } } if let Some(p) = post { let _ = self.ty_of_expr(p); } self.visit_stmt(body); }
            Stmt::Switch { expr, body, .. } => { let ct = self.ty_of_expr(expr); if !is_integer(&ct.ty) { self.ctx.diags.error(expr.span(), "switch requires integer"); } self.visit_stmt(body); }
            Stmt::Case { expr, stmt, span } => { let sz = |ty: &CType| self.ctx.sizeof_type(ty); if let Err(msg) = eval_ice(expr, &sz) { self.ctx.diags.error(*span, format!("case label is not a constant expression: {}", msg)); } self.visit_stmt(stmt); }
            Stmt::Default { stmt, .. } => { self.visit_stmt(stmt); }
            Stmt::Label { stmt, .. } => { self.visit_stmt(stmt); }
        }
    }

    // --- Typing core ---
    fn ty_of_expr(&mut self, e: &Expr) -> TyVal {
        match e {
            Expr::Ident(name, sp) => {
                if let Some(sym) = self.ctx.lookup(name) {
                    let ty = self.ctx.resolve_type(sym.ty.as_ref().unwrap());
                    let lv = !is_function(&ty) && !is_array(&ty);
                    TyVal { ty, lvalue: lv }
                } else { self.ctx.diags.error(*sp, format!("use of undeclared identifier '{}'", name)); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
            }
            Expr::IntLit(_, _) | Expr::CharLit(_, _) => TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false },
            Expr::FloatLit(_, _) => TyVal { ty: CType::Double { long: false }, lvalue: false },
            Expr::StrLit(_, _) => TyVal { ty: ptr_to(CType::Char { signed: None }), lvalue: false },

            Expr::Call { callee, args, span } => {
                let cal = decay_if_needed(self.ty_of_expr(callee));
                let (ret, params, varargs, proto) = match &cal.ty {
                    CType::Pointer { of, .. } => match &**of {
                        CType::Function { ret, params, varargs, prototype } => ((*ret).clone(), params.clone(), *varargs, *prototype),
                        _ => { self.ctx.diags.error(*span, "call of non-function"); return TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                    },
                    CType::Function { ret, params, varargs, prototype } => ((*ret).clone(), params.clone(), *varargs, *prototype),
                    _ => { self.ctx.diags.error(*span, "call of non-function"); return TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                };
                if !proto {
                    // Old-style declaration (no prototype): don't enforce arity; apply default promotions to all args
                    for a in args { let at = arg_decay_to_rvalue(self.ty_of_expr(a)); let _ = default_arg_promotion(&self.ctx.ti, &at.ty); }
                } else {
                    // Prototyped function
                    if varargs { if args.len() < params.len() { self.ctx.diags.error(*span, "too few arguments for function call"); } }
                    else { if !params.is_empty() && args.len() != params.len() { self.ctx.diags.error(*span, "argument count mismatch"); } }
                    let fixed_n = std::cmp::min(args.len(), params.len());
                    for i in 0..fixed_n {
                        let at = arg_decay_to_rvalue(self.ty_of_expr(&args[i]));
                        let pty = adjust_param_type(&self.ctx.resolve_type(&params[i].ty));
                        let mut ok = self.can_assign(&pty, &at.ty);
                        if !ok {
                            if matches!(pty, CType::Pointer { .. }) && is_null_constant(&args[i]) { ok = true; }
                            if !ok { if let CType::Pointer { .. } = pty { if matches!(args[i], Expr::StrLit(_, _)) { ok = true; } } }
                        }
                        if !ok { self.ctx.diags.error(args[i].span(), format!("incompatible argument {}", i+1)); }
                    }
                    if args.len() > fixed_n { for i in fixed_n..args.len() { let at = arg_decay_to_rvalue(self.ty_of_expr(&args[i])); let _ = default_arg_promotion(&self.ctx.ti, &at.ty); } }
                }
                TyVal { ty: *ret, lvalue: false }
            }

            Expr::Index { base, index, span } => {
                let bt = decay_if_needed(self.ty_of_expr(base));
                let it = self.ty_of_expr(index);
                if !is_integer(&it.ty) { self.ctx.diags.error(index.span(), "array index must be integer"); }
                match &bt.ty {
                    CType::Pointer { of, .. } => TyVal { ty: (*of.clone()), lvalue: true },
                    CType::Array { of, .. } => TyVal { ty: (*of.clone()), lvalue: true },
                    _ => { self.ctx.diags.error(*span, "subscripted value is not an array or pointer"); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                }
            }

            Expr::Member { base, field, span } => {
                let bt = self.ty_of_expr(base);
                match &bt.ty {
                    CType::Struct{ fields: Some(fs), .. } | CType::Union { fields: Some(fs), .. } => {
                        if let Some(d) = fs.iter().find(|d| &d.name == field) { TyVal { ty: d.ty.clone(), lvalue: true } }
                        else { self.ctx.diags.error(*span, format!("no such field '{}'", field)); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                    }
                    CType::Struct{ fields: None, .. } | CType::Union { fields: None, .. } => { self.ctx.diags.error(*span, "member access of incomplete type"); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                    _ => { self.ctx.diags.error(*span, "member access on non-aggregate"); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                }
            }

            Expr::PtrMember { base, field, span } => {
                let bt = decay_if_needed(self.ty_of_expr(base));
                match &bt.ty {
                    CType::Pointer{ of, .. } if matches!(**of, CType::Struct{..} | CType::Union{..}) => {
                        match &**of {
                            CType::Struct{ fields: Some(fs), .. } | CType::Union { fields: Some(fs), .. } => {
                                if let Some(d) = fs.iter().find(|d| &d.name == field) { TyVal { ty: d.ty.clone(), lvalue: true } } else { self.ctx.diags.error(*span, format!("no such field '{}'", field)); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                            }
                            _ => { self.ctx.diags.error(*span, "'->' to incomplete aggregate type"); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                        }
                    }
                    _ => { self.ctx.diags.error(*span, "'->' on non-pointer to aggregate"); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                }
            }

            Expr::PostInc { expr, .. } | Expr::PostDec { expr, .. } => {
                let tv = self.ty_of_expr(expr);
                if !tv.lvalue || !(is_arith(&tv.ty) || is_pointer(&tv.ty)) { self.ctx.diags.error(expr.span(), "operand must be modifiable lvalue of arithmetic or pointer type"); }
                TyVal { ty: tv.ty, lvalue: false }
            }

            Expr::PreInc { expr, .. } | Expr::PreDec { expr, .. } => {
                let tv = self.ty_of_expr(expr);
                if !tv.lvalue || !(is_arith(&tv.ty) || is_pointer(&tv.ty)) { self.ctx.diags.error(expr.span(), "operand must be modifiable lvalue of arithmetic or pointer type"); }
                TyVal { ty: tv.ty, lvalue: false }
            }

            Expr::Unary { op, expr, .. } => {
                let tv = decay_if_needed(self.ty_of_expr(expr));
                match op {
                    UnaryOp::Plus | UnaryOp::Minus => {
                        if !is_arith(&tv.ty) { self.ctx.diags.error(expr.span(), "unary operator requires arithmetic type"); }
                        TyVal { ty: integer_promote(&self.ctx.ti, &tv.ty), lvalue: false }
                    }
                    UnaryOp::BitNot => { if !is_integer(&tv.ty) { self.ctx.diags.error(expr.span(), "bitwise not requires integer type"); } TyVal { ty: integer_promote(&self.ctx.ti, &tv.ty), lvalue: false } }
                    UnaryOp::LogNot => { TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                    UnaryOp::AddrOf => {
                        // Allow & on arrays/functions too
                        TyVal { ty: ptr_to(self.ty_of_expr(expr).ty), lvalue: false }
                    }
                    UnaryOp::Deref => {
                        match &tv.ty { CType::Pointer{ of, .. } => TyVal { ty: (*of.clone()), lvalue: true }, _ => { self.ctx.diags.error(expr.span(), "indirection requires pointer"); TyVal { ty: tv.ty, lvalue: false } } }
                    }
                }
            }

            Expr::Binary { op, lhs, rhs, .. } => {
                use BinaryOp::*;
                match op {
                    Mul | Div => {
                        let lt = decay_if_needed(self.ty_of_expr(lhs));
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        if !is_arith(&lt.ty) || !is_arith(&rt.ty) { self.ctx.diags.error(lhs.span(), "multiplicative operators require arithmetic operands"); }
                        TyVal { ty: usual_arith_conv(&self.ctx.ti, &lt.ty, &rt.ty), lvalue: false }
                    }
                    Mod => {
                        let lt = decay_if_needed(self.ty_of_expr(lhs));
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        if !is_integer(&lt.ty) || !is_integer(&rt.ty) { self.ctx.diags.error(lhs.span(), "operator % requires integer operands"); }
                        TyVal { ty: usual_arith_conv(&self.ctx.ti, &lt.ty, &rt.ty), lvalue: false }
                    }
                    Add => {
                        let lt = decay_if_needed(self.ty_of_expr(lhs));
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        match (&lt.ty, &rt.ty) {
                            (t1, t2) if is_arith(t1) && is_arith(t2) => TyVal { ty: usual_arith_conv(&self.ctx.ti, t1, t2), lvalue: false },
                            (CType::Pointer{ of, .. }, t) if is_integer(t) => TyVal { ty: CType::Pointer{ of: of.clone(), qualifiers: Vec::new() }, lvalue: false },
                            (t, CType::Pointer{ of, .. }) if is_integer(t) => TyVal { ty: CType::Pointer{ of: of.clone(), qualifiers: Vec::new() }, lvalue: false },
                            _ => { self.ctx.diags.error(lhs.span(), "invalid operands to +"); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                        }
                    }
                    Sub => {
                        let lt = decay_if_needed(self.ty_of_expr(lhs));
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        match (&lt.ty, &rt.ty) {
                            (t1, t2) if is_arith(t1) && is_arith(t2) => TyVal { ty: usual_arith_conv(&self.ctx.ti, t1, t2), lvalue: false },
                            (CType::Pointer{ of, .. }, t) if is_integer(t) => TyVal { ty: CType::Pointer{ of: of.clone(), qualifiers: Vec::new() }, lvalue: false },
                            (CType::Pointer{ of: la, .. }, CType::Pointer{ of: ra, .. }) => {
                                if types_compatible(la, ra) || matches!(**la, CType::Void) || matches!(**ra, CType::Void) {
                                    let is_64 = self.ctx.ti.pointer_size() == 8;
                                    TyVal { ty: CType::Int { signed: Some(true), long: is_64, short: false }, lvalue: false }
                                } else { self.ctx.diags.error(lhs.span(), "invalid pointer subtraction"); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                            }
                            _ => { self.ctx.diags.error(lhs.span(), "invalid operands to -"); TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false } }
                        }
                    }
                    Shl | Shr => {
                        let lt = decay_if_needed(self.ty_of_expr(lhs));
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        if !is_integer(&lt.ty) || !is_integer(&rt.ty) { self.ctx.diags.error(lhs.span(), "shift operators require integer operands"); }
                        TyVal { ty: integer_promote(&self.ctx.ti, &lt.ty), lvalue: false }
                    }
                    Lt | Gt | Le | Ge => {
                        let lt = decay_if_needed(self.ty_of_expr(lhs));
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        let ok = (is_arith(&lt.ty) && is_arith(&rt.ty)) || (is_pointer(&lt.ty) && is_pointer(&rt.ty));
                        if !ok { self.ctx.diags.error(lhs.span(), "invalid operands to relational operator"); }
                        TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false }
                    }
                    Eq | Ne => {
                        let lt = decay_if_needed(self.ty_of_expr(lhs));
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        let mut ok = (is_arith(&lt.ty) && is_arith(&rt.ty)) || (is_pointer(&lt.ty) && is_pointer(&rt.ty));
                        if !ok {
                            if is_pointer(&lt.ty) && is_null_constant(rhs) { ok = true; }
                            else if is_pointer(&rt.ty) && is_null_constant(lhs) { ok = true; }
                        }
                        if !ok { self.ctx.diags.error(lhs.span(), "invalid operands to equality operator"); }
                        TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false }
                    }
                    BitAnd | BitOr | BitXor => {
                        let lt = decay_if_needed(self.ty_of_expr(lhs));
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        if !is_integer(&lt.ty) || !is_integer(&rt.ty) { self.ctx.diags.error(lhs.span(), "bitwise operators require integer operands"); }
                        TyVal { ty: usual_arith_conv(&self.ctx.ti, &lt.ty, &rt.ty), lvalue: false }
                    }
                    LogAnd | LogOr => {
                        let lt = decay_if_needed(self.ty_of_expr(lhs));
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        let ok = (is_arith(&lt.ty) || is_pointer(&lt.ty)) && (is_arith(&rt.ty) || is_pointer(&rt.ty));
                        if !ok { self.ctx.diags.error(lhs.span(), "logical operators require scalar operands"); }
                        TyVal { ty: CType::Int { signed: Some(true), long: false, short: false }, lvalue: false }
                    }
                    Comma => { let _ = self.ty_of_expr(lhs); let rt = self.ty_of_expr(rhs); rt }
                }
            }

            Expr::Cond { c, t, e, .. } => {
                let ct = self.ty_of_expr(c); if !is_arith(&ct.ty) && !is_pointer(&ct.ty) { self.ctx.diags.error(c.span(), "condition must be scalar"); }
                let tt = decay_if_needed(self.ty_of_expr(t));
                let et = decay_if_needed(self.ty_of_expr(e));
                if is_arith(&tt.ty) && is_arith(&et.ty) { TyVal { ty: usual_arith_conv(&self.ctx.ti, &tt.ty, &et.ty), lvalue: false } }
                else if is_pointer(&tt.ty) && is_pointer(&et.ty) {
                    // If compatible, keep that type; else if one is void*, result is void*
                    if types_compatible(&tt.ty, &et.ty) { TyVal { ty: tt.ty, lvalue: false } }
                    else {
                        match (&tt.ty, &et.ty) {
                            (CType::Pointer { of: a, .. }, CType::Pointer { of: _b, .. }) if matches!(**a, CType::Void) => TyVal { ty: tt.ty, lvalue: false },
                            (CType::Pointer { of: a, .. }, CType::Pointer { .. }) if matches!(**a, CType::Void) => TyVal { ty: tt.ty, lvalue: false },
                            (CType::Pointer { .. }, CType::Pointer { of: b, .. }) if matches!(**b, CType::Void) => TyVal { ty: et.ty, lvalue: false },
                            _ => { self.ctx.diags.error(c.span(), "incompatible pointer types in ?:"); TyVal { ty: tt.ty, lvalue: false } }
                        }
                    }
                }
                else if is_pointer(&tt.ty) && is_null_constant(e) { TyVal { ty: tt.ty, lvalue: false } }
                else if is_pointer(&et.ty) && is_null_constant(t) { TyVal { ty: et.ty, lvalue: false } }
                else { self.ctx.diags.error(c.span(), "incompatible operands to ?:" ); TyVal { ty: tt.ty, lvalue: false } }
            }

            Expr::Assign { op, lhs, rhs, span } => {
                use AssignOp as AO;
                let lt = self.ty_of_expr(lhs);
                if !lt.lvalue { self.ctx.diags.error(*span, "assignment to non-lvalue"); return TyVal { ty: lt.ty, lvalue: false }; }
                match op {
                    AO::Assign => {
                        let rt = self.ty_of_expr(rhs);
                        let mut ok = self.can_assign(&lt.ty, &rt.ty);
                        if !ok {
                            if matches!(lt.ty, CType::Pointer{ .. }) && is_null_constant(rhs) { ok = true; }
                        }
                        if !ok { self.ctx.diags.error(*span, "incompatible types for assignment"); }
                        TyVal { ty: lt.ty, lvalue: false }
                    }
                    AO::Mul | AO::Div => {
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        if !(is_arith(&lt.ty) && is_arith(&rt.ty)) { self.ctx.diags.error(*span, "invalid operands to '*=' or '/='"); }
                        TyVal { ty: lt.ty, lvalue: false }
                    }
                    AO::Mod => {
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        if !(is_integer(&lt.ty) && is_integer(&rt.ty)) { self.ctx.diags.error(*span, "invalid operands to '%='"); }
                        TyVal { ty: lt.ty, lvalue: false }
                    }
                    AO::Add => {
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        let mut ok = false;
                        if is_arith(&lt.ty) && is_arith(&rt.ty) { ok = true; }
                        if let CType::Pointer { .. } = lt.ty { if is_integer(&rt.ty) { ok = true; } }
                        if !ok { self.ctx.diags.error(*span, "invalid operands to '+='"); }
                        TyVal { ty: lt.ty, lvalue: false }
                    }
                    AO::Sub => {
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        let mut ok = false;
                        if is_arith(&lt.ty) && is_arith(&rt.ty) { ok = true; }
                        if let CType::Pointer { .. } = lt.ty { if is_integer(&rt.ty) { ok = true; } }
                        if !ok { self.ctx.diags.error(*span, "invalid operands to '-='"); }
                        TyVal { ty: lt.ty, lvalue: false }
                    }
                    AO::Shl | AO::Shr => {
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        if !(is_integer(&lt.ty) && is_integer(&rt.ty)) { self.ctx.diags.error(*span, "invalid operands to shift-assignment"); }
                        TyVal { ty: lt.ty, lvalue: false }
                    }
                    AO::And | AO::Xor | AO::Or => {
                        let rt = decay_if_needed(self.ty_of_expr(rhs));
                        if !(is_integer(&lt.ty) && is_integer(&rt.ty)) { self.ctx.diags.error(*span, "invalid operands to bitwise-assignment"); }
                        TyVal { ty: lt.ty, lvalue: false }
                    }
                }
            }

            Expr::Cast { ty, expr, .. } => { let _ = self.ty_of_expr(expr); TyVal { ty: self.ctx.resolve_type(ty), lvalue: false } }

            Expr::SizeOfExpr { expr, .. } => { let _ = self.ty_of_expr(expr); TyVal { ty: CType::Int { signed: Some(false), long: false, short: false }, lvalue: false } }
            Expr::SizeOfType { .. } => TyVal { ty: CType::Int { signed: Some(false), long: false, short: false }, lvalue: false },
        }
    }

    fn is_lvalue_expr(&mut self, e: &Expr) -> bool { self.ty_of_expr(e).lvalue }

    fn can_assign(&self, lhs: &CType, rhs: &CType) -> bool {
        let lhs = self.ctx.resolve_type(lhs);
        let rhs = self.ctx.resolve_type(rhs);
        if types_compatible(&lhs, &rhs) { return true; }
        if is_arith(&lhs) && is_arith(&rhs) { return true; }
        match (&lhs, &rhs) {
            (CType::Pointer{ of: la, qualifiers: lq }, CType::Pointer{ of: ra, qualifiers: rq }) => {
                let la_is_fn = matches!(**la, CType::Function { .. });
                let ra_is_fn = matches!(**ra, CType::Function { .. });
                if la_is_fn || ra_is_fn {
                    // function pointers: require compatible function type and identical pointer qualifiers
                    return types_compatible(la, ra) && lq == rq;
                }
                // object pointers
                // Rule: you may add qualifiers on the pointed-to type (rhs quals ⊆ lhs quals), but not drop them
                let quals_ok = quals_subset(rq, lq);
                if matches!(**la, CType::Void) || matches!(**ra, CType::Void) {
                    return quals_ok;
                }
                // Require compatibility of pointed-to types and non-dropping of qualifiers
                types_compatible(la, ra) && quals_ok
            }
            (CType::Pointer{ .. }, _) => false,
            _ => false,
        }
    }

    // --- Aggregate initializers ---
    fn infer_array_size(&self, ty: &CType, init: &Initializer) -> CType {
        let rty = self.ctx.resolve_type(ty);
        match (&rty, init) {
            (CType::Array { of, size: None }, Initializer::List(list)) => CType::Array { of: of.clone(), size: Some(list.len()) },
            (CType::Array { of, size: None }, Initializer::Expr(Expr::StrLit(s, _))) if is_char_type(of) => {
                let n = c_string_len(s);
                CType::Array { of: of.clone(), size: Some(n + 1) }
            }
            _ => rty,
        }
    }

    fn check_initializer(&mut self, ty: &CType, init: &Initializer, span: Span) {
        let rty = self.ctx.resolve_type(ty);
        match (&rty, init) {
            // Arrays
            (CType::Array { of, size }, Initializer::Expr(Expr::StrLit(s, sp))) if is_char_type(of) => {
                let n = c_string_len(s);
                if let Some(sz) = size { if *sz < n { self.ctx.diags.error(*sp, format!("string literal too long for array ({} > {})", n, sz)); } }
            }
            (CType::Array { .. }, Initializer::Expr(_)) => {
                self.ctx.diags.error(span, "array initializer must be a brace-enclosed list (or string for char[])");
            }
            (CType::Array { of, size }, Initializer::List(list)) => {
                if let Some(sz) = size { if list.len() > *sz { self.ctx.diags.error(span, "too many initializers for array"); } }
                for it in list { self.check_initializer(of, it, span); }
            }

            // Structs
            (CType::Struct { fields: None, .. }, _) => {
                self.ctx.diags.error(span, "initializer for incomplete struct type");
            }
            (CType::Struct { fields: Some(_fs), .. }, Initializer::Expr(_)) => {
                self.ctx.diags.error(span, "initializer for struct must be a brace-enclosed list");
            }
            (CType::Struct { fields: Some(fs), .. }, Initializer::List(list)) => {
                if list.len() > fs.len() { self.ctx.diags.error(span, "too many initializers for struct"); }
                let mut i = 0usize;
                for it in list { if i < fs.len() { self.check_initializer(&fs[i].ty, it, span); i += 1; } }
            }

            // Unions
            (CType::Union { fields: None, .. }, _) => { self.ctx.diags.error(span, "initializer for incomplete union type"); }
            (CType::Union { fields: Some(fs), .. }, Initializer::List(list)) => {
                if list.is_empty() { return; }
                if list.len() > 1 { self.ctx.diags.error(span, "initializer list for union must have exactly one element"); }
                if let Some(first) = fs.first() { self.check_initializer(&first.ty, &list[0], span); }
            }
            (CType::Union { fields: Some(fs), .. }, Initializer::Expr(e)) => {
                if let Some(first) = fs.first() {
                    // Allow scalar expr to initialize first member
                    let tv = self.ty_of_expr(e);
                    if !self.can_assign(&first.ty, &tv.ty) { self.ctx.diags.error(e.span(), "incompatible initializer for union"); }
                }
            }

            // Scalars and others: allow arbitrarily nested single-element brace lists
            (_, init_any) => {
                let unwrapped = unwrap_scalar_list(init_any);
                match unwrapped {
                    Initializer::List(list) => {
                        if list.len() == 1 { self.check_initializer(&rty, &list[0], span); }
                        else { self.ctx.diags.error(span, "too many initializers for scalar"); }
                    }
                    Initializer::Expr(e) => {
                        let rhs_tv = self.ty_of_expr(e);
                        let mut ok = self.can_assign(&rty, &rhs_tv.ty);
                        if !ok { if matches!(rty, CType::Pointer { .. }) && is_null_constant(e) { ok = true; } }
                        if !ok { self.ctx.diags.error(e.span(), "incompatible initializer type"); }
                    }
                }
            }
        }
    }

    fn process_type_defs(&mut self, ty: &CType) {
        match ty {
            CType::Enum { items: Some(items), tag, .. } => {
                if let Some(n) = tag { self.ctx.declare_tag(n, ty); }
                // Evaluate and declare enum constants with an environment for previous enumerators
                let mut env: std::collections::HashMap<String, i64> = std::collections::HashMap::new();
                let mut next: i64 = 0;
                for (name, opt_expr) in items {
                    if let Some(e) = opt_expr {
                        let sz = |t: &CType| self.ctx.sizeof_type(t);
                        match const_eval::eval_ice_with_env(e, &sz, &env) {
                            Ok(v) => next = v,
                            Err(msg) => { self.ctx.diags.error(e.span(), format!("invalid enum constant expression: {}", msg)); }
                        }
                    }
                    // declare enum const symbol in current scope
                    self.ctx.declare(Symbol { name: name.clone(), kind: SymKind::EnumConst, ty: Some(CType::Int { signed: Some(true), long: false, short: false }), span: ty_span(ty), const_value: Some(next) });
                    env.insert(name.clone(), next);
                    next = next.saturating_add(1);
                }
            }
            CType::Struct { tag, fields: Some(fs) , .. } => {
                if let Some(n) = tag { self.ctx.declare_tag(n, ty); }
                for f in fs { self.check_bit_fields_in_decl(f); }
            }
            CType::Union { tag, fields: Some(fs), .. } => {
                if let Some(n) = tag { self.ctx.declare_tag(n, ty); }
                for f in fs { self.check_bit_fields_in_decl(f); }
            }
            CType::Pointer { of, .. } | CType::Array { of, .. } => self.process_type_defs(of),
            CType::Function { ret, params, .. } => {
                self.process_type_defs(ret);
                for p in params { self.process_type_defs(&p.ty); }
            }
            _ => {}
        }
    }

    fn check_bit_fields_in_type(&mut self, ty: &CType) {
        match ty {
            CType::Struct { fields: Some(fs), .. } | CType::Union { fields: Some(fs), .. } => {
                for f in fs { self.check_bit_fields_in_decl(f); }
            }
            _ => {}
        }
    }

    fn check_bit_fields_in_decl(&mut self, d: &Decl) {
        if let Some(wexpr) = &d.bit_width {
            // width must be ICE, non-negative, and <= type bit width
            let sz = |t: &CType| self.ctx.sizeof_type(t);
            match eval_ice(wexpr, &sz) {
                Ok(v) => {
                    if v < 0 { self.ctx.diags.error(wexpr.span(), "bit-field width cannot be negative"); }
                    else {
                        let rty = self.ctx.resolve_type(&d.ty);
                        if let Some(bits) = self.ctx.ti.int_bit_width(&rty) {
                            if (v as u64) > bits as u64 { self.ctx.diags.error(wexpr.span(), format!("bit-field width {} exceeds type width {}", v, bits)); }
                        } else {
                            self.ctx.diags.error(d.span, "bit-field type must be an integer type");
                        }
                    }
                }
                Err(msg) => { self.ctx.diags.error(wexpr.span(), format!("bit-field width is not a constant expression: {}", msg)); }
            }
        }
    }

    fn check_zero_sized_arrays(&mut self, ty: &CType, span: Span) {
        match ty {
            CType::Array { size: Some(0), .. } => self.ctx.diags.error(span, "zero-sized arrays are not allowed in C89"),
            CType::Pointer { of, .. } | CType::Array { of, .. } => self.check_zero_sized_arrays(of, span),
            CType::Function { ret, params, .. } => { self.check_zero_sized_arrays(ret, span); for p in params { self.check_zero_sized_arrays(&p.ty, span); } }
            CType::Struct { fields: Some(fs), .. } | CType::Union { fields: Some(fs), .. } => { for f in fs { self.check_zero_sized_arrays(&f.ty, f.span); } }
            _ => {}
        }
    }

    fn has_static_storage_duration(&self, d: &Decl) -> bool {
        // file-scope objects (scopes.len()==1) have static storage duration unless 'register' (ignored here)
        // block-scope 'static' also has static storage duration
        let file_scope = self.ctx.scopes.len() == 1;
        if file_scope { !matches!(d.storage, Some(StorageClass::Register) | Some(StorageClass::Auto) | Some(StorageClass::Typedef)) }
        else { matches!(d.storage, Some(StorageClass::Static)) }
    }

    fn require_const_initializer(&mut self, init: &Initializer) -> Result<(), String> {
        match init {
            Initializer::Expr(e) => {
                let sz = |t: &CType| self.ctx.sizeof_type(t);
                // Build environment with enum constants from current scope
                let mut env: std::collections::HashMap<String, i64> = std::collections::HashMap::new();
                for scope in &self.ctx.scopes {
                    for sym in scope.syms.values() {
                        if let (SymKind::EnumConst, Some(value)) = (sym.kind, sym.const_value) {
                            env.insert(sym.name.clone(), value);
                        }
                    }
                }
                const_eval::eval_ice_with_env(e, &sz, &env).map(|_| ())
            }
            Initializer::List(list) => {
                for it in list { self.require_const_initializer(it)?; }
                Ok(())
            }
        }
    }
}

fn ty_span(_ty: &CType) -> Span { Span { lo: 0, hi: 0, line: 0, col: 0 } }

// Allow extra braces around scalars by unwrapping nested single-element lists
fn unwrap_scalar_list<'a>(init: &'a Initializer) -> &'a Initializer {
    let mut cur = init;
    loop {
        match cur {
            Initializer::List(list) if list.len() == 1 => { cur = &list[0]; }
            _ => break,
        }
    }
    cur
}

pub fn analyze_translation_unit(tu: &TranslationUnit) -> Diagnostics { SemanticAnalyzer::new(tu).analyze() }

fn usual_arith_conv(ti: &TargetInfo, a: &CType, b: &CType) -> CType {
    // Handle floating first
    if is_floating(a) || is_floating(b) { return CType::Double { long: false }; }
    // Integer conversions per C89 on ILP32
    let ap = integer_promote(ti, a); let bp = integer_promote(ti, b);
    // Extract signedness and rank (int=2, long=3)
    fn rank(ty: &CType) -> i32 { match ty { CType::Int { long: true, .. } => 3, CType::Int { .. } => 2, CType::Char { .. } => 1, CType::Enum { .. } => 2, _ => 2 } }
    fn is_unsigned(ty: &CType) -> bool { matches!(ty, CType::Int { signed: Some(false), .. }) }
    let ra = rank(&ap); let rb = rank(&bp);
    let ua = is_unsigned(&ap); let ub = is_unsigned(&bp);
    // Same type
    if types_compatible(&ap, &bp) { return ap; }
    // Both signed or both unsigned: pick higher rank with same signedness
    if ua == ub { return if ra >= rb { ap } else { bp }; }
    // One signed, one unsigned
    // Identify unsigned (u) and signed (s)
    let (u, ru) = if ua { (&ap, ra) } else { (&bp, rb) };
    let (s, rs) = if ua { (&bp, rb) } else { (&ap, ra) };
    if ru >= rs { return u.clone(); }
    // ru < rs: if signed type can represent all values of unsigned, keep signed; else use unsigned version of signed rank
    // On ILP32, signed long cannot represent all unsigned int; result -> unsigned long
    if matches!(s, CType::Int { long: true, .. }) && matches!(u, CType::Int { long: false, .. }) {
        return CType::Int { signed: Some(false), long: true, short: false };
    }
    // Fallback to unsigned of higher rank
    if rs > ru { return CType::Int { signed: Some(false), long: matches!(s, CType::Int { long: true, .. }), short: false }; }
    // Default
    ap
}

fn qualifiers_of(ty: &CType) -> Vec<TypeQualifier> { match ty { CType::Pointer { qualifiers, .. } => qualifiers.clone(), _ => Vec::new() } }
fn quals_subset(a: &[TypeQualifier], b: &[TypeQualifier]) -> bool { a.iter().all(|q| b.contains(q)) }
fn pointed_type<'a>(ty: &'a CType) -> Option<&'a CType> { if let CType::Pointer { of, .. } = ty { Some(of) } else { None } }

fn types_compatible(a: &CType, b: &CType) -> bool {
    match (a, b) {
        (CType::Int{ signed: sa, long: la, short: sha }, CType::Int{ signed: sb, long: lb, short: shb }) => sa==sb && la==lb && sha==shb,
        (CType::Char{ signed: sa }, CType::Char{ signed: sb }) => sa==sb || sa.is_none() || sb.is_none(),
        (CType::Float, CType::Float) => true,
        (CType::Double{ long: la }, CType::Double{ long: lb }) => la==lb,
        (CType::Pointer{ of: oa, qualifiers: qa }, CType::Pointer{ of: ob, qualifiers: qb }) => types_compatible(oa, ob) && qa == qb,
        (CType::Array{ of: oa, size: sa }, CType::Array{ of: ob, size: sb }) => sa==sb && types_compatible(oa, ob),
        (CType::Function{ ret: ra, params: pa, varargs: va, prototype: pra }, CType::Function{ ret: rb, params: pb, varargs: vb, prototype: prb }) => va==vb && pra==prb && types_compatible(ra, rb) && pa.len()==pb.len(),
        (CType::Struct { tag: ta, .. }, CType::Struct { tag: tb, .. }) => ta==tb,
        (CType::Union { tag: ta, .. }, CType::Union { tag: tb, .. }) => ta==tb,
        (CType::Enum { tag: ta, .. }, CType::Enum { tag: tb, .. }) => ta==tb,
        (CType::Named(na), CType::Named(nb)) => na==nb,
        _ => false,
    }
}

fn decay_if_needed(tv: TyVal) -> TyVal { match tv.ty { CType::Array{ of, .. } => TyVal { ty: ptr_to(*of), lvalue: false }, CType::Function{ .. } => TyVal { ty: ptr_to(tv.ty), lvalue: false }, _ => tv } }

fn is_null_constant(e: &Expr) -> bool { matches!(e, Expr::IntLit(s, _) if s=="0") }

// --- helpers for string literal length (C89 escapes subset) ---
fn c_string_len(s: &str) -> usize {
    let bytes = s.as_bytes();
    if bytes.len() >= 2 && bytes[0] == b'"' && bytes[bytes.len()-1] == b'"' {
        let mut i = 1usize; // skip opening quote
        let end = bytes.len() - 1; // before closing quote
        let mut len = 0usize;
        while i < end {
            let c = bytes[i];
            if c == b'\\' {
                i += 1;
                if i >= end { break; }
                let esc = bytes[i];
                match esc {
                    b'\'' | b'"' | b'\\' | b'?' | b'a' | b'b' | b'f' | b'n' | b'r' | b't' | b'v' => { len += 1; }
                    b'x' => {
                        // hex sequence: consume 1+ hex digits
                        let mut j = i + 1; let mut consumed = 0;
                        while j < end && (bytes[j] as char).is_ascii_hexdigit() { j += 1; consumed += 1; }
                        if consumed == 0 { /* ill-formed, count as 'x' */ }
                        len += 1; i = j - 1;
                    }
                    b'0'..=b'7' => {
                        // up to 3 octal digits starting from this one
                        let mut j = i; let mut k = 0;
                        while j < end && k < 3 && (bytes[j] >= b'0' && bytes[j] <= b'7') { j += 1; k += 1; }
                        len += 1; i = j - 1;
                    }
                    _ => { len += 1; }
                }
            } else if c == b'"' { break; } else { len += 1; }
            i += 1;
        }
        len
    } else {
        // Not quoted? Treat raw byte length
        s.len()
    }
}
