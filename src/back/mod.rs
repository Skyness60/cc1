// ----------------------------
// File: src/back/mod.rs
// ----------------------------
#![allow(dead_code)]
//! Minimal LLVM IR emission (textual) for i386 sufficient for hello world and basic codegen.

use crate::front::ast::*;
use crate::front::semantics::{Arch, TargetInfo};
use std::collections::HashMap;

// Minimal IR value model for the textual emitter
#[derive(Clone, Debug)]
enum Value {
    Reg { ty: String, name: String },
    ConstInt { val: String },
    ConstGlobal(String), // e.g. @.str, @g
}

// Produce a zero value for an LLVM type string
fn zero_of(llty: &str) -> String {
    if llty == "ptr" { "ptr null".into() }
    else if llty.starts_with('[') || llty.starts_with('{') { format!("{} zeroinitializer", llty) }
    else { format!("{} 0", llty) }
}

pub struct IrModule {
    pub arch: Arch,
    pub text: String,
}

pub fn lower_to_llvm_ir(tu: &TranslationUnit, arch: Arch, debug: bool, infile: &str) -> IrModule {
    let mut cg = Codegen::new(arch, debug, infile);
    cg.gen_module(tu);
    IrModule { arch, text: cg.finish() }
}

struct Codegen {
    arch: Arch,
    debug: bool,
    infile: String,
    buf: Vec<String>,
    globals: Vec<String>,
    decls: HashMap<String, String>,
    str_table: HashMap<String, String>,
    str_index: usize,
    reg: usize,
    // locals: name -> (alloca_reg, llvm_ty)
    locals: HashMap<String, (String, String)>,
    // Track CType for locals/globals to drive typed GEP and pointer arithmetic
    locals_cty: HashMap<String, CType>,
    globals_cty: HashMap<String, CType>,
    // control flow stacks
    loop_stack: Vec<(String, String)>, // (continue, break)
    break_stack: Vec<String>,
    // switch contexts and labels/goto
    switch_stack: Vec<SwitchCtx>,
    label_map: HashMap<String, String>,
    // Track LLVM types of globals (e.g., "[N x i8]", "[N x i32]", "{...}") for typed GEP
    global_types: HashMap<String, String>,
    // Cache for struct type definitions to prevent redefinitions
    defined_struct_types: HashMap<String, String>,
    // Current function ABI lowering state
    current_fn_is_sret: bool,
    sret_param_name: Option<String>,
    current_ret_ctype: Option<CType>,
    // Enum constants environment for constant evaluation
    enum_constants: HashMap<String, i64>,
}

struct SwitchCtx {
    discr_reg: String, // i32 value in a register
    end_label: String,
    // value -> label
    cases: HashMap<i64, String>,
    default_label: String,
    default_emitted: bool,
}

impl Codegen {
    fn new(arch: Arch, debug: bool, infile: &str) -> Self {
        Self {
            arch,
            debug,
            infile: infile.to_string(),
            buf: Vec::new(),
            globals: Vec::new(),
            decls: HashMap::new(),
            str_table: HashMap::new(),
            str_index: 0,
            reg: 1,
            locals: HashMap::new(),
            locals_cty: HashMap::new(),
            globals_cty: HashMap::new(),
            loop_stack: Vec::new(),
            break_stack: Vec::new(),
            switch_stack: Vec::new(),
            label_map: HashMap::new(),
            global_types: HashMap::new(),
            defined_struct_types: HashMap::new(),
            current_fn_is_sret: false,
            sret_param_name: None,
            current_ret_ctype: None,
            enum_constants: HashMap::new(),
        }
    }

    fn new_reg(&mut self) -> String { let r = self.reg; self.reg += 1; format!("%{}", r) }
    fn new_label(&mut self, base: &str) -> String { let r = self.reg; self.reg += 1; format!("{}.{}", base, r) }
    fn emit<S: Into<String>>(&mut self, s: S) { self.buf.push(s.into()); }

    fn ensure_named_label(&mut self, name: &str) -> String {
        if let Some(l) = self.label_map.get(name) { return l.clone(); }
        let l = self.new_label(&format!("label.{}", name));
        self.label_map.insert(name.to_string(), l.clone());
        l
    }

    fn start_new_block(&mut self, base: &str) {
        let l = self.new_label(base);
        self.emit(format!("{}:", l));
    }

    fn dl(&self) -> (&'static str, &'static str) {
        match self.arch {
            Arch::I386 => (
                "target datalayout = \"e-m:e-p:32:32-f64:32:64-f80:32-n8:16:32-S128\"",
                "target triple = \"i386-pc-linux-gnu\"",
            ),
            Arch::X86_64 => (
                "target datalayout = \"e-m:e-i64:64-f80:128-n8:16:32:64-S128\"",
                "target triple = \"x86_64-pc-linux-gnu\"",
            ),
        }
    }

    fn finish(&mut self) -> String {
        let (dl, triple) = self.dl();
        let mut out = String::new();
        if !dl.is_empty() { out.push_str(dl); out.push('\n'); }
        out.push_str(triple); out.push('\n');
        
        // Add debug info metadata if debug is enabled
        if self.debug {
            self.emit_debug_metadata(&mut out);
        }
        
        // Add debug intrinsic declarations if debug is enabled
        if self.debug {
            out.push_str("declare void @llvm.dbg.declare(metadata, metadata, metadata)\n");
        }
        
        for (_n, d) in &self.decls { out.push_str(d); out.push('\n'); }
        for g in &self.globals { out.push_str(g); out.push('\n'); }
        for l in &self.buf { out.push_str(l); out.push('\n'); }
        out
    }

    fn emit_debug_metadata(&self, out: &mut String) {
        // Complete DWARF debug information for LLVM IR
        
        // Debug info version and flags
        out.push_str("!llvm.module.flags = !{!0, !1, !2}\n");
        out.push_str("!llvm.dbg.cu = !{!3}\n");
        out.push_str("!llvm.ident = !{!4}\n");
        
        // Module flags
        out.push_str("!0 = !{i32 2, !\"Debug Info Version\", i32 3}\n");
        out.push_str("!1 = !{i32 2, !\"Dwarf Version\", i32 4}\n");
        out.push_str("!2 = !{i32 2, !\"wchar_size\", i32 4}\n");
        out.push_str("!4 = !{!\"cc1 C89 compiler\"}\n");
        
        // File and directory info
        let filename = std::path::Path::new(&self.infile)
            .file_name()
            .and_then(|n| n.to_str())
            .unwrap_or(&self.infile);
        let directory = std::env::current_dir()
            .ok()
            .and_then(|p| p.to_str().map(|s| s.to_string()))
            .unwrap_or_else(|| ".".to_string());
        
        // Compile unit with complete metadata
        out.push_str(&format!(
            "!3 = distinct !DICompileUnit(language: DW_LANG_C89, file: !5, producer: \"cc1 (C89 compiler)\", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugFilename: \"\", nameTableKind: None)\n"
        ));
        
        // File metadata
        out.push_str(&format!(
            "!5 = !DIFile(filename: \"{}\", directory: \"{}\")\n",
            filename, directory.replace("\\", "\\\\")
        ));
        
        // Basic types for debugging
        self.emit_basic_debug_types(out);
    }

    fn emit_basic_debug_types(&self, out: &mut String) {
        let ptr_size = match self.arch {
            Arch::I386 => 32,
            Arch::X86_64 => 64,
        };
        
        // Basic C89 types
        out.push_str("!6 = !DIBasicType(name: \"int\", size: 32, encoding: DW_ATE_signed)\n");
        out.push_str("!7 = !DIBasicType(name: \"char\", size: 8, encoding: DW_ATE_signed_char)\n");
        out.push_str("!8 = !DIBasicType(name: \"short\", size: 16, encoding: DW_ATE_signed)\n");
        out.push_str("!9 = !DIBasicType(name: \"long\", size: 32, encoding: DW_ATE_signed)\n");
        out.push_str("!10 = !DIBasicType(name: \"unsigned int\", size: 32, encoding: DW_ATE_unsigned)\n");
        out.push_str("!11 = !DIBasicType(name: \"unsigned char\", size: 8, encoding: DW_ATE_unsigned_char)\n");
        out.push_str("!12 = !DIBasicType(name: \"unsigned short\", size: 16, encoding: DW_ATE_unsigned)\n");
        out.push_str("!13 = !DIBasicType(name: \"unsigned long\", size: 32, encoding: DW_ATE_unsigned)\n");
        out.push_str("!14 = !DIBasicType(name: \"float\", size: 32, encoding: DW_ATE_float)\n");
        out.push_str("!15 = !DIBasicType(name: \"double\", size: 64, encoding: DW_ATE_float)\n");
        out.push_str("!16 = !DIBasicType(name: \"long double\", size: 80, encoding: DW_ATE_float)\n");
        out.push_str(&format!("!17 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: {})\n", ptr_size));
        
        // void type
        out.push_str("!18 = !DIBasicType(name: \"void\", encoding: DW_ATE_address)\n");
    }

    fn emit_function_debug_info(&mut self, func: &FuncDef, _func_start_label: &str) {
        if !self.debug { return; }
        
        let di_func = format!("!{}", self.reg);
        self.reg += 1;
        
        // Function debug info
        self.globals.push(format!(
            "{} = distinct !DISubprogram(name: \"{}\", scope: !5, file: !5, line: 1, type: !{}, scopeLine: 1, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !3)\n",
            di_func, func.name, self.reg
        ));
        
        // Function type (simplified)
        let func_type = format!("!{}", self.reg);
        self.reg += 1;
        self.globals.push(format!(
            "{} = !DISubroutineType(types: !{})\n",
            func_type, self.reg
        ));
        
        // Function type array (return type + parameters)
        let type_array = format!("!{}", self.reg);
        self.reg += 1;
        self.globals.push(format!("{} = !{{!6}}\n", type_array)); // Simple: int return type
        
        // Emit debug location at function start
        self.emit(&format!("call void @llvm.dbg.declare(metadata ptr undef, metadata {}, metadata !DIExpression()), !dbg !{}", 
                          di_func, self.reg));
        self.reg += 1;
    }

