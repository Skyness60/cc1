// ----------------------------
// File: src/front/semantics/const_eval.rs
// ----------------------------
#![allow(dead_code)]
use std::collections::HashMap;
use crate::front::ast::{Expr, UnaryOp, BinaryOp, CType};

fn parse_int_lit(s: &str) -> Option<i64> {
    // Strip suffixes (u/U/l/L combinations)
    let mut end = s.len();
    while end > 0 {
        let c = s.as_bytes()[end-1];
        if matches!(c, b'u'|b'U'|b'l'|b'L') { end -= 1; } else { break; }
    }
    let core = &s[..end];
    if core.starts_with("0x") || core.starts_with("0X") {
        i64::from_str_radix(&core[2..], 16).ok()
    } else if core.starts_with('0') && core.len() > 1 {
        i64::from_str_radix(&core[1..], 8).ok()
    } else {
        core.parse::<i64>().ok()
    }
}

// Minimal helpers for ICE support
fn is_hex_digit(b: u8) -> bool { (b'0'..=b'9').contains(&b) || (b'a'..=b'f').contains(&b) || (b'A'..=b'F').contains(&b) }
fn hex_val(b: u8) -> u8 { match b { b'0'..=b'9' => b - b'0', b'a'..=b'f' => 10 + (b - b'a'), b'A'..=b'F' => 10 + (b - b'A'), _ => 0 } }

fn parse_char_lit(s: &str) -> Option<i64> {
    // Expect a single-quoted C89 character literal, return its (first) character value as i64
    let bytes = s.as_bytes();
    if bytes.len() < 2 || bytes[0] != b'\'' || bytes[bytes.len()-1] != b'\'' { return None; }
    let mut i = 1usize; // after opening quote
    if i >= bytes.len()-1 { return None; }
    let v: i64 = if bytes[i] == b'\\' { // escape sequence
        i += 1; if i >= bytes.len()-1 { return None; }
        match bytes[i] {
            b'\'' => 0x27,
            b'"' => 0x22,
            b'\\' => 0x5C,
            b'?' => b'?' as i64,
            b'a' => 7,
            b'b' => 8,
            b'f' => 12,
            b'n' => 10,
            b'r' => 13,
            b't' => 9,
            b'v' => 11,
            b'x' => {
                // one or more hex digits
                let mut j = i + 1; let end = bytes.len()-1; let mut val: u32 = 0; let mut saw = false;
                while j < end && is_hex_digit(bytes[j]) { val = (val << 4) | (hex_val(bytes[j]) as u32); j += 1; saw = true; }
                if !saw { return None; }
                val as i64 & 0xFF
            }
            b'0'..=b'7' => {
                // up to 3 octal digits, including the one at i
                let mut j = i; let end = bytes.len()-1; let mut k = 0; let mut val: u32 = 0;
                while j < end && k < 3 { match bytes[j] { b'0'..=b'7' => { val = (val << 3) | ((bytes[j]-b'0') as u32); j += 1; k += 1; }, _ => break } }
                val as i64 & 0xFF
            }
            _ => bytes[i] as i64,
        }
    } else {
        bytes[i] as i64
    };
    Some(v)
}

fn c_string_len(s: &str) -> usize {
    let bytes = s.as_bytes();
    if bytes.len() >= 2 && bytes[0] == b'"' && bytes[bytes.len()-1] == b'"' {
        let mut i = 1usize; // skip opening quote
        let end = bytes.len() - 1; // before closing quote
        let mut len = 0usize;
        while i < end {
            let c = bytes[i];
            if c == b'\\' {
                i += 1; if i >= end { break; }
                match bytes[i] {
                    b'\'' | b'"' | b'\\' | b'?' | b'a' | b'b' | b'f' | b'n' | b'r' | b't' | b'v' => { len += 1; }
                    b'x' => { let mut j = i + 1; let mut consumed = 0; while j < end && is_hex_digit(bytes[j]) { j += 1; consumed += 1; } if consumed == 0 { /* ill-formed, count as 'x' */ } len += 1; i = j - 1; }
                    b'0'..=b'7' => { let mut j = i; let mut k = 0; while j < end && k < 3 { match bytes[j] { b'0'..=b'7' => { j += 1; k += 1; }, _ => break } } len += 1; i = j - 1; }
                    _ => { len += 1; }
                }
            } else if c == b'"' { break; } else { len += 1; }
            i += 1;
        }
        len
    } else { s.len() }
}

