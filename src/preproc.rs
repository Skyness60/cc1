// ----------------------------
// File: src/preproc.rs
// ----------------------------
//! Module préprocesseur C89 pour le compilateur cc1
//! 
//! Ce module implémente les phases de préprocessing du standard C89 :
//! - Phase 1 : Traduction des trigraphes (?? -> caractères spéciaux)
//! - Phase 2 : Fusion des lignes continuées avec backslash
//! - Phase 3 : Suppression des commentaires /* */
//! - Phase 4 : Expansion des macros et traitement des directives #
//! 
//! Le préprocesseur gère :
//! - Les directives #include (locales et système)
//! - Les macros #define (simples et fonction-like)
//! - Les directives #undef
//! - La compilation conditionnelle (#ifdef, #ifndef, #if, #else, #endif)
//! - Les définitions de ligne de commande (-D, -U, -I)

#![allow(dead_code)]

use std::collections::HashMap;  // Pour stocker les définitions de macros
use std::path::PathBuf;         // Pour la gestion des chemins de fichiers

/// Énumération des types de macros supportées
#[derive(Debug, Clone)]
pub enum MacroValue {
    /// Macro simple : #define NAME value
    Simple(String),
    /// Macro fonction : #define NAME(param1, param2) body
    FunctionLike { 
        params: Vec<String>,  // Liste des paramètres de la macro
        body: String          // Corps de la macro à expander
    },
}

/// Structure représentant le préprocesseur avancé
/// Gère les macros, les répertoires d'inclusion et la pile d'inclusion
pub struct AdvancedPreprocessor {
    defines: HashMap<String, MacroValue>,  // Toutes les macros définies
    include_dirs: Vec<String>,             // Répertoires où chercher les fichiers #include
    include_stack: Vec<PathBuf>,           // Pile pour détecter les inclusions circulaires
}

impl AdvancedPreprocessor {
    /// Crée un nouveau préprocesseur avec des définitions et répertoires initiaux
    /// 
    /// # Arguments
    /// * `defines` - HashMap des macros pré-définies (ex: via -D)
    /// * `include_dirs` - Vecteur des répertoires d'inclusion (ex: via -I)
    /// 
    /// # Fonctionnement
    /// - Initialise les macros standard C (__STDC__, __STDC_VERSION__)
    /// - Ajoute les macros spécifiques à la plateforme (__x86_64__, __linux__, etc.)
    pub fn new(defines: HashMap<String, MacroValue>, include_dirs: Vec<String>) -> Self {
        let mut preprocessor = AdvancedPreprocessor {
            defines,
            include_dirs,
            include_stack: Vec::new(),  // Pile d'inclusion initialement vide
        };
        
        // ===== DÉFINITION DES MACROS STANDARD C =====
        
        // Macro indiquant la conformité au standard C
        preprocessor.defines.insert("__STDC__".to_string(), MacroValue::Simple("1".to_string()));
        
        // Version du standard C supporté (C94/Amendment 1)
        preprocessor.defines.insert("__STDC_VERSION__".to_string(), MacroValue::Simple("199409L".to_string()));
        
        // ===== MACROS SPÉCIFIQUES À LA PLATEFORME =====
        
        // Macro d'architecture : définie selon la cible de compilation
        #[cfg(target_arch = "x86_64")]
        preprocessor.defines.insert("__x86_64__".to_string(), MacroValue::Simple("1".to_string()));
        
        #[cfg(target_arch = "x86")]
        preprocessor.defines.insert("__i386__".to_string(), MacroValue::Simple("1".to_string()));
        
        // Macro de système d'exploitation
        #[cfg(target_os = "linux")]
        preprocessor.defines.insert("__linux__".to_string(), MacroValue::Simple("1".to_string()));
        
        preprocessor
    }
    
    /// Lance le préprocessing complet du code source
    /// Délègue vers la fonction full_preprocess avec les définitions et pile actuelles
    pub fn preprocess(&mut self, source: &str) -> Result<String, String> {
        full_preprocess(source, &mut self.defines, &mut self.include_stack)
    }
    
    /// Alias de preprocess() pour compatibilité
    /// Effectue exactement la même opération
    pub fn full_preprocess(&mut self, source: &str) -> Result<String, String> {
        self.preprocess(source)
    }
    
    /// Ajoute un nouveau répertoire à la liste des répertoires d'inclusion
    /// Ces répertoires seront consultés lors du traitement des directives #include
    pub fn add_include_dir(&mut self, dir: String) {
        self.include_dirs.push(dir);
    }
    