    fn emit_variable_debug_info(&mut self, name: &str, alloca_reg: &str, _ctype: &CType) {
        if !self.debug { return; }
        
        let di_var = format!("!{}", self.reg);
        self.reg += 1;
        
        // Variable debug info
        self.globals.push(format!(
            "{} = !DILocalVariable(name: \"{}\", scope: !5, file: !5, line: 1, type: !6)\n",
            di_var, name
        ));
        
        // Declare variable location
        self.emit(&format!("call void @llvm.dbg.declare(metadata ptr {}, metadata {}, metadata !DIExpression())", 
                          alloca_reg, di_var));
    }

    fn gen_module(&mut self, tu: &TranslationUnit) {
        // First pass: collect enum constants
        self.collect_enum_constants(tu);
        
        // Decls/globals
        for it in &tu.items {
            match it {
                Top::Decl(d) => self.gen_global_decl(d),
                Top::Decls(ds) => for d in ds { self.gen_global_decl(d); },
                _ => {}
            }
        }
        // Funcs
        for it in &tu.items {
            if let Top::Func(f) = it { self.gen_func(f); }
        }
    }

    fn collect_enum_constants(&mut self, tu: &TranslationUnit) {
        for it in &tu.items {
            if let Top::Tag(CType::Enum { items: Some(items), .. }, _) = it {
                let mut next = 0i64;
                let ti = self.target_info();
                for (name, opt_expr) in items {
                    if let Some(e) = opt_expr {
                        let sz = |t: &CType| ti.sizeof(t).unwrap_or(0);
                        if let Ok(val) = crate::front::semantics::eval_ice_with_env(e, &sz, &self.enum_constants) {
                            next = val;
                        }
                    }
                    self.enum_constants.insert(name.clone(), next);
                    next = next.saturating_add(1);
                }
            }
        }
    }

    fn target_info(&self) -> TargetInfo {
        match self.arch { Arch::I386 => TargetInfo::i386(), Arch::X86_64 => TargetInfo::x86_64() }
    }

    fn ty(&self, ty: &CType) -> String {
        match ty {
            CType::Void => "void".into(),
            CType::Char { .. } => "i8".into(),
            CType::Int { long, short, .. } => {
                match self.arch {
                    Arch::I386 => { if *short { "i16".into() } else { "i32".into() } }
                    Arch::X86_64 => { if *short { "i16".into() } else if *long { "i64".into() } else { "i32".into() } }
                }
            }
            CType::Float => "float".into(),
            // Map long double to the target-specific 80-bit extended type
            CType::Double { long } => if *long { "x86_fp80".into() } else { "double".into() },
            CType::Pointer { .. } | CType::Array { .. } | CType::Function { .. } => "ptr".into(),
            CType::Struct { .. } | CType::Union { .. } | CType::Enum { .. } | CType::Named(_) => "ptr".into(),
        }
    }

    fn llvm_byval_pointee(&self, ty: &CType) -> String {
        match ty {
            CType::Array { of, size: Some(n) } => format!("[{} x {}]", n, self.ty(of)),
            CType::Array { of, size: None } => format!("[0 x {}]", self.ty(of)),
            CType::Struct { fields: Some(fs), .. } => {
                let mut ftys: Vec<String> = Vec::with_capacity(fs.len());
                for f in fs { match &f.ty { CType::Array { of, size: Some(n) } => ftys.push(format!("[{} x {}]", n, self.ty(of))), _ => ftys.push(self.ty(&f.ty)), } }
                format!("{{ {} }}", ftys.join(", "))
            }
            // Conservative fallback for unions/unknown: treat as opaque bytes
            CType::Union { .. } => "i8".into(),
            other => self.ty(other),
        }
    }

    fn func_sig(&self, name: &str, fty: &CType) -> (String, String, bool) {
        if let CType::Function { ret, params, varargs, .. } = fty {
            // Handle aggregate returns via sret
            if Self::is_aggregate(ret) {
                let mut ps: Vec<String> = Vec::new();
                let rt = self.llvm_byval_pointee(ret);
                ps.push(format!("ptr sret({})", rt));
                for p in params {
                    if matches!(p.ty, CType::Struct { .. } | CType::Union { .. }) {
                        let bt = self.llvm_byval_pointee(&p.ty);
                        ps.push(format!("ptr byval({})", bt));
                    } else {
                        ps.push(self.ty(&p.ty));
                    }
                }
                let sig = if *varargs {
                    if ps.len() == 1 { format!("void @{}(...)", name) } else { format!("void @{}({}, ...)", name, ps.join(", ")) }
                } else {
                    if ps.is_empty() { format!("void @{}()", name) } else { format!("void @{}({})", name, ps.join(", ")) }
                };
                return (sig, "void".into(), *varargs);
            }
            // Non-aggregate return
            let r = self.ty(ret);
            let mut ps: Vec<String> = Vec::new();
            for p in params {
                if matches!(p.ty, CType::Struct { .. } | CType::Union { .. }) {
                    let bt = self.llvm_byval_pointee(&p.ty);
                    ps.push(format!("ptr byval({})", bt));
                } else {
                    ps.push(self.ty(&p.ty));
                }
            }
            if *varargs {
                if ps.is_empty() { (format!("{} @{}(...)", r, name), r, true) }
                else { (format!("{} @{}({}, ...)", r, name, ps.join(", ")), r, true) }
            } else {
                if ps.is_empty() { (format!("{} @{}()", r, name), r, false) }
                else { (format!("{} @{}({})", r, name, ps.join(", ")), r, false) }
            }
        } else {
            (format!("i32 @{}()", name), "i32".into(), false)
        }
    }

    // Helper: lookup a known function type by name
    fn fn_type_of_name(&self, name: &str) -> Option<&CType> {
        self.globals_cty.get(name).and_then(|t| match t { CType::Function { .. } => Some(t), _ => None })
    }

    // Assure une déclaration LLVM pour une fonction appelée.
    // Utilise le prototype connu si disponible, sinon une déclaration varargs générique.
    fn ensure_decl_for_call(&mut self, name: &str) {
        if self.decls.contains_key(name) { return; }
        if let Some(fty) = self.fn_type_of_name(name) {
            if let CType::Function { ret, params, varargs, .. } = fty {
                // Handle sret when returning aggregate
                if Self::is_aggregate(ret) {
                    let mut ps: Vec<String> = Vec::new();
                    let rt = self.llvm_byval_pointee(ret);
                    ps.push(format!("ptr sret({})", rt));
                    for p in params {
                        if matches!(p.ty, CType::Struct { .. } | CType::Union { .. }) {
                            let bt = self.llvm_byval_pointee(&p.ty);
                            ps.push(format!("ptr byval({})", bt));
                        } else {
                            ps.push(self.ty(&p.ty));
                        }
                    }
                    let sig = if *varargs {
                        if ps.len() == 1 { format!("void @{}(...)", name) } else { format!("void @{}({}, ...)", name, ps.join(", ")) }
                    } else {
                        if ps.is_empty() { format!("void @{}()", name) } else { format!("void @{}({})", name, ps.join(", ")) }
                    };
                    self.decls.insert(name.to_string(), format!("declare {}", sig));
                    return;
                }
                // Non-aggregate return
                let r = self.ty(ret);
                let mut ps: Vec<String> = Vec::new();
                for p in params {
                    if matches!(p.ty, CType::Struct { .. } | CType::Union { .. }) {
                        let bt = self.llvm_byval_pointee(&p.ty);
                        ps.push(format!("ptr byval({})", bt));
                    } else {
                        ps.push(self.ty(&p.ty));
                    }
                }
                let sig = if *varargs {
                    if ps.is_empty() { format!("{} @{}(...)", r, name) } else { format!("{} @{}({}, ...)", r, name, ps.join(", ")) }
                } else {
                    if ps.is_empty() { format!("{} @{}()", r, name) } else { format!("{} @{}({})", r, name, ps.join(", ")) }
                };
                self.decls.insert(name.to_string(), format!("declare {}", sig));
            }
        } else {
            self.decls.insert(name.to_string(), format!("declare i32 @{}(...)", name));
        }
    }

    fn decode_c_string(raw: &str) -> Vec<u8> {
        // raw includes surrounding quotes. Decode C escapes into bytes.
        let bytes = raw.as_bytes();
        let mut out: Vec<u8> = Vec::new();
        if bytes.len() >= 2 && bytes[0] == b'"' && bytes[bytes.len()-1] == b'"' {
            let mut i = 1usize;
            while i + 1 < bytes.len() {
                let c = bytes[i];
                if c == b'\\' {
                    i += 1;
                    if i + 0 >= bytes.len() { break; }
                    match bytes[i] {
                        b'\\' => out.push(b'\\'),
                        b'"' => out.push(b'"'),
                        b'\'' => out.push(b'\''),
                        b'?' => out.push(b'?'),
                        b'a' => out.push(7),
                        b'b' => out.push(8),
                        b'f' => out.push(12),
                        b'n' => out.push(10),
                        b'r' => out.push(13),
                        b't' => out.push(9),
                        b'v' => out.push(11),
                        b'x' => {
                            // \x[1+ hex]
                            let mut j = i + 1; let mut val: u32 = 0; let mut saw = false;
                            while j < bytes.len() {
                                let ch = bytes[j];
                                let dig = match ch {
                                    b'0'..=b'9' => Some((ch - b'0') as u32),
                                    b'a'..=b'f' => Some((10 + (ch - b'a') as u32) as u32),
                                    b'A'..=b'F' => Some((10 + (ch - b'A') as u32) as u32),
                                    _ => None,
                                };
                                if let Some(d) = dig { val = (val << 4) | d; j += 1; saw = true; } else { break; }
                            }
                            if saw { out.push((val & 0xFF) as u8); i = j - 1; }
                        }
                        b'0'..=b'7' => {
                            // up to 3 octal digits, including the one we just saw
                            let mut j = i; let mut val: u32 = 0; let mut k = 0;
                            while j < bytes.len() && k < 3 {
                                let ch = bytes[j];
                                if (b'0'..=b'7').contains(&ch) { val = (val << 3) | ((ch - b'0') as u32); j += 1; k += 1; } else { break; }
                            }
                            out.push((val & 0xFF) as u8);
                            i = j - 1;
                        }
                        other => { out.push(other); }
                    }
                } else if c == b'"' {
                    // end
                    break;
                } else {
                    out.push(c);
                }
                i += 1;
            }
        } else {
            // no quotes; push raw as bytes
            out.extend_from_slice(bytes);
        }
        out
    }

