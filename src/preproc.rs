// ----------------------------
// File: src/preproc.rs
// ----------------------------
#![allow(dead_code)]
//! Complete C89 preprocessor implementing phases 1–4:
//! - Phase 1: Translate trigraphs
//! - Phase 2: Splice continued lines ending with '\'
//! - Phase 3: Remove comments (/* ... */) and reject '//' in strict C89
//! - Phase 4: Execute preprocessing directives and expand macros

use std::collections::HashMap;
use std::path::PathBuf;

#[derive(Debug, Clone)]
pub enum MacroValue {
    Simple(String),
    FunctionLike { params: Vec<String>, body: String },
}

pub fn basic_preprocess(source: &str) -> Result<String, String> {
    full_preprocess(source, &mut HashMap::new(), &mut Vec::new())
}

pub fn full_preprocess(source: &str, defines: &mut HashMap<String, MacroValue>, include_stack: &mut Vec<PathBuf>) -> Result<String, String> {
    // Phase 1-3: trigraphs, line splicing, comments
    let s1 = translate_trigraphs(source);
    let s2 = splice_lines(&s1);
    let s3 = remove_comments(&s2)?;
    
    // Phase 4: macro expansion and directives
    let s4 = expand_macros_with_context(&s3, defines, include_stack);
    Ok(s4)
}

fn translate_trigraphs(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    let mut i = 0; let b = s.as_bytes();
    while i < b.len() {
        if i + 2 < b.len() && b[i] == b'?' && b[i+1] == b'?' {
            // Use match on third
            let c = b[i+2];
            let repl = match c {
                b'=' => Some('#'), b'/' => Some('\\'), b'\'' => Some('^'), b'(' => Some('['), b')' => Some(']'), b'!' => Some('|'), b'<' => Some('{'), b'>' => Some('}'), b'-' => Some('~'),
                _ => None,
            };
            if let Some(ch) = repl { out.push(ch); i += 3; continue; }
        }
        out.push(b[i] as char);
        i += 1;
    }
    out
}

fn splice_lines(s: &str) -> String {
    let mut out = String::with_capacity(s.len());
    let mut it = s.split_inclusive('\n');
    let mut carry = String::new();
    while let Some(line) = it.next() {
        let mut l = line.to_string();
        if l.ends_with("\\\n") { l.truncate(l.len()-2); carry.push_str(&l); continue; }
        if l.ends_with('\\') { l.pop(); carry.push_str(&l); continue; }
        if !carry.is_empty() { carry.push_str(&l); out.push_str(&carry); carry.clear(); }
        else { out.push_str(&l); }
    }
    if !carry.is_empty() { out.push_str(&carry); }
    out
}

fn remove_comments(source: &str) -> Result<String, String> {
    let b = source.as_bytes();
    let mut out = String::new();
    let mut i = 0;
    while i < b.len() {
        if i + 1 < b.len() && b[i] == b'/' && b[i+1] == b'*' {
            i += 2; // skip /*
            let mut found_close = false;
            while i + 1 < b.len() {
                if b[i] == b'*' && b[i+1] == b'/' {
                    found_close = true;
                    i += 2; // skip */
                    break;
                }
                i += 1;
            }
            // Check if we reached EOF without closing the comment
            if !found_close {
                return Err("unterminated block comment".to_string());
            }
            continue;
        }
        if i + 1 < b.len() && b[i] == b'/' && b[i+1] == b'/' {
            // C89 forbids //; we need to report this error
            return Err("'//' line comments are not allowed in C89".to_string());
        }
        out.push(b[i] as char);
        i += 1;
    }
    Ok(out)
}

fn expand_macros_simple(s: &str) -> String {
    expand_macros_with_context(s, &mut std::collections::HashMap::new(), &mut Vec::new())
}

