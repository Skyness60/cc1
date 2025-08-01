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
}