    fn ensure_str_global(&mut self, s: &str) -> String {
        if let Some(n) = self.str_table.get(s) { return n.clone(); }
        // Decode C string literal (strip quotes and escapes)
        let mut data = Self::decode_c_string(s);
        data.push(0); // NUL-terminate
        let nbytes = data.len();
        let name = if self.str_index == 0 { "@str".to_string() } else { format!("@.str.{}", self.str_index) };
        self.str_index += 1;
        let elems: Vec<String> = data.iter().map(|b| format!("i8 {}", b)).collect();
        let g = format!("{} = private global [{} x i8] [{}]", name, nbytes, elems.join(", "));
        self.globals.push(g);
        // record its LLVM type for typed GEP
        self.global_types.insert(name.clone(), format!("[{} x i8]", nbytes));
        self.str_table.insert(s.to_string(), name.clone());
        name
    }

    fn gen_global_decl(&mut self, d: &Decl) {
        // Skip typedef-only declarations
        if matches!(d.storage, Some(StorageClass::Typedef)) { return; }
        // Record CType for global
        self.globals_cty.insert(d.name.clone(), d.ty.clone());
        if let CType::Function { .. } = &d.ty {
            let (sig, _r, _v) = self.func_sig(&d.name, &d.ty);
            self.decls.entry(d.name.clone()).or_insert_with(|| format!("declare {}", sig));
            return;
        }
        // Handle aggregate globals (arrays) specially with typed initializers
        if let CType::Array { of, size } = &d.ty {
            // Determine element LLVM type
            let elem_llty = self.ty(of);
            // Compute effective array size from type or initializer
            let eff_size: Option<usize> = *size;
            let mut pushed = false;
            if let Some(init) = &d.init {
                match init {
                    Initializer::Expr(Expr::StrLit(s, _)) => {
                        // char[] from string literal
                        if matches!(**of, CType::Char{..}) {
                            let mut bytes = Self::decode_c_string(s);
                            let len = bytes.len();
                            let n = eff_size.unwrap_or(len + 1);
                            if n < len { /* semantic layer should have errored; clamp */ }
                            // If implicit size, include a trailing NUL; if explicit and n>len, zero-fill handles it
                            if eff_size.is_none() { bytes.push(0); }
                            // Zero-pad to n
                            if n > bytes.len() { bytes.resize(n, 0); }
                            let elems: Vec<String> = bytes.into_iter().map(|b| format!("i8 {}", b)).collect();
                            self.globals.push(format!("@{} = global [{} x i8] [{}]", d.name, n, elems.join(", ")));
                            // record global type
                            self.global_types.insert(format!("@{}", d.name), format!("[{} x i8]", n));
                            pushed = true;
                        }
                    }
                    Initializer::List(list) => {
                        // Array from list; infer size if missing
                        let n = eff_size.unwrap_or(list.len());
                        let mut elems: Vec<String> = Vec::with_capacity(n);
                        for i in 0..n {
                            if i < list.len() {
                                match &list[i] {
                                    Initializer::Expr(Expr::IntLit(v, _)) => {
                                        elems.push(format!("{} {}", elem_llty, v));
                                    }
                                    Initializer::Expr(Expr::CharLit(cv, _)) if elem_llty == "i8" => {
                                        // crude: use first byte of decoded char literal string content if possible
                                        let b = cv.bytes().next().unwrap_or(b'\0');
                                        elems.push(format!("i8 {}", b));
                                    }
                                    _ => {
                                        // unsupported element expr: zero
                                        elems.push(format!("{} 0", elem_llty));
                                    }
                                }
                            } else {
                                elems.push(format!("{} 0", elem_llty));
                            }
                        }
                        self.globals.push(format!("@{} = global [{} x {}] [{}]", d.name, n, elem_llty, elems.join(", ")));
                        // record global type
                        self.global_types.insert(format!("@{}", d.name), format!("[{} x {}]", n, elem_llty));
                        pushed = true;
                    }
                    _ => { /* unsupported initializer for arrays: leave to default path */ }
                }
            }
            if !pushed {
                // No init or unsupported form: emit zeroinitializer if we know the size
                if let Some(n) = eff_size {
                    self.globals.push(format!("@{} = global [{} x {}] zeroinitializer", d.name, n, elem_llty));
                    self.global_types.insert(format!("@{}", d.name), format!("[{} x {}]", n, elem_llty));
                } else {
                    // Fallback: old path (opaque)
                    let llty = self.ty(&d.ty);
                    self.globals.push(format!("@{} = global {} 0", d.name, llty));
                    self.global_types.insert(format!("@{}", d.name), llty);
                }
            }
            return;
        }
        // Handle struct globals with initializer lists
        if let CType::Struct { fields: Some(fs), .. } = &d.ty {
            // Build LLVM struct type from field types (support arrays of known size)
            let mut ftys: Vec<String> = Vec::with_capacity(fs.len());
            for f in fs {
                let t = match &f.ty {
                    CType::Array { of, size: Some(n) } => format!("[{} x {}]", n, self.ty(of)),
                    _ => self.ty(&f.ty),
                };
                ftys.push(t);
            }
            let mut inits: Vec<String> = Vec::with_capacity(ftys.len());
            if let Some(Initializer::List(list)) = &d.init {
                for i in 0..ftys.len() {
                    let llty = &ftys[i];
                    let val = if i < list.len() {
                        match &list[i] {
                            Initializer::Expr(Expr::IntLit(v, _)) => format!("{} {}", llty, v),
                            Initializer::Expr(Expr::StrLit(s, _)) => {
                                if llty == "ptr" { let g = self.ensure_str_global(s); format!("ptr {}", g) }
                                else if llty.starts_with('[') && llty.contains("x i8]") {
                                    // char array field from string
                                    // extract n between [ and x
                                    let n = llty.trim_start_matches('[').split(' ').next().and_then(|k| k.parse::<usize>().ok()).unwrap_or(0);
                                    let mut bytes = Self::decode_c_string(s);
                                    // If fits and type implies array of i8
                                    if n > 0 {
                                        if bytes.len() < n { bytes.resize(n, 0); }
                                        else if bytes.len() > n { bytes.truncate(n); }
                                        let elems: Vec<String> = bytes.into_iter().map(|b| format!("i8 {}", b)).collect();
                                        format!("[{} x i8] [{}]", n, elems.join(", "))
                                    } else { format!("{} zeroinitializer", llty) }
                                } else { format!("{} zeroinitializer", llty) }
                            }
                            _ => zero_of(llty),
                        }
                    } else { zero_of(llty) };
                    inits.push(val);
                }
            } else {
                for llty in &ftys { inits.push(zero_of(llty)); }
            }
            let ty_str = format!("{{ {} }}", ftys.join(", "));
            let init_str = format!("{{ {} }}", inits.join(", "));
            self.globals.push(format!("@{} = global {} {}", d.name, ty_str, init_str));
            // record global type
            self.global_types.insert(format!("@{}", d.name), ty_str);
            return;
        }
        // Union global: emit [max_field_size x i8] with first field's initializer
        if let CType::Union { fields: Some(fs), .. } = &d.ty {
            let ti = match self.arch { Arch::I386 => crate::front::semantics::TargetInfo::i386(), Arch::X86_64 => crate::front::semantics::TargetInfo::x86_64() };
            let max_size = fs.iter().map(|f| ti.sizeof(&f.ty).unwrap_or(0)).max().unwrap_or(1);
            let mut bytes = vec![0u8; max_size];
            if let Some(Initializer::List(list)) = &d.init {
                if !list.is_empty() {
                    let f = &fs[0];
                    match &list[0] {
                        Initializer::Expr(Expr::IntLit(v, _)) => {
                            let val = v.parse::<i64>().unwrap_or(0);
                            let sz = ti.sizeof(&f.ty).unwrap_or(4);
                            for i in 0..sz.min(max_size) {
                                bytes[i] = ((val >> (i*8)) & 0xFF) as u8;
                            }
                        }
                        Initializer::Expr(Expr::CharLit(cv, _)) => {
                            bytes[0] = cv.bytes().next().unwrap_or(0);
                        }
                        Initializer::Expr(Expr::StrLit(s, _)) if matches!(f.ty, CType::Char{..}) => {
                            let str_bytes = Self::decode_c_string(s);
                            let sz = max_size.min(str_bytes.len());
                            bytes[..sz].copy_from_slice(&str_bytes[..sz]);
                        }
                        _ => {}
                    }
                }
            }
            let elems: Vec<String> = bytes.into_iter().map(|b| format!("i8 {}", b)).collect();
            self.globals.push(format!("@{} = global [{} x i8] [{}]", d.name, max_size, elems.join(", ")));
            self.global_types.insert(format!("@{}", d.name), format!("[{} x i8]", max_size));
            return;
        }
        // simple variable global (scalars/pointers)
        let llty = self.ty(&d.ty);
        let mut init = String::new();
        if let Some(Initializer::Expr(e)) = &d.init {
            match e {
                Expr::IntLit(v, _) => init = format!("{} {}", llty, v),
                Expr::StrLit(s, _) => { let g = self.ensure_str_global(s); init = format!("ptr {}", g); }
                _ => {
                    // Try to evaluate as a constant expression
                    let ti = self.target_info();
                    let sz = |t: &CType| ti.sizeof(t).unwrap_or(0);
                    // Use the enum constants environment we collected
                    if let Ok(val) = crate::front::semantics::eval_ice_with_env(e, &sz, &self.enum_constants) {
                        init = format!("{} {}", llty, val);
                    }
                }
            }
        }
        if init.is_empty() { init = format!("{} 0", llty); }
        self.globals.push(format!("@{} = global {}", d.name, init));
        // record global type for scalars as their LLVM type
        self.global_types.insert(format!("@{}", d.name), llty);
    }