    /// Définit une nouvelle macro dans le préprocesseur
    /// 
    /// # Arguments
    /// * `name` - Nom de la macro
    /// * `value` - Valeur de la macro (Simple ou FunctionLike)
    pub fn define_macro(&mut self, name: String, value: MacroValue) {
        self.defines.insert(name, value);
    }
    
    /// Supprime une macro du préprocesseur
    /// Équivalent à la directive #undef
    pub fn undefine_macro(&mut self, name: &str) {
        self.defines.remove(name);
    }
}

/// Fonction de préprocessing basique sans macros ni répertoires personnalisés
/// Utilise un préprocesseur temporaire avec des définitions par défaut uniquement
/// 
/// # Arguments
/// * `source` - Code source C à préprocesser
/// 
/// # Retour
/// * `Ok(String)` - Code préprocessé en cas de succès
/// * `Err(String)` - Message d'erreur en cas d'échec
pub fn basic_preprocess(source: &str) -> Result<String, String> {
    let mut preprocessor = AdvancedPreprocessor::new(HashMap::new(), Vec::new());
    preprocessor.preprocess(source)
}

/// Fonction principale de préprocessing complet
/// Applique toutes les phases de préprocessing du standard C89
/// 
/// # Arguments
/// * `source` - Code source C brut à préprocesser
/// * `defines` - HashMap mutable des définitions de macros
/// * `include_stack` - Pile mutable pour détecter les inclusions circulaires
/// 
/// # Retour
/// * `Ok(String)` - Code préprocessé et prêt pour la compilation
/// * `Err(String)` - Message d'erreur détaillant le problème rencontré
/// 
/// # Phases de traitement
/// 1. **Phase 1** : Traduction des trigraphes (?? -> caractères spéciaux)
/// 2. **Phase 2** : Fusion des lignes continuées (backslash à la fin)
/// 3. **Phase 3** : Suppression des commentaires /* */ (C89 uniquement)
/// 4. **Phase 4** : Expansion des macros et traitement des directives #
pub fn full_preprocess(source: &str, defines: &mut HashMap<String, MacroValue>, include_stack: &mut Vec<PathBuf>) -> Result<String, String> {
    // Phase 1-3: trigraphes, fusion de lignes, suppression de commentaires
    let s1 = translate_trigraphs(source);    // ?? -> caractères spéciaux
    let s2 = splice_lines(&s1);              // Fusion des lignes avec \ à la fin
    let s3 = remove_comments(&s2)?;          // Suppression /* */ (peut échouer)
    
    // Phase 4: expansion des macros et traitement des directives
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
            if !found_close {
                return Err("unterminated block comment".to_string());
            }
            continue;
        }
        if i + 1 < b.len() && b[i] == b'/' && b[i+1] == b'/' {
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
            let _rest = trimmed[1..].trim_start();
            
            if trimmed.starts_with('#') {
                let rest = trimmed[1..].trim_start();
                if rest.starts_with("include") {
                    let inc_arg = rest[7..].trim();
                    if inc_arg.starts_with('<') && inc_arg.ends_with('>') {
                        i += 1;
                        continue;
                    }
                    // Otherwise, handle as before (local includes)
                    if let Some((content, included_path)) = process_include_with_path(inc_arg, include_stack) {
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
                    i += 1;
                    continue;
                }
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
                // Handle conditional compilation
                if rest.starts_with("ifdef") || rest.starts_with("ifndef") || rest.starts_with("if") {
                    let (block_lines, next_i) = process_conditional_block(&lines[i..], defines);
                    for block_line in block_lines {
                        let trimmed = block_line.trim_start();
                        if trimmed.starts_with('#') {
                            let directive_rest = trimmed[1..].trim_start();
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
                            if directive_rest.starts_with("include") {
                                let inc_arg = directive_rest[7..].trim();
                                if inc_arg.starts_with('<') && inc_arg.ends_with('>') {
                                    // Ignore system header includes in conditionals
                                    continue;
                                }
                                if let Some((content, included_path)) = process_include_with_path(inc_arg, include_stack) {
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
                            continue;
                        }
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
            
            // Ignore other directives (#line, #pragma, etc.)
            i += 1;
            continue;
        }
        
        // Expand macros in regular lines
        let expanded_line = expand_line_macros(line, defines);
        // Skip builtin typedefs that our parser doesn't understand
        if expanded_line.trim().starts_with("typedef __builtin_") {
            i += 1;
            continue;
        }
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
    
    true
}