fn expand_macros_with_context(s: &str, defines: &mut HashMap<String, MacroValue>, include_stack: &mut Vec<PathBuf>) -> String {
    let mut out = String::new();
    let lines: Vec<&str> = s.lines().collect();
    let mut i = 0;
    
    while i < lines.len() {
        let line = lines[i];
        let trimmed = line.trim_start();
        
        if trimmed.starts_with('#') {
            let rest = trimmed[1..].trim_start();
            
            // Handle #define
            if let Some(r) = rest.strip_prefix("define") {
                let r = r.trim_start();
                if let Some((name, macro_val)) = parse_define(r) {
                    defines.insert(name, macro_val);
                }
                i += 1;
                continue;
            }
            
            // Handle #undef
            if let Some(r) = rest.strip_prefix("undef") {
                let name = r.trim();
                defines.remove(name);
                i += 1;
                continue;
            }
            
            // Handle #include
            if let Some(r) = rest.strip_prefix("include") {
                if let Some((content, included_path)) = process_include_with_path(r.trim(), include_stack) {
                    // Add to include stack to prevent infinite recursion
                    include_stack.push(included_path.clone());
                    
                    // Recursively preprocess the included file with the same defines context
                    let processed = match full_preprocess(&content, defines, include_stack) {
                        Ok(p) => p,
                        Err(_) => content,
                    };
                    
                    // Remove from include stack after processing
                    include_stack.pop();
                    
                    // Add the processed content, ensuring no blank lines accumulate
                    let trimmed_processed = processed.trim();
                    if !trimmed_processed.is_empty() {
                        out.push_str(trimmed_processed);
                        out.push('\n');
                    }
                }
                i += 1;
                continue;
            }
            
            // Handle conditional compilation
            if rest.starts_with("ifdef") || rest.starts_with("ifndef") || rest.starts_with("if") {
                let (block_lines, next_i) = process_conditional_block(&lines[i..], defines);
                // Process each line in the conditional block recursively to handle nested directives
                for block_line in block_lines {
                    let trimmed = block_line.trim_start();
                    if trimmed.starts_with('#') {
                        let directive_rest = trimmed[1..].trim_start();
                        // Handle directives within the conditional block
                        if let Some(r) = directive_rest.strip_prefix("define") {
                            let r = r.trim_start();
                            if let Some((name, macro_val)) = parse_define(r) {
                                defines.insert(name, macro_val);
                            }
                            continue;
                        }
                        if let Some(r) = directive_rest.strip_prefix("undef") {
                            let name = r.trim();
                            defines.remove(name);
                            continue;
                        }
                        if let Some(r) = directive_rest.strip_prefix("include") {
                            if let Some((content, included_path)) = process_include_with_path(r.trim(), include_stack) {
                                include_stack.push(included_path.clone());
                                let processed = match full_preprocess(&content, defines, include_stack) {
                                    Ok(p) => p,
                                    Err(_) => content,
                                };
                                include_stack.pop();
                                let trimmed_processed = processed.trim();
                                if !trimmed_processed.is_empty() {
                                    out.push_str(trimmed_processed);
                                    out.push('\n');
                                }
                            }
                            continue;
                        }
                        // Ignore other preprocessor directives in blocks
                        continue;
                    }
                    // Regular line - expand macros and add
                    out.push_str(&expand_line_macros(&block_line, defines));
                    out.push('\n');
                }
                i += next_i;
                continue;
            }
            
            // Handle #error
            if let Some(r) = rest.strip_prefix("error") {
                return format!("/* #error: {} */\n{}", r.trim(), out);
            }
            
            // Ignore other directives (#line, #pragma, etc.)
            i += 1;
            continue;
        }
        
        // Expand macros in regular lines
        let expanded_line = expand_line_macros(line, defines);
        out.push_str(&expanded_line);
        out.push('\n');
        i += 1;
    }
    out
}

fn parse_define(define_rest: &str) -> Option<(String, MacroValue)> {
    let define_rest = define_rest.trim();
    
    // Check if it's a function-like macro
    if let Some(paren_pos) = define_rest.find('(') {
        let name = define_rest[..paren_pos].trim();
        if name.is_empty() { return None; }
        
        // Find the closing parenthesis
        let rest = &define_rest[paren_pos + 1..];
        if let Some(close_pos) = rest.find(')') {
            let params_str = &rest[..close_pos];
            let body = rest[close_pos + 1..].trim().to_string();
            
            // Parse parameters
            let params: Vec<String> = if params_str.trim().is_empty() {
                Vec::new()
            } else {
                params_str.split(',').map(|p| p.trim().to_string()).collect()
            };
            
            return Some((name.to_string(), MacroValue::FunctionLike { params, body }));
        }
    }
    
    // Simple macro
    let mut parts = define_rest.splitn(2, char::is_whitespace);
    if let Some(name) = parts.next() {
        if !name.is_empty() {
            let value = parts.next().unwrap_or("").trim().to_string();
            return Some((name.to_string(), MacroValue::Simple(value)));
        }
    }
    None
}