    fn gen_func(&mut self, f: &FuncDef) {
        // remember function type for calls
        self.globals_cty.insert(f.name.clone(), f.ty.clone());

        // reset function state
        self.reg = 1;
        self.locals.clear();
        self.locals_cty.clear();
        self.loop_stack.clear();
        self.break_stack.clear();
        self.switch_stack.clear();
        self.label_map.clear();
        self.current_fn_is_sret = false;
        self.sret_param_name = None;
        self.current_ret_ctype = None;

        // Emit function debug info if debug is enabled
        if self.debug {
            self.emit_function_debug_info(f, "entry");
        }

        // Build definition signature with named params (handle sret and byval)
        let (ret_ty_str, is_varargs, ret_ctype, params_cty): (String, bool, CType, Vec<CType>) = match &f.ty {
            CType::Function { ret, params, varargs, .. } => {
                let is_aggr = Self::is_aggregate(ret);
                let r = if is_aggr { "void".into() } else { self.ty(ret) };
                (r, *varargs, (*ret.clone()), params.iter().map(|p| p.ty.clone()).collect())
            }
            _ => ("i32".into(), false, CType::Int { signed: None, long: false, short: false }, Vec::new()),
        };
        self.current_ret_ctype = Some(ret_ctype.clone());
        let ret_is_aggr = Self::is_aggregate(&ret_ctype);
        if ret_is_aggr { self.current_fn_is_sret = true; }

        let mut param_defs: Vec<String> = Vec::new();
        let mut param_regs: Vec<String> = Vec::new();
        if self.current_fn_is_sret {
            let preg = "%sret".to_string();
            let rt = self.llvm_byval_pointee(&ret_ctype);
            param_defs.push(format!("ptr sret({}) %sret", rt));
            param_regs.push(preg.clone());
            self.sret_param_name = Some(preg);
        }
        for (i, pty) in params_cty.iter().enumerate() {
            let preg = format!("%arg{}", i);
            if matches!(pty, CType::Struct { .. } | CType::Union { .. }) {
                let bt = self.llvm_byval_pointee(pty);
                param_defs.push(format!("ptr byval({}) {}", bt, preg));
            } else {
                let llty = self.ty(pty);
                param_defs.push(format!("{} {}", llty, preg));
            }
            param_regs.push(preg);
        }
        let sig = if is_varargs {
            if param_defs.is_empty() { format!("{} @{}(...)", ret_ty_str, f.name) }
            else { format!("{} @{}({}, ...)", ret_ty_str, f.name, param_defs.join(", ")) }
        } else {
            format!("{} @{}({})", ret_ty_str, f.name, param_defs.join(", "))
        };

        self.emit(format!("define {} {{", sig));
        self.emit("entry:");

        // Bind parameters to locals
        for (i, p) in f.params.iter().enumerate() {
            if let Some(name) = &p.name {
                if matches!(p.ty, CType::Struct { .. } | CType::Union { .. }) {
                    let preg = if self.current_fn_is_sret { &param_regs[i+1] } else { &param_regs[i] };
                    self.locals.insert(name.clone(), (preg.clone(), "ptr".into()));
                    self.locals_cty.insert(name.clone(), p.ty.clone());
                } else {
                    let alloca = self.new_reg();
                    let llty = self.ty(&p.ty);
                    self.emit(format!("  {} = alloca {}", alloca, llty));
                    let preg = if self.current_fn_is_sret { &param_regs[i+1] } else { &param_regs[i] };
                    self.emit(format!("  store {} {}, ptr {}", llty, preg, alloca));
                    self.locals.insert(name.clone(), (alloca.clone(), llty));
                    self.locals_cty.insert(name.clone(), p.ty.clone());
                }
            }
        }

        let mut ended = false;
        self.gen_stmt(&f.body, &mut ended, ret_ty_str.as_str() == "i32" && !self.current_fn_is_sret);
        if !ended {
            if self.current_fn_is_sret || ret_ty_str == "void" { self.emit("  ret void"); } else { self.emit("  ret i32 0"); }
        }
        self.emit("}");
    }

    fn is_aggregate(ty: &CType) -> bool {
        matches!(ty, CType::Struct { .. } | CType::Union { .. } | CType::Array { .. })
    }
    // Byte-wise copy helper for aggregates (size in bytes)
    fn copy_block(&mut self, dst_ptr: &str, src_ptr: &str, size: usize) {
        if size == 0 { return; }
        let idx = self.new_reg();
        self.emit(format!("  {} = add i32 0, 0", idx));
        let loop_l = self.new_label("copy.loop");
        let body_l = self.new_label("copy.body");
        let end_l  = self.new_label("copy.end");
        self.emit(format!("  br label %{}", loop_l));
        self.emit(format!("{}:", loop_l));
        let cond = self.new_reg();
        self.emit(format!("  {} = icmp ult i32 {}, {}", cond, idx, size));
        self.emit(format!("  br i1 {}, label %{}, label %{}", cond, body_l, end_l));
        self.emit(format!("{}:", body_l));
        let dptr = self.new_reg();
        self.emit(format!("  {} = getelementptr i8, ptr {}, i32 {}", dptr, dst_ptr, idx));
        let sptr = self.new_reg();
        self.emit(format!("  {} = getelementptr i8, ptr {}, i32 {}", sptr, src_ptr, idx));
        let byte = self.new_reg();
        self.emit(format!("  {} = load i8, ptr {}", byte, sptr));
        self.emit(format!("  store i8 {}, ptr {}", byte, dptr));
        let next = self.new_reg();
        self.emit(format!("  {} = add i32 {}, 1", next, idx));
        self.emit(format!("  br label %{}", loop_l));
        self.emit(format!("{}:", end_l));
    }