fn is_integer_type(ty: &CType) -> bool {
    match ty {
        // Non-integer types
        CType::Void
        | CType::Float
        | CType::Double { .. }
        | CType::Pointer { .. }
        | CType::Array { .. }
        | CType::Function { .. }
        | CType::Struct { .. }
        | CType::Union { .. }
        | CType::Named(_) => false,
        // Integer-like types
        CType::Char { .. }
        | CType::Int { .. }
        | CType::Enum { .. } => true,
    }
}

// Best-effort type deduction for sizeof(expr) in ICE without full typing context
fn ice_type_of_expr_for_sizeof(e: &Expr) -> Option<CType> {
    match e {
        Expr::IntLit(_, _) | Expr::CharLit(_, _) => Some(CType::Int { signed: Some(true), long: false, short: false }),
        Expr::FloatLit(_, _) => Some(CType::Double { long: false }),
        Expr::StrLit(s, _) => {
            let n = c_string_len(s) + 1; // includes NUL
            Some(CType::Array { of: Box::new(CType::Char { signed: None }), size: Some(n) })
        }
        Expr::Cast { ty, .. } => Some(ty.clone()),
        // Unary operators: approximate
        Expr::Unary { op, expr, .. } => {
            match op {
                UnaryOp::AddrOf => {
                    // &expr has pointer type; pointed-to type doesn't matter for sizeof
                    Some(CType::Pointer { of: Box::new(CType::Void), qualifiers: vec![] })
                }
                UnaryOp::Deref => {
                    // *( (T*)expr ) has type T; try to detect cast-to-pointer
                    match &**expr {
                        Expr::Cast { ty: CType::Pointer { of, .. }, .. } => Some((**of).clone()),
                        // Heuristic: *("str" + i) -> char
                        Expr::Binary { lhs, .. } if matches!(**lhs, Expr::StrLit(_, _)) => Some(CType::Char { signed: None }),
                        _ => None,
                    }
                }
                UnaryOp::Plus | UnaryOp::Minus | UnaryOp::BitNot => {
                    // Preserve inner arithmetic kind if known, else assume int
                    ice_type_of_expr_for_sizeof(expr).or_else(|| Some(CType::Int { signed: Some(true), long: false, short: false }))
                }
                UnaryOp::LogNot => Some(CType::Int { signed: Some(true), long: false, short: false }),
            }
        }
        // Indexing: a[i] where a is array/pointer. Handle common cases.
        Expr::Index { base, .. } => {
            match &**base {
                Expr::StrLit(_, _) => Some(CType::Char { signed: None }),
                Expr::Cast { ty: CType::Pointer { of, .. }, .. } => Some((**of).clone()),
                _ => None,
            }
        }
        // Simple binary operators: if either side float -> double, else int; comparisons/logicals -> int
        Expr::Binary { op, lhs, rhs, .. } => {
            use BinaryOp::*;
            match op {
                Add | Sub | Mul | Div | Mod | Shl | Shr | BitAnd | BitOr | BitXor => {
                    let lt = ice_type_of_expr_for_sizeof(lhs);
                    let rt = ice_type_of_expr_for_sizeof(rhs);
                    if matches!(lt, Some(CType::Double { .. })) || matches!(rt, Some(CType::Double { .. })) {
                        Some(CType::Double { long: false })
                    } else { Some(CType::Int { signed: Some(true), long: false, short: false }) }
                }
                Lt | Gt | Le | Ge | Eq | Ne | LogAnd | LogOr | Comma => Some(CType::Int { signed: Some(true), long: false, short: false }),
            }
        }
        // Conditional operator: unify arithmetic roughly
        Expr::Cond { t, e, .. } => {
            let tt = ice_type_of_expr_for_sizeof(t);
            let et = ice_type_of_expr_for_sizeof(e);
            match (tt, et) {
                (Some(CType::Double{..}), _) | (_, Some(CType::Double{..})) => Some(CType::Double { long: false }),
                (Some(CType::Int{..}), Some(CType::Int{..})) | (Some(CType::Char{..}), Some(CType::Int{..})) | (Some(CType::Int{..}), Some(CType::Char{..})) => Some(CType::Int { signed: Some(true), long: false, short: false }),
                (Some(t), Some(_)) => Some(t),
                _ => None,
            }
        }
        // sizeof does not evaluate; other cases require full sema -> not available here
        _ => None,
    }
}