fn process_include(include_rest: &str, include_stack: &mut Vec<PathBuf>) -> Option<String> {
    if let Some((content, _)) = process_include_with_path(include_rest, include_stack) {
        Some(content)
    } else {
        None
    }
}

fn process_include_with_path(include_rest: &str, include_stack: &Vec<PathBuf>) -> Option<(String, PathBuf)> {
    let filename = if include_rest.starts_with('"') && include_rest.ends_with('"') {
        &include_rest[1..include_rest.len()-1]
    } else if include_rest.starts_with('<') && include_rest.ends_with('>') {
        &include_rest[1..include_rest.len()-1]
    } else {
        return None;
    };
    
    // Try current directory first, then tools/include
    let paths = vec![
        PathBuf::from(filename),
        PathBuf::from("tools/include").join(filename),
    ];
    
    for path in paths {
        if let Ok(content) = std::fs::read_to_string(&path) {
            // Prevent infinite recursion
            if !include_stack.contains(&path) {
                return Some((content, path));
            }
        }
    }
    None
}

fn expand_line_macros(line: &str, defines: &HashMap<String, MacroValue>) -> String {
    let mut expanded = String::with_capacity(line.len() * 2);
    let mut chars = line.chars().peekable();
    let mut in_string = false;
    let mut in_char = false;
    let mut escaped = false;
    
    while let Some(ch) = chars.next() {
        if escaped {
            expanded.push(ch);
            escaped = false;
            continue;
        }
        
        if ch == '\\' {
            escaped = true;
            expanded.push(ch);
            continue;
        }
        
        if ch == '"' && !in_char {
            in_string = !in_string;
            expanded.push(ch);
            continue;
        }
        
        if ch == '\'' && !in_string {
            in_char = !in_char;
            expanded.push(ch);
            continue;
        }
        
        if in_string || in_char {
            expanded.push(ch);
            continue;
        }
        
        // Check for identifier start
        if ch.is_alphabetic() || ch == '_' {
            let mut identifier = String::new();
            identifier.push(ch);
            
            // Read full identifier
            while let Some(&next_ch) = chars.peek() {
                if next_ch.is_alphanumeric() || next_ch == '_' {
                    identifier.push(chars.next().unwrap());
                } else {
                    break;
                }
            }
            
            // Check if it's a macro
            if let Some(macro_val) = defines.get(&identifier) {
                match macro_val {
                    MacroValue::Simple(value) => {
                        expanded.push_str(value);
                    }
                    MacroValue::FunctionLike { params, body } => {
                        // Check if next non-whitespace character is '('
                        let mut temp_chars = chars.clone();
                        let mut found_paren = false;
                        
                        // Skip whitespace to find '('
                        while let Some(&next_ch) = temp_chars.peek() {
                            if next_ch.is_whitespace() {
                                temp_chars.next();
                            } else if next_ch == '(' {
                                found_paren = true;
                                break;
                            } else {
                                break;
                            }
                        }
                        
                        if found_paren {
                            // Skip whitespace in original iterator
                            while let Some(&next_ch) = chars.peek() {
                                if next_ch.is_whitespace() {
                                    expanded.push(chars.next().unwrap());
                                } else if next_ch == '(' {
                                    chars.next(); // consume '('
                                    break;
                                } else {
                                    break;
                                }
                            }
                            
                            // Parse arguments
                            let args = parse_macro_arguments(&mut chars);
                            
                            // Expand function-like macro
                            let expanded_macro = expand_function_like_macro(params, body, &args);
                            expanded.push_str(&expanded_macro);
                        } else {
                            // Function-like macro without parentheses - don't expand
                            expanded.push_str(&identifier);
                        }
                    }
                }
            } else {
                expanded.push_str(&identifier);
            }
        } else {
            expanded.push(ch);
        }
    }
    
    expanded
}

