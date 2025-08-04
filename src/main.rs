mod lexer;

use std::env;
use colored::Colorize;

fn main() {
    let args: Vec<String> = env::args().collect();
    println!("Arguments: {:?}", args);
    if args.len() < 2
    {
        eprintln!("{}: {} no input files", "cc1".bold(), "fatal error:".red().bold());
        eprintln!("compilation terminated.");
        std::process::exit(1);
    }
    let filename = &args[1];
    println!("Compiling file: {}", filename);

    let source = match std::fs::read_to_string(filename) {
        Ok(content) => content,
        Err(e) => {
            eprintln!("{}: {} {}", "cc1".bold(), "error:".red().bold(), e);
            std::process::exit(1);
        }
    };

    match lexer::tokenize(&source) {
        Ok(tokens) => {
            println!("Tokens trouvés ({} au total):", tokens.len());
            for (i, token) in tokens.iter().enumerate() {
                println!("  {}: {:?}", i+1, token);
            }
        }
        Err(e) => {
            eprintln!("{}: {} {}", "cc1".bold(), "error:".red().bold(), e);
            std::process::exit(1);
        }
    }
}
