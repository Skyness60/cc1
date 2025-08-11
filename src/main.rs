// ----------------------------
// File: src/main.rs
// ----------------------------
mod front;
use std::env;
use std::fs;
use std::io::{self, Write};

fn usage() { eprintln!("usage: cc1 <infile> [-o <outfile|->]"); }

fn main() {
    let mut args: Vec<String> = env::args().collect();
    args.remove(0);
    if args.is_empty() { usage(); std::process::exit(1); }

    let mut infile: Option<String> = None;
    let mut outfile: Option<String> = None;

    let mut i = 0;
    while i < args.len() {
        match args[i].as_str() {
            "-o" => { if i + 1 >= args.len() { eprintln!("cc1: error: -o requires a value"); std::process::exit(1);} outfile = Some(args[i+1].clone()); i += 2; }
            s if s.starts_with('-') => { eprintln!("cc1: error: unknown option: {}", s); std::process::exit(1);} 
            s => { if infile.is_some() { eprintln!("cc1: error: multiple input files not supported"); std::process::exit(1);} infile = Some(s.to_string()); i += 1; }
        }
    }

    let infile = infile.expect("input required");
    let source = match fs::read_to_string(&infile) { Ok(s) => s, Err(e) => { eprintln!("cc1: error: could not read {}: {}", infile, e); std::process::exit(1);} };

    let mut lx = front::lexer::Lexer::new(&source, &infile);

    let mut out: Box<dyn Write> = match outfile.as_deref() {
        Some("-") => Box::new(io::stdout()),
        Some(path) => match fs::File::create(path) { Ok(f) => Box::new(f), Err(e) => { eprintln!("cc1: error: could not create {}: {}", path, e); std::process::exit(1)}},
        #[allow(non_snake_case)]
        None => Box::new(io::stdout()),
    };

    while let Some(item) = lx.next() {
        match item {
            Ok(tok) => {
                use front::token::TokenKind as K;
                if matches!(tok.kind, K::Eof) { break; }
                writeln!(out, "{:?}", tok).ok();
            }
            Err(err) => {
                eprintln!("{}:{}:{}: error: {}", err.file, err.span.line, err.span.col, err.msg);
                std::process::exit(1);
            }
        }
    }
}