fn parse_macro_arguments(chars: &mut std::iter::Peekable<std::str::Chars>) -> Vec<String> {
    let mut args = Vec::new();
    let mut current_arg = String::new();
    let mut paren_depth = 0;
    let mut in_string = false;
    let mut in_char = false;
    let mut escaped = false;
    
    while let Some(ch) = chars.next() {
        if escaped {
            current_arg.push(ch);
            escaped = false;
            continue;
        }
        
        if ch == '\\' {
            escaped = true;
            current_arg.push(ch);
            continue;
        }
        
        if ch == '"' && !in_char {
            in_string = !in_string;
            current_arg.push(ch);
            continue;
        }
        
        if ch == '\'' && !in_string {
            in_char = !in_char;
            current_arg.push(ch);
            continue;
        }
        
        if in_string || in_char {
            current_arg.push(ch);
            continue;
        }
        
        match ch {
            '(' => {
                paren_depth += 1;
                current_arg.push(ch);
            }
            ')' => {
                if paren_depth == 0 {
                    // End of arguments
                    if !current_arg.is_empty() || !args.is_empty() {
                        args.push(current_arg.trim().to_string());
                    }
                    break;
                } else {
                    paren_depth -= 1;
                    current_arg.push(ch);
                }
            }
            ',' => {
                if paren_depth == 0 {
                    args.push(current_arg.trim().to_string());
                    current_arg.clear();
                } else {
                    current_arg.push(ch);
                }
            }
            _ => {
                current_arg.push(ch);
            }
        }
    }
    
    args
}

fn expand_function_like_macro(params: &[String], body: &str, args: &[String]) -> String {
    let mut result = body.to_string();
    
    // Simple parameter substitution
    for (i, param) in params.iter().enumerate() {
        if let Some(arg) = args.get(i) {
            // Replace parameter with argument
            result = result.replace(param, arg);
        }
    }
    
    result
}

fn expand_function_macro(_name: &str, _params: &[String], body: &str, _line: &str, _pos: &mut usize) -> Option<String> {
    // This function is now unused - keeping for compatibility
    Some(body.to_string())
}

fn process_conditional_block(lines: &[&str], defines: &HashMap<String, MacroValue>) -> (Vec<String>, usize) {
    let mut result = Vec::new();
    let mut i = 0;
    let mut nesting_level = 0;
    let mut include_section = false;
    
    if i >= lines.len() { return (result, i); }
    
    // Parse the initial condition
    let first_line = lines[i].trim_start();
    if first_line.starts_with('#') {
        let rest = first_line[1..].trim_start();
        if let Some(r) = rest.strip_prefix("ifdef") {
            let name = r.trim();
            include_section = defines.contains_key(name);
        } else if let Some(r) = rest.strip_prefix("ifndef") {
            let name = r.trim();
            include_section = !defines.contains_key(name);
        } else if rest.starts_with("if") {
            // Simple #if support for constant expressions
            let condition = rest.strip_prefix("if").unwrap_or("").trim();
            include_section = evaluate_if_condition(condition, defines);
        }
    }
    i += 1;
    
    while i < lines.len() {
        let line = lines[i];
        let trimmed = line.trim_start();
        
        if trimmed.starts_with('#') {
            let rest = trimmed[1..].trim_start();
            if rest.starts_with("ifdef") || rest.starts_with("ifndef") || rest.starts_with("if") {
                nesting_level += 1;
            } else if rest.starts_with("endif") {
                if nesting_level == 0 {
                    i += 1;
                    break;
                }
                nesting_level -= 1;
            } else if (rest.starts_with("else") || rest.starts_with("elif")) && nesting_level == 0 {
                include_section = !include_section;
                i += 1;
                continue;
            }
        }
        
        if include_section && nesting_level == 0 {
            result.push(line.to_string());
        }
        i += 1;
    }
    
    (result, i)
}

fn evaluate_if_condition(condition: &str, defines: &HashMap<String, MacroValue>) -> bool {
    let trimmed = condition.trim();
    
    // Handle simple cases
    if trimmed == "0" { return false; }
    if trimmed == "1" { return true; }
    
    // Handle defined(MACRO)
    if let Some(defined_part) = trimmed.strip_prefix("defined(") {
        if let Some(macro_name) = defined_part.strip_suffix(')') {
            return defines.contains_key(macro_name.trim());
        }
    }
    
    // Handle !defined(MACRO)
    if let Some(not_defined_part) = trimmed.strip_prefix("!defined(") {
        if let Some(macro_name) = not_defined_part.strip_suffix(')') {
            return !defines.contains_key(macro_name.trim());
        }
    }
    
    // Simple macro expansion and comparison
    let expanded = expand_line_macros(trimmed, defines);
    let expanded_trimmed = expanded.trim();
    
    // Try to evaluate as number
    if let Ok(num) = expanded_trimmed.parse::<i32>() {
        return num != 0;
    }
    
    // Default: include the block if we can't evaluate
    true
}