    fn gen_stmt(&mut self, s: &Stmt, ended: &mut bool, default_ret_i32: bool) {
        if *ended { return; }
        match s {
            Stmt::Compound { items, .. } => {
                for it in items {
                    match it {
                        BlockItem::Stmt(ss) => self.gen_stmt(ss, ended, default_ret_i32),
                        BlockItem::Decl(d) => self.gen_local_decl(d),
                    }
                    // do not stop on branches like break/goto; only on function return
                    if *ended { break; }
                }
            }
            Stmt::Expr(e, _) => { let _ = self.gen_expr(e); }
            Stmt::Return(opt, _) => {
                if self.current_fn_is_sret {
                    // Copy aggregate value to sret buffer then ret void
                    if let Some(e) = opt {
                        let src = self.gen_expr(e);
                        // Expect a pointer to aggregate memory
                        let src_ptr = match src { Value::Reg { ty, name } if ty == "ptr" => name, Value::ConstGlobal(g) => g, other => {
                            let (_t, r) = self.as_reg(other); let p = self.new_reg(); self.emit(format!("  {} = inttoptr i32 {} to ptr", p, r)); p }
                        };
                        let dst_ptr = self.sret_param_name.clone().unwrap_or_else(|| "%sret".into());
                        let ti = self.target_info();
                        let sz = self.current_ret_ctype.as_ref().and_then(|t| ti.sizeof(t).ok()).unwrap_or(0);
                        self.copy_block(&dst_ptr, &src_ptr, sz);
                    }
                    self.emit("  ret void");
                    *ended = true;
                } else {
                    if let Some(e) = opt {
                        let v = self.gen_expr(e);
                        match v {
                            Value::Reg { ty, name } => self.emit(format!("  ret {} {}", ty, name)),
                            Value::ConstInt { val } => self.emit(format!("  ret i32 {}", val)),
                            Value::ConstGlobal(g) => self.emit(format!("  ret ptr {}", g)),
                        }
                    } else {
                        if default_ret_i32 { self.emit("  ret i32 0"); } else { self.emit("  ret void"); }
                    }
                    *ended = true;
                }
            }
            Stmt::If { c, t, e, .. } => {
                let cval = self.gen_expr(c);
                let cond = self.to_i1(cval);
                let then_l = self.new_label("then");
                let else_l = self.new_label("else");
                let end_l = self.new_label("endif");
                self.emit(format!("  br i1 {}, label %{}, label %{}", cond, then_l, else_l));
                self.emit(format!("{}:", then_l));
                self.gen_stmt(t, ended, default_ret_i32);
                if !*ended { self.emit(format!("  br label %{}", end_l)); }
                self.emit(format!("{}:", else_l));
                if let Some(ee) = e { self.gen_stmt(ee, ended, default_ret_i32); }
                if !*ended { self.emit(format!("  br label %{}", end_l)); }
                self.emit(format!("{}:", end_l));
            }
            Stmt::While { c, body, .. } => {
                let head = self.new_label("while.cond");
                let loopb = self.new_label("while.body");
                let end = self.new_label("while.end");
                self.emit(format!("  br label %{}", head));
                self.emit(format!("{}:", head));
                let cval = self.gen_expr(c);
                let cond = self.to_i1(cval);
                self.emit(format!("  br i1 {}, label %{}, label %{}", cond, loopb, end));
                self.emit(format!("{}:", loopb));
                self.loop_stack.push((head.clone(), end.clone()));
                self.break_stack.push(end.clone());
                self.gen_stmt(body, ended, default_ret_i32);
                self.break_stack.pop(); self.loop_stack.pop();
                if !*ended { self.emit(format!("  br label %{}", head)); }
                self.emit(format!("{}:", end));
            }
            Stmt::DoWhile { c, body, .. } => {
                let loopb = self.new_label("do.body");
                let head = self.new_label("do.cond");
                let end = self.new_label("do.end");
                self.emit(format!("  br label %{}", loopb));
                self.emit(format!("{}:", loopb));
                self.loop_stack.push((head.clone(), end.clone()));
                self.break_stack.push(end.clone());
                self.gen_stmt(body, ended, default_ret_i32);
                self.break_stack.pop(); self.loop_stack.pop();
                self.emit(format!("{}:", head));
                let cval = self.gen_expr(c);
                let cond = self.to_i1(cval);
                self.emit(format!("  br i1 {}, label %{}, label %{}", cond, loopb, end));
                self.emit(format!("{}:", end));
            }
            Stmt::For { init, c, post, body, .. } => {
                let head = self.new_label("for.cond");
                let loopb = self.new_label("for.body");
                let cont = self.new_label("for.cont");
                let end = self.new_label("for.end");
                if let Some(i) = init { self.gen_stmt(i, ended, default_ret_i32); }
                self.emit(format!("  br label %{}", head));
                self.emit(format!("{}:", head));
                if let Some(ce) = c {
                    let cval = self.gen_expr(ce);
                    let cond = self.to_i1(cval);
                    self.emit(format!("  br i1 {}, label %{}, label %{}", cond, loopb, end));
                } else { self.emit(format!("  br label %{}", loopb)); }
                self.emit(format!("{}:", loopb));
                self.loop_stack.push((cont.clone(), end.clone()));
                self.break_stack.push(end.clone());
                self.gen_stmt(body, ended, default_ret_i32);
                self.break_stack.pop(); self.loop_stack.pop();
                self.emit(format!("{}:", cont));
                if let Some(p) = post { let _ = self.gen_expr(p); }
                self.emit(format!("  br label %{}", head));
                self.emit(format!("{}:", end));
            }
            Stmt::Break(_) => {
                if let Some(b) = self.break_stack.last() { self.emit(format!("  br label %{}", b)); }
                // continue code must be placed in a fresh block to keep IR valid
                self.start_new_block("after.break");
            }
            Stmt::Continue(_) => {
                if let Some((c, _)) = self.loop_stack.last() { self.emit(format!("  br label %{}", c)); }
                self.start_new_block("after.continue");
            }
            Stmt::Switch { expr, body, .. } => {
                // Evaluate discriminator to i32 reg
                let v = self.gen_expr(expr);
                let v32 = self.to_i32(v);
                let (_ty, discr) = self.as_reg(v32);
                let end_label = self.new_label("switch.end");
                let default_label = self.new_label("default");
                // Build context and pre-scan body for case constants to allocate labels
                let mut ctx = SwitchCtx { discr_reg: discr.clone(), end_label: end_label.clone(), cases: HashMap::new(), default_label: default_label.clone(), default_emitted: false };
                self.scan_cases_alloc_labels(body.as_ref(), &mut ctx);
                // Emit linear dispatch chain (icmp/branch) for each case
                if ctx.cases.is_empty() {
                    // No cases: jump to default
                    self.emit(format!("  br label %{}", default_label));
                } else {
                    let mut next_cmp = self.new_label("switch.cmp");
                    self.emit(format!("  br label %{}", next_cmp));
                    self.emit(format!("{}:", next_cmp));
                    for (val, lab) in ctx.cases.iter() {
                        let r = self.new_reg();
                        self.emit(format!("  {} = icmp eq i32 {}, {}", r, discr, val));
                        let nxt = self.new_label("switch.cmp");
                        self.emit(format!("  br i1 {}, label %{}, label %{}", r, lab, nxt));
                        self.emit(format!("{}:", nxt));
                        let _ = next_cmp; // Prevent warning while preserving logic
                        next_cmp = nxt;
                    }
                    // Default at end
                    self.emit(format!("  br label %{}", default_label));
                }
                // Push break and switch context, then lower body which will place labels
                self.break_stack.push(end_label.clone());
                self.switch_stack.push(ctx);
                self.gen_stmt(body, ended, default_ret_i32);
                let ctx = if let Some(ctx) = self.switch_stack.pop() {
                    ctx
                } else {
                    // Synthétise un contexte minimal pour éviter tout panic et garder un IR valide
                    SwitchCtx {
                        discr_reg: discr.clone(),
                        end_label: end_label.clone(),
                        cases: HashMap::new(),
                        default_label: default_label.clone(),
                        default_emitted: false,
                    }
                };
                self.break_stack.pop();
                // Ensure default label exists even if not present in source
                if !ctx.default_emitted {
                    self.emit(format!("{}:", ctx.default_label));
                    self.emit(format!("  br label %{}", ctx.end_label));
                }
                // End label
                self.emit(format!("{}:", end_label));
            }
            Stmt::Case { expr, stmt, .. } => {
                if let Some(val) = Self::eval_const_i64(expr) {
                    let lab_opt = self.switch_stack.last().and_then(|ctx| ctx.cases.get(&val).cloned());
                    if let Some(lab) = lab_opt {
                        self.emit(format!("{}:", lab));
                    } else {
                        let lab = self.new_label("case");
                        self.emit(format!("{}:", lab));
                    }
                }
                self.gen_stmt(stmt, ended, default_ret_i32);
            }
            Stmt::Default { stmt, .. } => {
                let lab = self.switch_stack.last().map(|ctx| ctx.default_label.clone()).unwrap_or_else(|| self.new_label("default"));
                self.emit(format!("{}:", lab));
                if let Some(ctx) = self.switch_stack.last_mut() { ctx.default_emitted = true; }
                self.gen_stmt(stmt, ended, default_ret_i32);
            }
            Stmt::Label { name, stmt, .. } => {
                let l = self.ensure_named_label(name);
                // force terminate current block before label
                self.emit(format!("  br label %{}", l));
                self.emit(format!("{}:", l));
                self.gen_stmt(stmt, ended, default_ret_i32);
            }
            Stmt::Goto { name, .. } => {
                let l = self.ensure_named_label(name);
                self.emit(format!("  br label %{}", l));
                // continue in a fresh block to keep IR structurally valid
                self.start_new_block("after.goto");
            }
            _ => { /* unhandled parts will be ignored */ }
        }
    }

    fn gen_local_decl(&mut self, d: &Decl) {
        // Skip typedefs in local scope
        if matches!(d.storage, Some(StorageClass::Typedef)) { return; }
        // Track CType
        self.locals_cty.insert(d.name.clone(), d.ty.clone());
        // Handle local unions: allocate [max_field_size x i8] (already added) and local structs similarly
        if let CType::Struct { fields: Some(fs), .. } = &d.ty {
            let ti = self.target_info();
            let size = fs.iter().fold((0usize, 1usize), |(off, maxa), fld| {
                let a = ti.alignof(&fld.ty).unwrap_or(1);
                let s = ti.sizeof(&fld.ty).unwrap_or(0);
                (Self::bf_align_to(off, a) + s, maxa.max(a))
            });
            let total = Self::bf_align_to(size.0, size.1);
            let alloca = self.new_reg();
            self.emit(format!("  {} = alloca [{} x i8]", alloca, total));
            self.locals.insert(d.name.clone(), (alloca.clone(), format!("[{} x i8]", total)));
            // Zero-init for now
            self.emit(format!("  store [{} x i8] zeroinitializer, ptr {}", total, alloca));
            return;
        }
        // Handle local arrays specially: typed alloca + constant store
        if let CType::Array { of, size } = &d.ty {
            // Determine element LLVM type
            let elem_llty = self.ty(of);
            // Determine effective size
            let mut eff_size: Option<usize> = *size;
            let mut str_bytes: Option<Vec<u8>> = None;
            if eff_size.is_none() {
                if let Some(init) = &d.init {
                    match init {
                        Initializer::Expr(Expr::StrLit(s, _)) if matches!(**of, CType::Char{..}) => {
                            let bytes = Self::decode_c_string(s);
                            eff_size = Some(bytes.len() + 1);
                            str_bytes = Some(bytes);
                        }
                        Initializer::List(list) => {
                            eff_size = Some(list.len());
                        }
                        _ => {}
                    }
                }
            } else if let (Some(_n), Some(Initializer::Expr(Expr::StrLit(s, _)))) = (eff_size, &d.init) {
                if matches!(**of, CType::Char{..}) {
                    // keep bytes for init use
                    str_bytes = Some(Self::decode_c_string(s));
                }
            }
            if let Some(n) = eff_size {
                // Alloca [n x elem]
                let alloca = self.new_reg();
                self.emit(format!("  {} = alloca [{} x {}]", alloca, n, elem_llty));
                // Remember with array-typed marker to avoid scalar loads later
                self.locals.insert(d.name.clone(), (alloca.clone(), format!("[{} x {}]", n, elem_llty)));
                // Initialize
                if let Some(init) = &d.init {
                    match init {
                        Initializer::Expr(Expr::StrLit(_, _)) if matches!(**of, CType::Char{..}) => {
                            // Build bytes vector including NUL for implicit size; zero-pad
                            let mut bytes = str_bytes.take().unwrap_or_default();
                            if size.is_none() { bytes.push(0); }
                            if n > bytes.len() { bytes.resize(n, 0); }
                            let elems: Vec<String> = bytes.into_iter().map(|b| format!("i8 {}", b)).collect();
                            self.emit(format!("  store [{} x i8] [{}], ptr {}", n, elems.join(", "), alloca));
                        }
                        Initializer::List(list) => {
                            let mut elems: Vec<String> = Vec::with_capacity(n);
                            for i in 0..n {
                                if i < list.len() {
                                    match &list[i] {
                                        Initializer::Expr(Expr::IntLit(v, _)) => elems.push(format!("{} {}", elem_llty, v)),
                                        Initializer::Expr(Expr::CharLit(cv, _)) if elem_llty == "i8" => {
                                            let b = cv.bytes().next().unwrap_or(b'\0');
                                            elems.push(format!("i8 {}", b));
                                        }
                                        _ => elems.push(format!("{} 0", elem_llty)),
                                    }
                                } else {
                                    elems.push(format!("{} 0", elem_llty));
                                }
                            }
                            self.emit(format!("  store [{} x {}] [{}], ptr {}", n, elem_llty, elems.join(", "), alloca));
                        }
                        _ => {
                            // Unsupported init form: zero the array
                            self.emit(format!("  store [{} x {}] zeroinitializer, ptr {}", n, elem_llty, alloca));
                        }
                    }
                } else {
                    // No initializer: zero-init
                    self.emit(format!("  store [{} x {}] zeroinitializer, ptr {}", n, elem_llty, alloca));
                }
                return;
            }
            // If we reach here, no size could be determined: fallback to opaque alloca
        }
        let llty = self.ty(&d.ty);
        let alloca = self.new_reg();
        self.emit(format!("  {} = alloca {}", alloca, llty));
        self.locals.insert(d.name.clone(), (alloca.clone(), llty.clone()));
        
        // Emit debug info for local variable
        if self.debug {
            self.emit_variable_debug_info(&d.name, &alloca, &d.ty);
        }
        
        if let Some(Initializer::Expr(e)) = &d.init {
            // init only simple int
            if let Value::ConstInt { val } = self.gen_expr(e) {
                let tmp = self.new_reg();
                self.emit(format!("  {} = add i32 0, {}", tmp, val));
                self.emit(format!("  store i32 {}, ptr {}", tmp, alloca));
            }
        }
    }

