// ----------------------------
// File: src/front/semantics/types.rs
// ----------------------------
#![allow(dead_code)]
use crate::front::ast::{CType, TypeQualifier, Decl};

#[derive(Debug, Clone, Copy)]
pub struct Layout { pub size: usize, pub align: usize }

impl Layout { pub const fn new(size: usize, align: usize) -> Self { Self { size, align } } }

// i386 SysV (baseline)
pub const CHAR: Layout = Layout::new(1, 1);
pub const SHORT: Layout = Layout::new(2, 2);
pub const INT: Layout = Layout::new(4, 4);
pub const LONG: Layout = Layout::new(4, 4);
pub const PTR: Layout = Layout::new(4, 4);
pub const FLOAT: Layout = Layout::new(4, 4);
pub const DOUBLE: Layout = Layout::new(8, 4);
// long double is implementation-defined on i386; use 12 bytes, align 4 (common)
pub const LONG_DOUBLE: Layout = Layout::new(12, 4);

pub fn int_bit_width(ty: &CType) -> Option<u32> {
    match ty {
        CType::Char { .. } => Some(8),
        CType::Int { short: true, .. } => Some((SHORT.size * 8) as u32),
        CType::Int { long: true, .. } => Some((LONG.size * 8) as u32),
        CType::Int { .. } => Some((INT.size * 8) as u32),
        _ => None,
    }
}

pub fn layout_of(resolved: &CType) -> Option<Layout> {
    match resolved {
        CType::Void => None,
        CType::Char { .. } => Some(CHAR),
        CType::Int { short: true, .. } => Some(SHORT),
        CType::Int { long: true, .. } => Some(LONG),
        CType::Int { .. } => Some(INT),
        CType::Float => Some(FLOAT),
        CType::Double { long: true } => Some(LONG_DOUBLE),
        CType::Double { long: false } => Some(DOUBLE),
        CType::Pointer { .. } => Some(PTR),
        CType::Array { of, size } => {
            if let Some(el) = layout_of(of) {
                let n = size.unwrap_or(0);
                Some(Layout::new(el.size.saturating_mul(n), el.align))
            } else { None }
        }
        CType::Function { .. } => None,
        CType::Struct { fields, .. } => struct_layout(fields.as_ref()?),
        CType::Union { fields, .. } => union_layout(fields.as_ref()?),
        CType::Enum { .. } => Some(INT),
        CType::Named(_) => None,
    }
}

fn struct_layout(fields: &Vec<Decl>) -> Option<Layout> {
    let mut offset = 0usize;
    let mut max_align = 1usize;
    for f in fields {
        let ly = match layout_of(&f.ty) { Some(l) => l, None => return None };
        max_align = max_align.max(ly.align);
        offset = align_to(offset, ly.align) + ly.size; // naive: ignores bit-fields packing
    }
    let size = align_to(offset, max_align);
    Some(Layout::new(size, max_align))
}

fn union_layout(fields: &Vec<Decl>) -> Option<Layout> {
    let mut size = 0usize; let mut align = 1usize;
    for f in fields {
        if let Some(ly) = layout_of(&f.ty) { size = size.max(ly.size); align = align.max(ly.align); } else { return None }
    }
    Some(Layout::new(size, align))
}

#[inline]
fn align_to(x: usize, a: usize) -> usize { if a == 0 { x } else { (x + a - 1) & !(a - 1) } }
