// ----------------------------
// File: src/front/semantics/target.rs
// ----------------------------
#![allow(dead_code)]
//! Target information: sizes/alignments for types on a given architecture.

use crate::front::ast::CType;

#[derive(Debug, Clone, Copy)]
pub enum Arch { I386, X86_64 }

#[derive(Debug, Clone, Copy)]
pub struct TargetInfo {
    pub arch: Arch,
}

impl TargetInfo {
    pub fn i386() -> Self { Self { arch: Arch::I386 } }
    pub fn x86_64() -> Self { Self { arch: Arch::X86_64 } }

    pub fn pointer_size(&self) -> usize { match self.arch { Arch::I386 => 4, Arch::X86_64 => 8 } }

    pub fn sizeof(&self, ty: &CType) -> Result<usize, String> {
        match ty {
            CType::Void => Err("invalid: sizeof(void)".into()),
            CType::Char { .. } => Ok(1),
            CType::Int { short, long, .. } => {
                match (short, long) {
                    (true, false) => Ok(2),
                    (false, false) => Ok(4),
                    (false, true) => Ok(match self.arch { Arch::I386 => 4, Arch::X86_64 => 8 }),
                    (true, true) => Err("invalid type: short long".into()),
                }
            }
            CType::Float => Ok(4),
            CType::Double { long } => {
                if *long { Ok(match self.arch { Arch::I386 => 12, Arch::X86_64 => 16 }) } else { Ok(8) }
            }
            CType::Named(_) => Err("sizeof typedef name requires resolution".into()),
            CType::Pointer { .. } => Ok(self.pointer_size()),
            CType::Array { of, size } => {
                if let Some(n) = size { Ok(self.sizeof(of)? * *n) } else { Err("invalid: sizeof(incomplete array)".into()) }
            }
            CType::Function { .. } => Err("invalid: sizeof(function)".into()),
            CType::Struct { fields, .. } => {
                if let Some(flds) = fields {
                    // Minimal layout: sum of fields with natural alignment; no packing, final size rounded up to max align.
                    let mut size = 0usize;
                    let mut max_align = 1usize;
                    for d in flds {
                        let a = self.alignof(&d.ty)?;
                        max_align = max_align.max(a);
                        let field_size = self.sizeof(&d.ty)?;
                        size = align_to(size, a) + field_size;
                    }
                    Ok(align_to(size, max_align))
                } else {
                    Err("invalid: sizeof(incomplete struct)".into())
                }
            }
            CType::Union { fields, .. } => {
                if let Some(flds) = fields {
                    let mut size = 0usize;
                    let mut max_align = 1usize;
                    for d in flds {
                        let a = self.alignof(&d.ty)?;
                        max_align = max_align.max(a);
                        size = size.max(self.sizeof(&d.ty)?);
                    }
                    Ok(align_to(size, max_align))
                } else {
                    Err("invalid: sizeof(incomplete union)".into())
                }
            }
            CType::Enum { .. } => Ok(4), // C89: compatible with int on both i386/x86_64 generally
        }
    }

    pub fn alignof(&self, ty: &CType) -> Result<usize, String> {
        match ty {
            CType::Void => Err("alignof(void)".into()),
            CType::Char { .. } => Ok(1),
            CType::Int { short, long, .. } => {
                match (short, long) {
                    (true, false) => Ok(2),
                    (false, false) => Ok(4),
                    (false, true) => Ok(match self.arch { Arch::I386 => 4, Arch::X86_64 => 8 }),
                    (true, true) => Err("invalid type: short long".into()),
                }
            }
            CType::Float => Ok(4),
            CType::Double { long } => {
                if *long { Ok(match self.arch { Arch::I386 => 4, Arch::X86_64 => 16 }) } else { Ok(8) }
            }
            CType::Named(_) => Err("alignof typedef requires resolution".into()),
            CType::Pointer { .. } => Ok(self.pointer_size()),
            CType::Array { of, .. } => self.alignof(of),
            CType::Function { .. } => Ok(self.pointer_size()),
            CType::Struct { fields, .. } => {
                if let Some(flds) = fields {
                    let mut max_align = 1usize;
                    for d in flds {
                        max_align = max_align.max(self.alignof(&d.ty)?);
                    }
                    Ok(max_align)
                } else { Err("alignof incomplete struct".into()) }
            }
            CType::Union { fields, .. } => {
                if let Some(flds) = fields {
                    let mut max_align = 1usize;
                    for d in flds { max_align = max_align.max(self.alignof(&d.ty)?); }
                    Ok(max_align)
                } else { Err("alignof incomplete union".into()) }
            }
            CType::Enum { .. } => Ok(4),
        }
    }

    pub fn int_bit_width(&self, ty: &CType) -> Option<usize> {
        match ty {
            CType::Char { .. } => Some(8),
            CType::Int { short, long, .. } => Some(if *short { 16 } else if *long { match self.arch { Arch::I386 => 32, Arch::X86_64 => 64 } } else { 32 }),
            CType::Enum { .. } => Some(32),
            _ => None,
        }
    }
}

fn align_to(sz: usize, a: usize) -> usize { if a == 0 { sz } else { (sz + a - 1) / a * a } }