    #[inline]
    fn bf_align_to(sz: usize, a: usize) -> usize { if a == 0 { sz } else { (sz + a - 1) / a * a } }

    fn find_bitfield_in_struct(&self, fs: &Vec<Decl>, field: &str) -> Option<BitfieldInfo> {
        let ti = self.target_info();
        // running state for current allocation unit of bitfields
        let mut offset_bytes: usize = 0;
        let mut in_bf = false;
        let mut unit_align: usize = 1;
        let mut unit_bits: usize = 0;
        let mut unit_start: usize = 0; // byte offset of current unit
        let mut bitpos: usize = 0;     // bit position within current unit
        for f in fs {
            if f.bit_width.is_none() {
                if in_bf {
                    let unit_bytes = (unit_bits + 7) / 8;
                    offset_bytes = Self::bf_align_to(unit_start + unit_bytes, unit_align);
                    in_bf = false;
                }
                let a = ti.alignof(&f.ty).ok()?;
                let s = ti.sizeof(&f.ty).ok()?;
                offset_bytes = Self::bf_align_to(offset_bytes, a) + s;
                continue;
            }
            let width = Self::eval_bit_width(f.bit_width.as_ref()?)?;
            let ty_bits = ti.int_bit_width(&f.ty)?;
            let a = ti.alignof(&f.ty).ok()?;
            if width == 0 {
                if in_bf {
                    let unit_bytes = (unit_bits + 7) / 8;
                    offset_bytes = Self::bf_align_to(unit_start + unit_bytes, a);
                } else {
                    offset_bytes = Self::bf_align_to(offset_bytes, a);
                }
                in_bf = false; bitpos = 0; unit_bits = 0; unit_align = a; unit_start = offset_bytes; continue;
            }
            if !in_bf {
                unit_align = a;
                unit_bits = ty_bits;
                unit_start = Self::bf_align_to(offset_bytes, unit_align);
                bitpos = 0;
                in_bf = true;
            }
            if width > unit_bits.saturating_sub(bitpos) {
                let unit_bytes = (unit_bits + 7) / 8;
                offset_bytes = Self::bf_align_to(unit_start + unit_bytes, unit_align);
                unit_start = offset_bytes;
                bitpos = 0;
                unit_bits = ty_bits;
            }
            if f.name == field {
                let llty = match ty_bits { 8 => "i8".into(), 16 => "i16".into(), _ => "i32".into() };
                let signed = match &f.ty { CType::Int { signed, .. } | CType::Char { signed } => signed.unwrap_or(true), CType::Enum { .. } => true, _ => true };
                return Some(BitfieldInfo { byte_off: unit_start, bit_off: bitpos, width, llty, bits: ty_bits, signed });
            }
            bitpos += width;
            if bitpos == unit_bits {
                let unit_bytes = (unit_bits + 7) / 8;
                offset_bytes = Self::bf_align_to(unit_start + unit_bytes, unit_align);
                in_bf = false; bitpos = 0;
            }
        }
        None
    }

    // --- helpers for expressions/values ---
    fn as_reg(&mut self, v: Value) -> (String, String) {
        match v {
            Value::Reg { ty, name } => (ty, name),
            Value::ConstInt { val } => { let r = self.new_reg(); self.emit(format!("  {} = add i32 0, {}", r, val)); ("i32".into(), r) }
            Value::ConstGlobal(g) => ("ptr".into(), g),
        }
    }

    fn to_i32(&mut self, v: Value) -> Value {
        match v {
            Value::Reg { ty, name } if ty == "i32" => Value::Reg { ty, name },
            Value::Reg { ty, name } if ty == "i1" => { let r = self.new_reg(); self.emit(format!("  {} = zext i1 {} to i32", r, name)); Value::Reg { ty: "i32".into(), name: r } },
            Value::Reg { ty, name } if ty == "i8" => { let r = self.new_reg(); self.emit(format!("  {} = zext i8 {} to i32", r, name)); Value::Reg { ty: "i32".into(), name: r } },
            Value::Reg { ty, name } if ty == "i16" => { let r = self.new_reg(); self.emit(format!("  {} = zext i16 {} to i32", r, name)); Value::Reg { ty: "i32".into(), name: r } },
            Value::Reg { ty, name } if ty == "ptr" => { let r = self.new_reg(); self.emit(format!("  {} = ptrtoint ptr {} to i32", r, name)); Value::Reg { ty: "i32".into(), name: r } },
            Value::Reg { ty: _, name } => { let r = self.new_reg(); self.emit(format!("  {} = add i32 0, {}", r, name)); Value::Reg { ty: "i32".into(), name: r } },
            Value::ConstInt { val } => Value::ConstInt { val },
            Value::ConstGlobal(g) => { let r = self.new_reg(); self.emit(format!("  {} = ptrtoint ptr {} to i32", r, g)); Value::Reg { ty: "i32".into(), name: r } },
        }
    }

    fn to_i1(&mut self, v: Value) -> String {
        match v {
            Value::ConstInt { val } => { let r = self.new_reg(); self.emit(format!("  {} = icmp ne i32 {}, 0", r, val)); r }
            Value::ConstGlobal(g) => { let r = self.new_reg(); self.emit(format!("  {} = icmp ne ptr {}, null", r, g)); r }
            Value::Reg { ty, name } => {
                if ty == "ptr" { let r = self.new_reg(); self.emit(format!("  {} = icmp ne ptr {}, null", r, name)); r }
                else if ty == "i1" { name }
                else {
                    let vv = if ty == "i32" { name } else { let v32 = self.new_reg(); self.emit(format!("  {} = zext {} {} to i32", v32, ty, name)); v32 };
                    let r = self.new_reg(); self.emit(format!("  {} = icmp ne i32 {}, 0", r, vv)); r
                }
            }
        }
    }

    fn scan_cases_alloc_labels(&mut self, s: &Stmt, ctx: &mut SwitchCtx) {
        match s {
            Stmt::Compound { items, .. } => {
                for it in items { if let BlockItem::Stmt(ss) = it { self.scan_cases_alloc_labels(ss, ctx); } }
            }
            Stmt::Case { expr, .. } => {
                if let Some(v) = Self::eval_const_i64(expr) { ctx.cases.entry(v).or_insert_with(|| self.new_label("case")); }
            }
            Stmt::Default { .. } => { /* label already reserved */ }
            Stmt::Switch { body, .. } => { self.scan_cases_alloc_labels(body, ctx); }
            _ => {}
        }
    }

    fn eval_const_i64(e: &Expr) -> Option<i64> {
        match e { Expr::IntLit(s, _) => s.parse::<i64>().ok(), _ => None }
    }

    fn eval_bit_width(e: &Expr) -> Option<usize> { Self::eval_const_i64(e).map(|v| if v < 0 { 0 } else { v as usize }) }