// Helper to determine if an expression should be treated as unsigned
fn expr_is_unsigned(e: &Expr) -> bool {
    match e {
        Expr::Cast { ty, .. } => type_is_unsigned(ty),
        Expr::Unary { expr, .. } => expr_is_unsigned(expr),
        _ => false, // Default to signed
    }
}

fn type_is_unsigned(ty: &CType) -> bool {
    match ty {
        CType::Int { signed: Some(false), .. } => true,
        CType::Char { signed: Some(false) } => true,
        _ => false,
    }
}

// Helper to infer the effective bit width of an expression for bitwise ops
fn infer_bitwise_width_from_expr<F: Fn(&CType) -> usize>(e: &Expr, sizeof_of: &F) -> usize {
    match e {
        Expr::Cast { ty, .. } => {
            sizeof_of(ty) * 8  // convert bytes to bits
        }
        _ => 64, // default to full width
    }
}

pub fn eval_ice_with_env<F: Fn(&CType) -> usize>(e: &Expr, sizeof_of: &F, env: &HashMap<String, i64>) -> Result<i64, String> {
    match e {
        Expr::IntLit(s, _) => parse_int_lit(s).ok_or_else(|| "invalid integer literal".to_string()),
        Expr::CharLit(s, _) => parse_char_lit(s).ok_or_else(|| "invalid character literal in ICE".to_string()),
        Expr::FloatLit(_, _) | Expr::StrLit(_, _) => Err("non-integer literal not allowed in ICE".into()),
        Expr::Ident(name, _) => env.get(name).copied().ok_or_else(|| format!("identifier '{}' not constant", name)),
        Expr::Unary { op, expr, .. } => {
            let v = eval_ice_with_env(expr, sizeof_of, env)?;
            match op {
                UnaryOp::Plus => Ok(v),
                UnaryOp::Minus => v.checked_neg().ok_or_else(|| "overflow".into()),
                UnaryOp::BitNot => {
                    // Apply BitNot with proper width truncation
                    let width = infer_bitwise_width_from_expr(expr, sizeof_of);
                    if width >= 64 {
                        Ok(!v)
                    } else {
                        let mask = (1u64 << width) - 1;
                        let truncated = (v as u64) & mask;
                        let result = (!truncated) & mask;
                        Ok(result as i64)
                    }
                },
                UnaryOp::LogNot => Ok((v == 0) as i64),
                _ => Err("invalid unary operator in ICE".into()),
            }
        }
        Expr::Binary { op, lhs, rhs, .. } => {
            let a = eval_ice_with_env(lhs, sizeof_of, env)?;
            let b = eval_ice_with_env(rhs, sizeof_of, env)?;
            
            // Check if either operand is unsigned for arithmetic operations
            let lhs_unsigned = expr_is_unsigned(lhs);
            let rhs_unsigned = expr_is_unsigned(rhs);
            let is_unsigned_op = lhs_unsigned || rhs_unsigned;
            
            match op {
                BinaryOp::Add => a.checked_add(b).ok_or_else(|| "overflow".into()),
                BinaryOp::Sub => a.checked_sub(b).ok_or_else(|| "overflow".into()),
                BinaryOp::Mul => a.checked_mul(b).ok_or_else(|| "overflow".into()),
                BinaryOp::Div => {
                    if b==0 { 
                        Err("division by zero".into()) 
                    } else if is_unsigned_op {
                        // Use unsigned division
                        let result = (a as u64) / (b as u64);
                        Ok(result as i64)
                    } else {
                        // Use signed division
                        Ok(a / b)
                    }
                },
                BinaryOp::Mod => {
                    if b==0 { 
                        Err("division by zero".into()) 
                    } else if is_unsigned_op {
                        // Use unsigned modulo
                        let result = (a as u64) % (b as u64);
                        Ok(result as i64)
                    } else {
                        // Use signed modulo
                        Ok(a % b)
                    }
                },
                BinaryOp::Shl => if b < 0 { Err("negative shift".into()) } else { a.checked_shl(b as u32).ok_or_else(|| "overflow".into()) },
                BinaryOp::Shr => {
                    if b < 0 { 
                        Err("negative shift".into()) 
                    } else if is_unsigned_op {
                        // Logical right shift for unsigned
                        Ok(((a as u64) >> (b as u32)) as i64)
                    } else {
                        // Arithmetic right shift for signed
                        Ok(a >> (b as u32))
                    }
                },
                BinaryOp::BitAnd => Ok(a & b),
                BinaryOp::BitXor => Ok(a ^ b),
                BinaryOp::BitOr => Ok(a | b),
                BinaryOp::Lt => {
                    if is_unsigned_op {
                        Ok(((a as u64) < (b as u64)) as i64)
                    } else {
                        Ok((a < b) as i64)
                    }
                },
                BinaryOp::Gt => {
                    if is_unsigned_op {
                        Ok(((a as u64) > (b as u64)) as i64)
                    } else {
                        Ok((a > b) as i64)
                    }
                },
                BinaryOp::Le => {
                    if is_unsigned_op {
                        Ok(((a as u64) <= (b as u64)) as i64)
                    } else {
                        Ok((a <= b) as i64)
                    }
                },
                BinaryOp::Ge => {
                    if is_unsigned_op {
                        Ok(((a as u64) >= (b as u64)) as i64)
                    } else {
                        Ok((a >= b) as i64)
                    }
                },
                BinaryOp::Eq => Ok((a == b) as i64),
                BinaryOp::Ne => Ok((a != b) as i64),
                BinaryOp::LogAnd => Ok(((a != 0) && (b != 0)) as i64),
                BinaryOp::LogOr => Ok(((a != 0) || (b != 0)) as i64),
                BinaryOp::Comma => Err("comma not allowed in ICE".into()),
            }
        }
        Expr::Cond { c, t, e, .. } => {
            let cc = eval_ice_with_env(c, sizeof_of, env)?;
            if cc != 0 { eval_ice_with_env(t, sizeof_of, env) } else { eval_ice_with_env(e, sizeof_of, env) }
        }
        Expr::Assign { .. } => Err("assignment not allowed in ICE".into()),
        Expr::Cast { ty, expr, .. } => {
            if !is_integer_type(ty) { return Err("cast to non-integer type in ICE".into()); }
            let v = eval_ice_with_env(expr, sizeof_of, env)?;
            // Truncate/extend value according to target type size and signedness
            match ty {
                CType::Char { signed } => {
                    let truncated = (v as u8) as i64;
                    if matches!(signed, Some(false)) {
                        // unsigned char: 0-255
                        Ok(truncated & 0xFF)
                    } else {
                        // signed char: -128 to 127 (sign-extend)
                        Ok((truncated as i8) as i64)
                    }
                }
                CType::Int { signed, long: _, short: _ } => {
                    let size_bytes = sizeof_of(ty);
                    let size_bits = size_bytes * 8;
                    
                    if size_bits >= 64 {
                        // 64-bit or larger: no truncation needed
                        Ok(v)
                    } else {
                        let mask = (1u64 << size_bits) - 1;
                        let truncated = v as u64 & mask;
                        
                        if matches!(signed, Some(false)) {
                            // unsigned: keep as-is
                            Ok(truncated as i64)
                        } else {
                            // signed: sign-extend if MSB is set
                            let sign_bit = 1u64 << (size_bits - 1);
                            if truncated & sign_bit != 0 {
                                // negative: sign-extend
                                Ok((truncated | (!mask)) as i64)
                            } else {
                                // positive: keep as-is
                                Ok(truncated as i64)
                            }
                        }
                    }
                }
                CType::Enum { .. } => {
                    // enum is int-compatible, truncate to 32 bits
                    let truncated = v as u32 as i64;
                    Ok(truncated)
                }
                _ => Ok(v)
            }
        }
        Expr::SizeOfType { ty, .. } => Ok(sizeof_of(ty) as i64),
        Expr::SizeOfExpr { expr, .. } => {
            if let Some(ty) = ice_type_of_expr_for_sizeof(expr) { Ok(sizeof_of(&ty) as i64) } else { Err("sizeof(expr) not supported in ICE yet".into()) }
        }
        Expr::Call { .. } | Expr::Index { .. } | Expr::Member { .. } | Expr::PtrMember { .. } |
        Expr::PostInc { .. } | Expr::PostDec { .. } | Expr::PreInc { .. } | Expr::PreDec { .. } => Err("non-constant operation in ICE".into()),
    }
}

pub fn eval_ice<F: Fn(&CType) -> usize>(e: &Expr, sizeof_of: &F) -> Result<i64, String> {
    let env = HashMap::new();
    eval_ice_with_env(e, sizeof_of, &env)
}
