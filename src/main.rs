// ----------------------------
// File: src/main.rs
// ----------------------------
mod front;
mod back;
mod preproc;
use std::env;
use std::fs;
use std::io::{self, Write, Read};

fn usage() { 
    eprintln!("usage: cc1 infile [-o outfile]");
    eprintln!();
    eprintln!("Options (development):");
    eprintln!("  -m32|-m64           Target architecture (default: -m32)");
    eprintln!("  -g                  Enable debug info");
    eprintln!("  --lex-only          Lexical analysis only");
    eprintln!("  --parse-expr        Parse expressions only");
    eprintln!("  --parse-tu          Parse translation unit only"); 
    eprintln!("  --sem               Semantic analysis only");
    eprintln!("  --preprocess-only   Preprocessing only (internal preprocessor)");
}

fn main() {
    let mut args: Vec<String> = env::args().collect();
    args.remove(0);
    if args.is_empty() { usage(); std::process::exit(1); }

    let mut infile: Option<String> = None;
    let mut outfile: Option<String> = None;
    let mut lex_only_mode = false;
    let mut parse_expr_mode = false;
    let mut parse_tu_mode = false;
    let mut sem_mode = false;
    let mut preprocess_only_mode = false;
    let mut arch_opt: Option<front::semantics::Arch> = None;
    let mut debug_info = false;

    let mut i = 0;
    while i < args.len() {
        match args[i].as_str() {
            "--lex-only" => { lex_only_mode = true; i += 1; }
            "--parse-expr" => { parse_expr_mode = true; i += 1; }
            "--parse-tu" => { parse_tu_mode = true; i += 1; }
            "--sem" => { sem_mode = true; i += 1; }
            "--preprocess-only" => { preprocess_only_mode = true; i += 1; }
            "-m32" => { arch_opt = Some(front::semantics::Arch::I386); i += 1; }
            "-m64" => { arch_opt = Some(front::semantics::Arch::X86_64); i += 1; }
            "-g" => { debug_info = true; i += 1; }
            "-o" => { if i + 1 >= args.len() { eprintln!("cc1: error: -o requires a value"); std::process::exit(1);} outfile = Some(args[i+1].clone()); i += 2; }
            s if s.starts_with("-o") => { outfile = Some(s[2..].to_string()); i += 1; }
            s if s.starts_with('-') && s != "-" => { eprintln!("cc1: error: unknown option: {}", s); std::process::exit(1);} 
            s => { if infile.is_some() { eprintln!("cc1: error: multiple input files not supported"); std::process::exit(1);} infile = Some(s.to_string()); i += 1; }
        }
    }

    let infile = match infile {
        Some(s) => s,
        None => { usage(); std::process::exit(1); }
    };
    let source = if infile == "-" {
        let mut s = String::new();
        if let Err(e) = io::stdin().read_to_string(&mut s) { eprintln!("cc1: error: could not read <stdin>: {}", e); std::process::exit(1);} s
    } else {
        match fs::read_to_string(&infile) { Ok(s) => s, Err(e) => { eprintln!("cc1: error: could not read {}: {}", infile, e); std::process::exit(1);} }
    };

    let mut out: Box<dyn Write> = match outfile.as_deref() {
        Some("-") => Box::new(io::stdout()),
        Some(path) => match fs::File::create(path) { Ok(f) => Box::new(f), Err(e) => { eprintln!("cc1: error: could not create {}: {}", path, e); std::process::exit(1)}},
        #[allow(non_snake_case)]
        None => Box::new(io::stdout()),
    };

    // Handle preprocess-only mode early
    if preprocess_only_mode {
        match preproc::basic_preprocess(&source) {
            Ok(s) => {
                writeln!(out, "{}", s).ok();
                return;
            }
            Err(err) => {
                eprintln!("{}:1:1: error: {}", infile, err);
                std::process::exit(1);
            }
        }
    }

    // Basic internal preprocessor: trigraphs, line splices, comment removal (phases 1–3/4)
    let psource = match preproc::basic_preprocess(&source) {
        Ok(s) => s,
        Err(err) => {
            eprintln!("{}:1:1: error: {}", infile, err);
            std::process::exit(1);
        }
    };

    if lex_only_mode {
        let mut lexer = front::lexer::Lexer::new(&psource, &infile);
        loop {
            match lexer.next() {
                Some(Ok(token)) => {
                    if token.kind == front::token::TokenKind::Eof { break; }
                    writeln!(out, "{:?}", token).ok();
                }
                Some(Err(err)) => { 
                    eprintln!("{}:{}:{}: error: {}", err.file, err.span.line, err.span.col, err.msg); 
                    std::process::exit(1); 
                }
                None => break,
            }
        }
        return;
    }

    if parse_tu_mode {
        let mut p = front::parser::Parser::new(&psource, &infile);
        match p.parse_translation_unit() {
            Ok(tu) => { writeln!(out, "{:#?}", tu).ok(); }
            Err(err) => { eprintln!("{}:{}:{}: error: {}", infile, err.span.line, err.span.col, err.msg); std::process::exit(1); }
        }
        return;
    }

    if parse_expr_mode {
        let mut p = front::parser::Parser::new(&psource, &infile);
        if infile == "-" {
            match p.parse_expr() {
                Ok(ast) => { writeln!(out, "{:#?}", ast).ok(); }
                Err(err) => { eprintln!("{}:{}:{}: error: {}", infile, err.span.line, err.span.col, err.msg); std::process::exit(1); }
            }
        } else {
            match p.parse_exprs_semi() {
                Ok(list) => { for e in list { writeln!(out, "{:#?}", e).ok(); } }
                Err(err) => { eprintln!("{}:{}:{}: error: {}", infile, err.span.line, err.span.col, err.msg); std::process::exit(1); }
            }
        }
        return;
    }

    if sem_mode {
        let mut p = front::parser::Parser::new(&psource, &infile);
        match p.parse_translation_unit() {
            Ok(tu) => {
                let diags = front::semantics::analyze_translation_unit(&tu);
                if diags.is_empty() {
                    writeln!(out, "OK").ok();
                } else {
                    for d in diags.errors {
                        eprintln!("{}:{}:{}: error: {}", infile, d.span.line, d.span.col, d.msg);
                    }
                    std::process::exit(1);
                }
            }
            Err(err) => { eprintln!("{}:{}:{}: error: {}", infile, err.span.line, err.span.col, err.msg); std::process::exit(1); }
        }
        return;
    }

    // Default: parse TU and emit LLVM IR
    let mut p = front::parser::Parser::new(&psource, &infile);
    match p.parse_translation_unit() {
        Ok(tu) => {
            let arch = arch_opt.unwrap_or(front::semantics::Arch::I386);
            let m = back::lower_to_llvm_ir(&tu, arch, debug_info, &infile);
            let _ = debug_info; // reserved for future DI metadata emission
            writeln!(out, "{}", m.text).ok();
        }
        Err(err) => { eprintln!("{}:{}:{}: error: {}", infile, err.span.line, err.span.col, err.msg); std::process::exit(1); }
    }
}