    fn gen_expr(&mut self, e: &Expr) -> Value {
        match e {
            Expr::IntLit(v, _) => Value::ConstInt { val: v.clone() },
            Expr::FloatLit(v, _) => Value::ConstInt { val: format!("bitcast (double {} to i64)", v) },
            Expr::CharLit(v, _) => {
                let byte = v.bytes().next().unwrap_or(0);
                Value::ConstInt { val: byte.to_string() }
            }
            Expr::StrLit(s, _) => { let g = self.ensure_str_global(s); Value::ConstGlobal(g) }
            Expr::Ident(name, _) => {
                if let Some((alloca, llty)) = self.locals.get(name).cloned() {
                    let r = self.new_reg();
                    self.emit(format!("  {} = load {}, ptr {}", r, llty, alloca));
                    Value::Reg { ty: llty, name: r }
                } else {
                    // Reference to a global/function symbol
                    Value::ConstGlobal(format!("@{}", name))
                }
            }
            Expr::Index { base, index, .. } => {
                // Array indexing: base[index]
                let base_val = self.gen_expr(base);
                let index_val = self.gen_expr(index);
                
                let base_ptr = match base_val {
                    Value::Reg { ty, name } if ty == "ptr" => name,
                    Value::ConstGlobal(g) => g,
                    other => {
                        let (_, r) = self.as_reg(other);
                        let p = self.new_reg();
                        self.emit(format!("  {} = inttoptr i32 {} to ptr", p, r));
                        p
                    }
                };
                
                let (_, idx_reg) = self.as_reg(index_val);
                let result_ptr = self.new_reg();
                self.emit(format!("  {} = getelementptr i8, ptr {}, i32 {}", result_ptr, base_ptr, idx_reg));
                
                let r = self.new_reg();
                self.emit(format!("  {} = load i8, ptr {}", r, result_ptr));
                Value::Reg { ty: "i8".into(), name: r }
            }
            Expr::Member { base, field, .. } => {
                // Struct member access
                let _base_val = self.gen_expr(base);
                // For now, return a placeholder
                let r = self.new_reg();
                self.emit(format!("  {} = add i32 0, 0  ; member access {}", r, field));
                Value::Reg { ty: "i32".into(), name: r }
            }
            Expr::PtrMember { base, field, .. } => {
                // Pointer to struct member access
                let _base_val = self.gen_expr(base);
                // For now, return a placeholder
                let r = self.new_reg();
                self.emit(format!("  {} = add i32 0, 0  ; ptr member access {}", r, field));
                Value::Reg { ty: "i32".into(), name: r }
            }
            Expr::Call { callee, args, .. } => {
                if let Expr::Ident(name, _) = &**callee {
                    // Builtin varargs lowering
                    if name == "__builtin_va_start" {
                        if !args.is_empty() {
                            // Pass pointer to va_list object (alloca), not loaded value
                            if let Expr::Ident(vname, _) = &args[0] {
                                if let Some((alloca, _llty)) = self.locals.get(vname).cloned() {
                                    self.ensure_va_intrinsics();
                                    self.emit(format!("  call void @llvm.va_start(ptr {})", alloca));
                                }
                            } else {
                                let ap = self.gen_expr(&args[0]);
                                let (_ty, ap_reg) = self.as_reg(ap);
                                self.ensure_va_intrinsics();
                                self.emit(format!("  call void @llvm.va_start(ptr {})", ap_reg));
                            }
                        }
                        return Value::ConstInt { val: "0".into() };
                    } else if name == "__builtin_va_end" {
                        if !args.is_empty() {
                            if let Expr::Ident(vname, _) = &args[0] {
                                if let Some((alloca, _llty)) = self.locals.get(vname).cloned() {
                                    self.ensure_va_intrinsics();
                                    self.emit(format!("  call void @llvm.va_end(ptr {})", alloca));
                                }
                            } else {
                                let ap = self.gen_expr(&args[0]);
                                let (_ty, ap_reg) = self.as_reg(ap);
                                self.ensure_va_intrinsics();
                                self.emit(format!("  call void @llvm.va_end(ptr {})", ap_reg));
                            }
                        }
                        return Value::ConstInt { val: "0".into() };
                    } else if name == "__builtin_va_arg_i32" {
                        if !args.is_empty() {
                            if let Expr::Ident(vname, _) = &args[0] {
                                if let Some((alloca, _llty)) = self.locals.get(vname).cloned() {
                                    let r = self.new_reg();
                                    self.emit(format!("  {} = va_arg ptr {}, i32", r, alloca));
                                    return Value::Reg { ty: "i32".into(), name: r };
                                }
                            }
                            let ap = self.gen_expr(&args[0]);
                            let (_ty, ap_reg) = self.as_reg(ap);
                            let r = self.new_reg();
                            self.emit(format!("  {} = va_arg ptr {}, i32", r, ap_reg));
                            return Value::Reg { ty: "i32".into(), name: r };
                        }
                        return Value::ConstInt { val: "0".into() };
                    }
                    self.ensure_decl_for_call(name);
                    // Determine return type and parameter types
                    let mut ret_is_aggr = false;
                    let mut ret_ll: String = "i32".into();
                    let mut params: Vec<CType> = Vec::new();
                    let mut is_varargs = true; // default: no prototype implies varargs and default promotions apply to all args
                    if let Some(CType::Function { ret, params: ps, varargs, prototype: _ }) = self.fn_type_of_name(name).cloned() {
                        ret_is_aggr = Self::is_aggregate(&ret);
                        ret_ll = if ret_is_aggr { "void".into() } else { self.ty(&ret) };
                        params = ps.iter().map(|p| p.ty.clone()).collect();
                        is_varargs = varargs;
                    }
                    let fixed_n = std::cmp::min(args.len(), params.len());
                    let mut a: Vec<String> = Vec::new();
                    // If sret, allocate result buffer and pass as first argument
                    let mut sret_result_ptr: Option<String> = None;
                    if ret_is_aggr {
                        if let Some(CType::Function { ret, .. }) = self.fn_type_of_name(name) {
                            let agg = self.llvm_byval_pointee(ret);
                            let dst = self.new_reg();
                            self.emit(format!("  {} = alloca {}", dst, agg));
                            a.push(format!("ptr {}", dst));
                            sret_result_ptr = Some(dst);
                        }
                    }
                    for i in 0..fixed_n {
                        let v = self.gen_expr(&args[i]);
                        // Convert according to parameter type
                        let pty = &params[i];
                        let val_str = match v {
                            Value::Reg { ty, name } => {
                                let dst = self.ty(pty);
                                if ty == dst { format!("{} {}", ty, name) }
                                else if ty == "ptr" && dst == "ptr" { format!("ptr {}", name) }
                                else if ty.starts_with('i') && dst.starts_with('i') {
                                    let rv = self.new_reg();
                                    let srcw: usize = ty[1..].parse().unwrap_or(32);
                                    let dstw: usize = dst[1..].parse().unwrap_or(32);
                                    if srcw < dstw { self.emit(format!("  {} = sext {} {} to {}", rv, ty, name, dst)); format!("{} {}", dst, rv) }
                                    else if srcw > dstw { self.emit(format!("  {} = trunc {} {} to {}", rv, ty, name, dst)); format!("{} {}", dst, rv) }
                                    else { format!("{} {}", ty, name) }
                                } else if ty == "float" && dst == "double" {
                                    let rv = self.new_reg(); self.emit(format!("  {} = fpext float {} to double", rv, name)); format!("double {}", rv)
                                } else { format!("{} {}", ty, name) }
                            }
                            Value::ConstInt { val } => format!("i32 {}", val),
                            Value::ConstGlobal(g) => format!("ptr {}", g),
                        };
                        a.push(val_str);
                    }
                    // Extra args: default argument promotions
                    if args.len() > fixed_n {
                        for i in fixed_n..args.len() {
                            let raw = self.gen_expr(&args[i]);
                            let hint = self.ctype_hint(&args[i]);
                            let prom = self.promote_default_for_vararg(raw, hint.as_ref());
                            match prom {
                                Value::Reg { ty, name } => a.push(format!("{} {}", ty, name)),
                                Value::ConstInt { val } => a.push(format!("i32 {}", val)),
                                Value::ConstGlobal(g) => a.push(format!("ptr {}", g)),
                            }
                        }
                    }
                    // No prototype: apply promotions to all arguments
                    if params.is_empty() && is_varargs {
                        let mut a2: Vec<String> = Vec::with_capacity(a.len());
                        a2.clear();
                        for (_i, arg) in args.iter().enumerate() {
                            let raw = self.gen_expr(arg);
                            let hint = self.ctype_hint(arg);
                            let prom = self.promote_default_for_vararg(raw, hint.as_ref());
                            match prom {
                                Value::Reg { ty, name } => a2.push(format!("{} {}", ty, name)),
                                Value::ConstInt { val } => a2.push(format!("i32 {}", val)),
                                Value::ConstGlobal(g) => a2.push(format!("ptr {}", g)),
                            }
                        }
                        // keep any sret pointer already placed at the beginning
                        if let Some(dst) = sret_result_ptr.clone() { a2.insert(0, format!("ptr {}", dst)); }
                        a = a2;
                    }
                    if ret_is_aggr {
                        self.emit(format!("  call void @{}({})", name, a.join(", ")));
                        // return the sret buffer pointer as the value of the call expression
                        let dst = sret_result_ptr.unwrap();
                        Value::Reg { ty: "ptr".into(), name: dst }
                    } else if ret_ll == "void" {
                        self.emit(format!("  call {} @{}({})", ret_ll, name, a.join(", ")));
                        Value::ConstInt { val: "0".into() }
                    } else {
                        let r = self.new_reg();
                        self.emit(format!("  {} = call {} @{}({})", r, ret_ll, name, a.join(", ")));
                        Value::Reg { ty: ret_ll, name: r }
                    }
                } else {
                    Value::ConstInt { val: "0".into() }
                }
            }
            Expr::Unary { op, expr, .. } => {
                match op {
                    UnaryOp::LogNot => {
                        let expr_val = self.gen_expr(expr);
                        let (_, r) = self.as_reg(expr_val);
                        let res = self.new_reg();
                        self.emit(format!("  {} = icmp eq i32 {}, 0", res, r));
                        let final_res = self.new_reg();
                        self.emit(format!("  {} = zext i1 {} to i32", final_res, res));
                        Value::Reg { ty: "i32".into(), name: final_res }
                    }
                    UnaryOp::BitNot => {
                        let expr_val = self.gen_expr(expr);
                        let (_, r) = self.as_reg(expr_val);
                        let res = self.new_reg();
                        self.emit(format!("  {} = xor i32 {}, -1", res, r));
                        Value::Reg { ty: "i32".into(), name: res }
                    }
                    UnaryOp::Minus => {
                        let expr_val = self.gen_expr(expr);
                        let (_, r) = self.as_reg(expr_val);
                        let res = self.new_reg();
                        self.emit(format!("  {} = sub i32 0, {}", res, r));
                        Value::Reg { ty: "i32".into(), name: res }
                    }
                    UnaryOp::Plus => {
                        // Unary plus is a no-op
                        self.gen_expr(expr)
                    }
                    UnaryOp::AddrOf => {
                        // Address-of operator - for now return placeholder
                        let r = self.new_reg();
                        self.emit(format!("  {} = inttoptr i32 0 to ptr  ; address-of", r));
                        Value::Reg { ty: "ptr".into(), name: r }
                    }
                    UnaryOp::Deref => {
                        // Dereference operator
                        let ptr_val = self.gen_expr(expr);
                        let ptr_reg = match ptr_val {
                            Value::Reg { name, .. } => name,
                            _ => {
                                let (_, r) = self.as_reg(ptr_val);
                                let p = self.new_reg();
                                self.emit(format!("  {} = inttoptr i32 {} to ptr", p, r));
                                p
                            }
                        };
                        let r = self.new_reg();
                        self.emit(format!("  {} = load i32, ptr {}", r, ptr_reg));
                        Value::Reg { ty: "i32".into(), name: r }
                    }
                }
            }
            Expr::Binary { op, lhs, rhs, .. } => {
                // Binary operations
                let lhs_val = self.gen_expr(lhs);
                let rhs_val = self.gen_expr(rhs);
                let (_, l_reg) = self.as_reg(lhs_val);
                let (_, r_reg) = self.as_reg(rhs_val);
                let res = self.new_reg();
                
                match op {
                    BinaryOp::Add => self.emit(format!("  {} = add i32 {}, {}", res, l_reg, r_reg)),
                    BinaryOp::Sub => self.emit(format!("  {} = sub i32 {}, {}", res, l_reg, r_reg)),
                    BinaryOp::Mul => self.emit(format!("  {} = mul i32 {}, {}", res, l_reg, r_reg)),
                    BinaryOp::Div => self.emit(format!("  {} = sdiv i32 {}, {}", res, l_reg, r_reg)),
                    BinaryOp::Mod => self.emit(format!("  {} = srem i32 {}, {}", res, l_reg, r_reg)),
                    BinaryOp::Shl => self.emit(format!("  {} = shl i32 {}, {}", res, l_reg, r_reg)),
                    BinaryOp::Shr => self.emit(format!("  {} = ashr i32 {}, {}", res, l_reg, r_reg)),
                    BinaryOp::BitAnd => self.emit(format!("  {} = and i32 {}, {}", res, l_reg, r_reg)),
                    BinaryOp::BitOr => self.emit(format!("  {} = or i32 {}, {}", res, l_reg, r_reg)),
                    BinaryOp::BitXor => self.emit(format!("  {} = xor i32 {}, {}", res, l_reg, r_reg)),
                    BinaryOp::Lt => {
                        self.emit(format!("  {} = icmp slt i32 {}, {}", res, l_reg, r_reg));
                        let final_res = self.new_reg();
                        self.emit(format!("  {} = zext i1 {} to i32", final_res, res));
                        return Value::Reg { ty: "i32".into(), name: final_res };
                    }
                    BinaryOp::Gt => {
                        self.emit(format!("  {} = icmp sgt i32 {}, {}", res, l_reg, r_reg));
                        let final_res = self.new_reg();
                        self.emit(format!("  {} = zext i1 {} to i32", final_res, res));
                        return Value::Reg { ty: "i32".into(), name: final_res };
                    }
                    BinaryOp::Le => {
                        self.emit(format!("  {} = icmp sle i32 {}, {}", res, l_reg, r_reg));
                        let final_res = self.new_reg();
                        self.emit(format!("  {} = zext i1 {} to i32", final_res, res));
                        return Value::Reg { ty: "i32".into(), name: final_res };
                    }
                    BinaryOp::Ge => {
                        self.emit(format!("  {} = icmp sge i32 {}, {}", res, l_reg, r_reg));
                        let final_res = self.new_reg();
                        self.emit(format!("  {} = zext i1 {} to i32", final_res, res));
                        return Value::Reg { ty: "i32".into(), name: final_res };
                    }
                    BinaryOp::Eq => {
                        self.emit(format!("  {} = icmp eq i32 {}, {}", res, l_reg, r_reg));
                        let final_res = self.new_reg();
                        self.emit(format!("  {} = zext i1 {} to i32", final_res, res));
                        return Value::Reg { ty: "i32".into(), name: final_res };
                    }
                    BinaryOp::Ne => {
                        self.emit(format!("  {} = icmp ne i32 {}, {}", res, l_reg, r_reg));
                        let final_res = self.new_reg();
                        self.emit(format!("  {} = zext i1 {} to i32", final_res, res));
                        return Value::Reg { ty: "i32".into(), name: final_res };
                    }
                    BinaryOp::LogAnd => {
                        // Short-circuit evaluation: for now, just use regular and
                        self.emit(format!("  {} = and i32 {}, {}", res, l_reg, r_reg));
                    }
                    BinaryOp::LogOr => {
                        // Short-circuit evaluation: for now, just use regular or
                        self.emit(format!("  {} = or i32 {}, {}", res, l_reg, r_reg));
                    }
                    BinaryOp::Comma => {
                        // Comma operator: evaluate both, return the right
                        return self.gen_expr(rhs);
                    }
                }
                
                Value::Reg { ty: "i32".into(), name: res }
            }
            Expr::PreInc { expr: _, .. } | Expr::PostInc { expr: _, .. } => {
                // Pre/post increment - for now just return the expression value
                let r = self.new_reg();
                self.emit(format!("  {} = add i32 0, 1  ; increment", r));
                Value::Reg { ty: "i32".into(), name: r }
            }
            Expr::PreDec { expr: _, .. } | Expr::PostDec { expr: _, .. } => {
                // Pre/post decrement - for now just return the expression value  
                let r = self.new_reg();
                self.emit(format!("  {} = sub i32 0, 1  ; decrement", r));
                Value::Reg { ty: "i32".into(), name: r }
            }
            Expr::Cast { expr, .. } => {
                // Type cast - for now just generate the expression
                self.gen_expr(expr)
            }
            Expr::SizeOfExpr { .. } => {
                // sizeof expression - return a constant size
                Value::ConstInt { val: "4".to_string() }  // Default to 4 bytes
            }
            Expr::SizeOfType { .. } => {
                // sizeof type - return a constant size  
                Value::ConstInt { val: "4".to_string() }  // Default to 4 bytes
            }
            Expr::Assign { rhs, .. } => {
                // Assignment - generate rhs and return it
                self.gen_expr(rhs)
            }
            Expr::Cond { c, t, e, .. } => {
                // Conditional operator a ? b : c
                let cond_val = self.gen_expr(c);
                let (_, _cond_reg) = self.as_reg(cond_val);
                let then_val = self.gen_expr(t);
                let _else_val = self.gen_expr(e);
                
                // For now, just return then_val
                then_val
            }
        }
    }

    fn ctype_hint(&self, e: &Expr) -> Option<CType> {
        match e {
            Expr::Ident(n, _) => self.locals_cty.get(n).cloned().or_else(|| self.globals_cty.get(n).cloned()),
            Expr::IntLit(_, _) => Some(CType::Int { signed: Some(true), long: false, short: false }),
            Expr::StrLit(_, _) => Some(CType::Pointer { of: Box::new(CType::Char { signed: None }), qualifiers: Vec::new() }),
            Expr::FloatLit(_, _) => Some(CType::Double { long: false }), // literal without suffix is double; used as hint only
            _ => None,
        }
    }

    fn promote_default_for_vararg(&mut self, v: Value, cty: Option<&CType>) -> Value {
        match v {
            Value::Reg { ty, name } => {
                match ty.as_str() {
                    "float" => {
                        let r = self.new_reg();
                        self.emit(format!("  {} = fpext float {} to double", r, name));
                        Value::Reg { ty: "double".into(), name: r }
                    }
                    "i8" | "i16" | "i1" => {
                        // decide signedness for extension
                        let signed = match cty {
                            Some(CType::Char { signed }) => signed.unwrap_or(true),
                            Some(CType::Int { signed, .. }) => signed.unwrap_or(true),
                            Some(CType::Enum { .. }) => true,
                            _ => true,
                        };
                        let r = self.new_reg();
                        if signed { self.emit(format!("  {} = sext {} {} to i32", r, ty, name)); }
                        else { self.emit(format!("  {} = zext {} {} to i32", r, ty, name)); }
                        Value::Reg { ty: "i32".into(), name: r }
                    }
                    _ => Value::Reg { ty, name },
                }
            }
            other @ Value::ConstInt { .. } => other, // already i32
            other @ Value::ConstGlobal(_) => other,  // pointers unchanged
        }
    }

    fn ensure_va_intrinsics(&mut self) {
        if !self.decls.contains_key("llvm.va_start") {
            self.decls.insert("llvm.va_start".into(), "declare void @llvm.va_start(ptr)".into());
        }
        if !self.decls.contains_key("llvm.va_end") {
            self.decls.insert("llvm.va_end".into(), "declare void @llvm.va_end(ptr)".into());
        }
    }
}

#[derive(Clone, Debug)]
struct BitfieldInfo {
    byte_off: usize,
    bit_off: usize,
    width: usize,
    llty: String,    // container LLVM int type (i8/i16/i32)
    bits: usize,     // container bit width
    signed: bool,
}