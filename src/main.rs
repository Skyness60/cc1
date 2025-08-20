// ============================================================================================
// Fichier: src/main.rs - Point d'entrée principal du compilateur C cc1
// ============================================================================================
//! 
//! # Compilateur C89 cc1
//! 
//! Ce fichier contient le point d'entrée principal du compilateur C cc1, qui implémente
//! une chaîne de compilation complète pour le standard C89.
//! 
//! ## Architecture du compilateur
//! 
//! Le compilateur cc1 est organisé en plusieurs phases distinctes :
//! 
//! ### 1. Front-end (Analyse)
//! - **Préprocesseur** : Gère les directives #include, #define, etc.
//! - **Analyseur lexical** : Transforme le texte source en tokens
//! - **Analyseur syntaxique** : Construit l'Abstract Syntax Tree (AST)
//! - **Analyseur sémantique** : Vérifie la cohérence des types et symboles
//! 
//! ### 2. Back-end (Génération)
//! - **Générateur LLVM IR** : Produit du code intermédiaire LLVM
//! 
//! ## Modes d'exécution supportés
//! 
//! - **Mode normal** : Compilation complète vers LLVM IR
//! - **Mode préprocesseur** : Arrête après le préprocessing
//! - **Mode lexical** : Arrête après la tokenisation
//! - **Mode parsing** : Arrête après l'analyse syntaxique
//! - **Mode sémantique** : Arrête après l'analyse sémantique
//! 
//! ## Options de ligne de commande
//! 
//! - Architecture cible : `-m32`, `-m64`
//! - Génération de debug : `-g`
//! - Définition de macros : `-DMACRO=value`
//! - Répertoires d'inclusion : `-I/path/to/includes`
//! - Fichier de sortie : `-o outfile`
//! 

// ============================================================================================
// IMPORTATION DES MODULES ET BIBLIOTHÈQUES
// ============================================================================================

// Importation des modules spécifiques au compilateur cc1
mod front;    // Module front-end contenant lexer, parser et analyse sémantique
mod back;     // Module back-end pour la génération de code LLVM IR  
mod preproc;  // Module préprocesseur pour les directives C (#include, #define, etc.)

// Importation des bibliothèques standard Rust nécessaires
use std::env;                           // Permet d'accéder aux arguments de la ligne de commande
use std::fs;                           // Fonctions de lecture/écriture de fichiers sur le système
use std::io::{self, Write, Read};      // Traits et types pour les entrées/sorties (stdin, stdout, fichiers)

// ============================================================================================
// FONCTIONS UTILITAIRES
// ============================================================================================

/// Affiche l'aide sur l'utilisation correcte du programme cc1
/// 
/// Cette fonction est appelée dans deux cas :
/// 1. Quand aucun argument n'est fourni sur la ligne de commande
/// 2. Quand les arguments fournis sont incorrects ou insuffisants
/// 
/// Elle affiche le format d'utilisation sur stderr et permet à l'utilisateur
/// de comprendre comment invoquer correctement le compilateur.
fn usage() { 
    eprintln!("usage: cc1 infile [-o outfile]");
}

// ============================================================================================
// FONCTION PRINCIPALE - POINT D'ENTRÉE DU COMPILATEUR
// ============================================================================================

/// Point d'entrée principal du compilateur cc1
/// 
/// Cette fonction orchestrale l'ensemble du processus de compilation, depuis l'analyse
/// des arguments de ligne de commande jusqu'à la génération du code de sortie.
/// 
/// # Processus de compilation détaillé :
/// 
/// ## Phase 1 : Initialisation
/// - Récupération et analyse des arguments de ligne de commande
/// - Validation des options et paramètres fournis
/// - Configuration des modes d'exécution (normal, debug, modes spéciaux)
/// 
/// ## Phase 2 : Lecture des sources
/// - Lecture du fichier source C depuis le disque ou stdin
/// - Gestion des erreurs de lecture de fichier
/// 
/// ## Phase 3 : Préprocessing
/// - Application des directives de préprocesseur (#include, #define, etc.)
/// - Gestion des macros définies via -D et des répertoires d'inclusion via -I
/// - Expansion des macros et inclusion des fichiers d'en-tête
/// 
/// ## Phase 4 : Compilation selon le mode choisi
/// - **Mode lexical** : Tokenisation et affichage des tokens
/// - **Mode parsing** : Analyse syntaxique et affichage de l'AST
/// - **Mode sémantique** : Vérification des types et symboles
/// - **Mode normal** : Génération complète de code LLVM IR
/// 
/// ## Phase 5 : Sortie
/// - Écriture du résultat vers stdout ou fichier spécifié
/// - Gestion des erreurs et codes de retour appropriés
/// 
/// # Gestion d'erreurs
/// 
/// La fonction implémente une gestion d'erreurs robuste :
/// - Validation de tous les arguments avant traitement
/// - Messages d'erreur localisés avec nom de fichier et position
/// - Codes de sortie appropriés pour l'intégration dans des scripts
/// - Mode de récupération d'erreurs pour continuer malgré certaines erreurs
/// 
fn main() {
    // ========================================================================================
    // PHASE 1 : RÉCUPÉRATION ET TRAITEMENT DES ARGUMENTS DE LIGNE DE COMMANDE
    // ========================================================================================
    
    // Récupération de tous les arguments passés au programme
    // args[0] est toujours le nom du programme lui-même, on le supprime
    let mut args: Vec<String> = env::args().collect();
    args.remove(0);  // Suppression du nom du programme (premier élément)
    
    // Vérification que des arguments ont été fournis
    // Si aucun argument, on affiche l'aide et on quitte avec un code d'erreur
    if args.is_empty() { 
        usage();                    // Affiche l'aide sur stderr
        std::process::exit(1);      // Quitte avec code d'erreur 1
    }

    // ========================================================================================
    // PHASE 2 : DÉCLARATION DES VARIABLES DE CONFIGURATION
    // ========================================================================================
    
    // Ces variables stockent toutes les options configurables du compilateur
    // Elles sont initialisées avec des valeurs par défaut appropriées
    
    // --- Fichiers d'entrée et de sortie ---
    let mut infile: Option<String> = None;              // Nom du fichier source C à compiler (obligatoire)
    let mut outfile: Option<String> = None;             // Nom du fichier de sortie (optionnel, stdout par défaut)
    let mut debug_source_file: Option<String> = None;   // Nom alternatif pour les métadonnées de debug
    
    // --- Modes d'exécution exclusifs ---
    // Un seul de ces modes peut être actif à la fois
    // Ils permettent d'arrêter la compilation à différentes étapes pour déboguer
    let mut lex_only_mode = false;        // Mode : Arrêter après la tokenisation (--lex-only)
    let mut parse_expr_mode = false;      // Mode : Arrêter après parsing d'expressions (--parse-expr)
    let mut parse_tu_mode = false;        // Mode : Arrêter après parsing d'unité de traduction (--parse-tu)
    let mut sem_mode = false;             // Mode : Arrêter après l'analyse sémantique (--sem)
    let mut preprocess_only_mode = false; // Mode : Arrêter après le préprocessing (--preprocess-only)
    
    // --- Options de compilation ---
    let mut arch_opt: Option<front::semantics::Arch> = None; // Architecture cible (i386/x86_64, None = défaut i386)
    let mut debug_info = false;           // Activation de la génération d'informations de debug (-g)
    let mut continue_on_error = false;    // Continue la compilation malgré certaines erreurs (--continue-on-error)
    let mut max_errors = 10;              // Nombre maximum d'erreurs de parsing avant arrêt (--max-errors=N)
    
    // --- Options du préprocesseur ---
    // Ces collections stockent les paramètres de préprocessing fournis via la ligne de commande
    let mut defines = std::collections::HashMap::new();      // Macros définies avec -D (nom -> valeur)
    let mut undefines = std::collections::HashSet::new();    // Macros à supprimer avec -U (juste les noms)
    let mut include_dirs = Vec::new();                       // Répertoires d'inclusion avec -I (liste de chemins)

    // ========================================================================================
    // PHASE 3 : PARSING SÉQUENTIEL DES ARGUMENTS DE LIGNE DE COMMANDE
    // ========================================================================================
    
    // Parcours manuel de tous les arguments avec gestion d'état
    // Certaines options consomment l'argument suivant, d'où la gestion explicite de l'index
    let mut i = 0;  // Index courant dans le tableau des arguments
    
    while i < args.len() {
        // Pattern matching sur l'argument courant
        // Chaque branche gère un type d'option différent
        match args[i].as_str() {
            
            // ================================================================================
            // MODES D'EXÉCUTION SPÉCIAUX (options --xxx)
            // ================================================================================
            
            // Ces modes permettent d'arrêter la compilation à différentes étapes
            // Utiles pour déboguer le compilateur ou comprendre son fonctionnement interne
            
            "--lex-only" => { 
                lex_only_mode = true;        // Active le mode tokenisation seule
                i += 1;                      // Passe à l'argument suivant
            }
            "--parse-expr" => { 
                parse_expr_mode = true;      // Active le mode parsing d'expressions
                i += 1; 
            }
            "--parse-tu" => { 
                parse_tu_mode = true;        // Active le mode parsing d'unité de traduction
                i += 1; 
            }
            "--sem" => { 
                sem_mode = true;             // Active le mode analyse sémantique seule
                i += 1; 
            }
            "--preprocess-only" => { 
                preprocess_only_mode = true; // Active le mode préprocesseur seul
                i += 1; 
            }
            
            // ================================================================================
            // OPTIONS D'ARCHITECTURE CIBLE (-m32, -m64)
            // ================================================================================
            
            // Ces options déterminent l'architecture cible pour la génération de code
            // Elles influencent les tailles des types et l'ABI utilisée
            
            "-m32" => { 
                arch_opt = Some(front::semantics::Arch::I386);    // Cible i386 32-bit (Intel x86 32-bit)
                i += 1; 
            }
            "-m64" => { 
                arch_opt = Some(front::semantics::Arch::X86_64);  // Cible x86_64 64-bit (Intel x86 64-bit)
                i += 1; 
            }
            
            // ================================================================================
            // OPTIONS DE DEBUG ET GESTION D'ERREURS
            // ================================================================================
            
            "-g" => { 
                debug_info = true;           // Active la génération d'informations de debug DWARF
                i += 1; 
            }
            "--continue-on-error" => { 
                continue_on_error = true;    // Continue malgré les erreurs de parsing (mode récupération)
                i += 1; 
            }
            
            // ================================================================================
            // OPTIONS AVEC VALEURS OBLIGATOIRES (format --option value)
            // ================================================================================
            
            "--debug-source-file" => {
                // Spécifie un nom de fichier alternatif pour les métadonnées de debug
                // Utile quand le fichier d'entrée est stdin ou un nom temporaire
                if i + 1 >= args.len() { 
                    eprintln!("cc1: error: --debug-source-file requires a value"); 
                    std::process::exit(1);
                } 
                debug_source_file = Some(args[i+1].clone());  // Stocke le nom du fichier pour debug
                i += 2;  // Passe l'option ET sa valeur
            }
            
            "-o" => { 
                // Spécifie le fichier de sortie
                // Si absent, la sortie se fait vers stdout
                if i + 1 >= args.len() { 
                    eprintln!("cc1: error: -o requires a value"); 
                    std::process::exit(1);
                } 
                outfile = Some(args[i+1].clone());  // Stocke le nom du fichier de sortie
                i += 2;  // Passe l'option ET sa valeur
            }
            
            // ================================================================================
            // OPTIONS AVEC VALEURS INTÉGRÉES (format --option=value)
            // ================================================================================
            
            s if s.starts_with("--max-errors=") => {
                // Définit le nombre maximum d'erreurs de parsing avant arrêt
                // Format : --max-errors=10
                let number_part = &s[13..];  // Récupère tout après "--max-errors="
                
                match number_part.parse::<usize>() {
                    Ok(n) => max_errors = n,  // Parse réussi : utilise la valeur
                    Err(_) => {
                        eprintln!("cc1: error: invalid value for --max-errors: {}", number_part);
                        std::process::exit(1);
                    }
                }
                i += 1;
            }
            
            // ================================================================================
            // OPTIONS DU PRÉPROCESSEUR - DÉFINITION DE MACROS (-D)
            // ================================================================================
            
            s if s.starts_with("-D") => {
                // Définit une macro du préprocesseur
                // Formats supportés : -DMACRO, -DMACRO=value, -D MACRO, -D MACRO=value
                
                let define_str = &s[2..];  // Récupère tout après "-D"
                
                if define_str.is_empty() && i + 1 < args.len() {
                    // Format "-D MACRO" : la définition est dans l'argument suivant
                    i += 1;
                    let define_str = &args[i];
                    parse_define_option(define_str, &mut defines);
                } else {
                    // Format "-DMACRO" ou "-DMACRO=value" : la définition est collée à -D
                    parse_define_option(define_str, &mut defines);
                }
                i += 1;
            }
            
            // ================================================================================
            // OPTIONS DU PRÉPROCESSEUR - SUPPRESSION DE MACROS (-U)
            // ================================================================================
            
            s if s.starts_with("-U") => {
                // Supprime une macro du préprocesseur (annule une définition précédente)
                // Formats supportés : -UMACRO, -U MACRO
                
                let undef_str = &s[2..];  // Récupère tout après "-U"
                
                if undef_str.is_empty() && i + 1 < args.len() {
                    // Format "-U MACRO" : le nom est dans l'argument suivant
                    i += 1;
                    undefines.insert(args[i].clone());  // Ajoute le nom à la liste des suppressions
                } else {
                    // Format "-UMACRO" : le nom est collé à -U
                    undefines.insert(undef_str.to_string());
                }
                i += 1;
            }
            
            // ================================================================================
            // OPTIONS DU PRÉPROCESSEUR - RÉPERTOIRES D'INCLUSION (-I)
            // ================================================================================
            
            s if s.starts_with("-I") => {
                // Ajoute un répertoire à la liste de recherche pour les #include
                // Formats supportés : -Ipath, -I path
                
                let include_str = &s[2..];  // Récupère tout après "-I"
                
                if include_str.is_empty() && i + 1 < args.len() {
                    // Format "-I path" : le chemin est dans l'argument suivant
                    i += 1;
                    include_dirs.push(args[i].clone());  // Ajoute le répertoire à la liste
                } else {
                    // Format "-Ipath" : le chemin est collé à -I
                    include_dirs.push(include_str.to_string());
                }
                i += 1;
            }
            
            // ================================================================================
            // OPTION DE SORTIE ALTERNATIVE (format -ofile)
            // ================================================================================
            
            s if s.starts_with("-o") => { 
                // Format alternatif pour spécifier le fichier de sortie : -ofile
                // Équivalent à -o file mais sans espace
                outfile = Some(s[2..].to_string());  // Récupère tout après "-o"
                i += 1; 
            }
            
            // ================================================================================
            // GESTION DES OPTIONS INCONNUES
            // ================================================================================
            
            s if s.starts_with('-') && s != "-" => { 
                // Toute option commençant par '-' qui n'est pas reconnue
                // Exception : "-" seul est accepté (stdin)
                eprintln!("cc1: error: unknown option: {}", s); 
                std::process::exit(1);
            } 
            
            // ================================================================================
            // FICHIER D'ENTRÉE (argument sans préfixe -)
            // ================================================================================
            
            s => { 
                // Tout argument ne commençant pas par '-' est considéré comme le fichier source
                if infile.is_some() { 
                    // Un seul fichier d'entrée est supporté
                    eprintln!("cc1: error: multiple input files not supported"); 
                    std::process::exit(1);
                } 
                infile = Some(s.to_string());  // Définit le fichier source à compiler
                i += 1; 
            }
        }
    }

    // ========================================================================================
    // FONCTION UTILITAIRE LOCALE : PARSING DES DÉFINITIONS DE MACROS
    // ========================================================================================
    
    /// Parse une définition de macro du format -D
    /// 
    /// Cette fonction analyse les chaînes passées après -D et les convertit
    /// en entrées appropriées dans la table des macros.
    /// 
    /// # Formats supportés :
    /// - `MACRO` → `MACRO=1` (valeur par défaut)
    /// - `MACRO=value` → `MACRO=value` (valeur explicite)
    /// - `MACRO=` → `MACRO=""` (valeur vide)
    /// 
    /// # Arguments :
    /// - `define_str` : La chaîne après -D (ex: "DEBUG=1" ou "VERBOSE")
    /// - `defines` : Table de hachage où stocker les définitions
    /// 
    /// # Exemple d'utilisation :
    /// ```
    /// parse_define_option("DEBUG=1", &mut defines);    // defines["DEBUG"] = "1"
    /// parse_define_option("VERBOSE", &mut defines);    // defines["VERBOSE"] = "1"
    /// ```
    fn parse_define_option(define_str: &str, defines: &mut std::collections::HashMap<String, preproc::MacroValue>) {
        // Recherche d'un signe '=' pour séparer le nom de la macro de sa valeur
        if let Some(eq_pos) = define_str.find('=') {
            // Format "MACRO=value" : sépare nom et valeur
            let name = define_str[..eq_pos].to_string();      // Partie avant '='
            let value = define_str[eq_pos + 1..].to_string(); // Partie après '='
            defines.insert(name, preproc::MacroValue::Simple(value));
        } else {
            // Format "MACRO" : définit la macro avec la valeur par défaut "1"
            // C'est la convention standard pour les macros booléennes
            defines.insert(define_str.to_string(), preproc::MacroValue::Simple("1".to_string()));
        }
    }

    // ========================================================================================
    // PHASE 4 : VALIDATION DES ARGUMENTS ET LECTURE DU FICHIER SOURCE
    // ========================================================================================
    
    // Vérification qu'un fichier d'entrée a été spécifié
    // Cette vérification est obligatoire car le compilateur ne peut pas fonctionner sans source
    let infile = match infile {
        Some(s) => s,  // Fichier spécifié : utilise ce nom pour la suite
        None => { 
            usage();                    // Affiche l'aide utilisateur
            std::process::exit(1);      // Quitte avec code d'erreur
        }
    };
    
    // Lecture du contenu du fichier source
    // Le compilateur supporte deux modes d'entrée :
    // 1. Fichier spécifique sur le disque
    // 2. Entrée standard (stdin) si le nom de fichier est "-"
    let source = if infile == "-" {
        // Mode stdin : lit depuis l'entrée standard
        // Utile pour les pipes Unix ou la lecture interactive
        let mut s = String::new();
        if let Err(e) = io::stdin().read_to_string(&mut s) { 
            eprintln!("cc1: error: could not read <stdin>: {}", e); 
            std::process::exit(1);
        } 
        s  // Retourne le contenu lu depuis stdin
    } else {
        // Mode fichier : lit depuis le système de fichiers
        match fs::read_to_string(&infile) { 
            Ok(s) => s,  // Lecture réussie : retourne le contenu
            Err(e) => { 
                // Erreur de lecture : affiche l'erreur et quitte
                eprintln!("cc1: error: could not read {}: {}", infile, e); 
                std::process::exit(1);
            } 
        }
    };

    // ========================================================================================
    // PHASE 5 : CONFIGURATION DU CANAL DE SORTIE
    // ========================================================================================
    
    // Détermine où écrire le résultat de la compilation
    // Le compilateur supporte trois modes de sortie :
    // 1. Fichier spécifique (-o filename)
    // 2. Sortie standard (-o - ou pas d'option -o)
    // 3. Gestion d'erreurs de création de fichier
    let mut out: Box<dyn Write> = match outfile.as_deref() {
        Some("-") => Box::new(io::stdout()),  // "-" signifie explicitement stdout
        Some(path) => {
            // Fichier spécifique : tente de le créer
            match fs::File::create(path) { 
                Ok(f) => Box::new(f),  // Création réussie : utilise ce fichier
                Err(e) => { 
                    // Erreur de création : affiche l'erreur et quitte
                    eprintln!("cc1: error: could not create {}: {}", path, e); 
                    std::process::exit(1);
                }
            }
        },
        None => Box::new(io::stdout()),  // Pas d'option -o : utilise stdout par défaut
    };

    // ========================================================================================
    // PHASE 6 : GESTION DU MODE PRÉPROCESSEUR UNIQUEMENT
    // ========================================================================================
    
    // Si le mode préprocesseur seul est activé, on traite et on sort immédiatement
    // Ce mode est utile pour déboguer les directives de préprocessing ou voir le code après expansion
    if preprocess_only_mode {
        match preproc::basic_preprocess(&source) {
            Ok(s) => {
                writeln!(out, "{}", s).ok();  // Écrit le code préprocessé vers la sortie
                return;  // Termine immédiatement l'exécution
            }
            Err(err) => {
                // Erreur de préprocessing : affiche avec localisation
                eprintln!("{}:1:1: error: {}", infile, err);
                std::process::exit(1);
            }
        }
    }

    // ========================================================================================
    // PHASE 7 : PRÉPROCESSING AVANCÉ AVEC MACROS ET INCLUDES
    // ========================================================================================
    
    // Détermine quel préprocesseur utiliser selon les options spécifiées
    // - Préprocesseur avancé si des macros (-D) ou includes (-I) sont définis
    // - Préprocesseur basique sinon
    let psource = if !defines.is_empty() || !include_dirs.is_empty() {
        // Préprocesseur avancé : support complet des macros et répertoires d'inclusion
        let mut adv_preprocessor = preproc::AdvancedPreprocessor::new(defines.clone(), include_dirs.clone());
        match adv_preprocessor.full_preprocess(&source) {
            Ok(s) => s,  // Préprocessing réussi : utilise le code traité
            Err(err) => {
                eprintln!("{}:1:1: error: {}", infile, err);
                std::process::exit(1);
            }
        }
    } else {
        // Préprocesseur basique : traitement minimal (commentaires, continuations de ligne)
        match preproc::basic_preprocess(&source) {
            Ok(s) => s,  // Préprocessing réussi : utilise le code traité
            Err(err) => {
                eprintln!("{}:1:1: error: {}", infile, err);
                std::process::exit(1);
            }
        }
    };

    // ========================================================================================
    // PHASE 8 : GESTION DES MODES D'EXÉCUTION SPÉCIAUX
    // ========================================================================================
    
    // Ces modes permettent d'arrêter la compilation à différentes étapes
    // Très utiles pour déboguer le compilateur ou comprendre son fonctionnement
    
    // ----------------------------------------------------------------------------------------
    // MODE LEXICAL UNIQUEMENT : Tokenisation et affichage
    // ----------------------------------------------------------------------------------------
    if lex_only_mode {
        // Création du lexer (analyseur lexical) avec le code préprocessé
        let mut lexer = front::lexer::Lexer::new(&psource, &infile);
        
        // Boucle de tokenisation : lit tous les tokens jusqu'à EOF
        loop {
            match lexer.next() {  // Demande le prochain token au lexer
                Some(Ok(token)) => {
                    // Token lu avec succès
                    if token.kind == front::token::TokenKind::Eof { 
                        break;  // Arrête à EOF (End Of File)
                    }
                    writeln!(out, "{:?}", token).ok();  // Affiche le token avec debug formatting
                }
                Some(Err(err)) => { 
                    // Erreur lexicale (caractère invalide, littéral malformé, etc.)
                    eprintln!("{}:{}:{}: error: {}", err.file, err.span.line, err.span.col, err.msg); 
                    std::process::exit(1); 
                }
                None => break,  // Plus de tokens à lire (normalement ne devrait pas arriver)
            }
        }
        return;  // Termine après la tokenisation
    }

    // ----------------------------------------------------------------------------------------
    // MODE PARSING D'UNITÉ DE TRADUCTION : AST complet
    // ----------------------------------------------------------------------------------------
    if parse_tu_mode {
        // Création du parser (analyseur syntaxique)
        let mut p = front::parser::Parser::new(&psource, &infile);
        
        // Parse l'unité de traduction complète (tout le fichier C)
        match p.parse_translation_unit() {
            Ok(tu) => { 
                // Parsing réussi : affiche l'AST complet avec formatage pretty-print
                writeln!(out, "{:#?}", tu).ok();  // {:#?} = debug formatting avec indentation
            }
            Err(err) => { 
                // Erreur de parsing : affiche avec localisation précise
                eprintln!("{}:{}:{}: error: {}", infile, err.span.line, err.span.col, err.msg); 
                std::process::exit(1); 
            }
        }
        return;  // Termine après le parsing
    }

    // ----------------------------------------------------------------------------------------
    // MODE PARSING D'EXPRESSIONS : AST d'expressions spécifiques
    // ----------------------------------------------------------------------------------------
    if parse_expr_mode {
        // Création du parser pour les expressions
        let mut p = front::parser::Parser::new(&psource, &infile);
        
        if infile == "-" {
            // Mode interactif depuis stdin : parse une seule expression
            match p.parse_expr() {
                Ok(ast) => { 
                    writeln!(out, "{:#?}", ast).ok();  // Affiche l'AST de l'expression
                }
                Err(err) => { 
                    eprintln!("{}:{}:{}: error: {}", infile, err.span.line, err.span.col, err.msg); 
                    std::process::exit(1); 
                }
            }
        } else {
            // Mode fichier : parse plusieurs expressions séparées par ';'
            match p.parse_exprs_semi() {
                Ok(list) => { 
                    // Affiche l'AST de chaque expression trouvée
                    for e in list { 
                        writeln!(out, "{:#?}", e).ok(); 
                    } 
                }
                Err(err) => { 
                    eprintln!("{}:{}:{}: error: {}", infile, err.span.line, err.span.col, err.msg); 
                    std::process::exit(1); 
                }
            }
        }
        return;  // Termine après le parsing d'expressions
    }

    // ----------------------------------------------------------------------------------------
    // MODE ANALYSE SÉMANTIQUE : Vérification des types et symboles
    // ----------------------------------------------------------------------------------------
    if sem_mode {
        // Création du parser pour l'analyse sémantique
        let mut p = front::parser::Parser::new(&psource, &infile);
        
        // Parse d'abord l'unité de traduction complète
        match p.parse_translation_unit() {
            Ok(tu) => {
                // Lance l'analyse sémantique sur l'AST généré
                // Cette phase vérifie :
                // - La cohérence des types (int = float est une erreur)
                // - La déclaration des variables avant utilisation
                // - La cohérence des signatures de fonctions
                // - Les règles de portée des identificateurs
                let diags = front::semantics::analyze_translation_unit(&tu);
                
                if diags.is_empty() {
                    // Aucune erreur sémantique trouvée : programme valide
                    writeln!(out, "OK").ok();
                } else {
                    // Affiche toutes les erreurs sémantiques trouvées
                    for d in diags.errors {
                        eprintln!("{}:{}:{}: error: {}", infile, d.span.line, d.span.col, d.msg);
                    }
                    std::process::exit(1);
                }
            }
            Err(err) => { 
                // Erreur de parsing avant même l'analyse sémantique
                eprintln!("{}:{}:{}: error: {}", infile, err.span.line, err.span.col, err.msg); 
                std::process::exit(1); 
            }
        }
        return;  // Termine après l'analyse sémantique
    }

    // ========================================================================================
    // PHASE 9 : MODE PAR DÉFAUT - COMPILATION COMPLÈTE VERS LLVM IR
    // ========================================================================================
    
    // Cette phase représente le mode normal de fonctionnement du compilateur
    // Elle effectue la compilation complète depuis le code C jusqu'au LLVM IR
    
    // ----------------------------------------------------------------------------------------
    // Configuration de la récupération d'erreurs pour le parsing robuste
    // ----------------------------------------------------------------------------------------
    let error_recovery = front::parser::ErrorRecovery::new(continue_on_error, max_errors);
    
    // Création du parser avec gestion d'erreurs avancée
    // Ce parser peut potentiellement continuer après certaines erreurs si configuré
    let mut p = match front::parser::Parser::new_with_recovery(&psource, &infile, error_recovery) {
        Ok(p) => p,  // Parser créé avec succès
        Err(e) => {
            // Erreur fatale lors de la création du parser
            eprintln!("{}:1:1: error: {}", infile, e);
            std::process::exit(1);
        }
    };
    
    // ----------------------------------------------------------------------------------------
    // Parsing de l'unité de traduction complète avec gestion d'erreurs
    // ----------------------------------------------------------------------------------------
    match p.parse_translation_unit() {
        Ok(tu) => {
            // ============================================================================
            // GÉNÉRATION DU CODE LLVM IR (Back-end du compilateur)
            // ============================================================================
            
            // Détermination de l'architecture cible
            // Par défaut, on utilise i386 (32-bit) si aucune architecture n'est spécifiée
            let arch = arch_opt.unwrap_or(front::semantics::Arch::I386);
            
            // Détermination du nom de fichier pour les métadonnées de debug
            // Utilise le nom alternatif s'il est spécifié, sinon le nom d'entrée original
            let debug_filename = debug_source_file.as_ref().unwrap_or(&infile);
            
            // Lancement de la génération de code LLVM IR
            // Cette fonction transforme l'AST en code LLVM IR équivalent
            // Le résultat est un module LLVM complet prêt à être compilé
            let m = back::lower_to_llvm_ir(&tu, arch, debug_info, debug_filename);
            
            // Note : la variable debug_info est actuellement réservée pour de futures
            // fonctionnalités de métadonnées de debug DWARF
            let _ = debug_info; 
            
            // Écriture du code LLVM IR généré vers la sortie configurée
            writeln!(out, "{}", m.text).ok();
        }
        Err(err) => { 
            // ============================================================================
            // GESTION DES ERREURS DE PARSING
            // ============================================================================
            
            // Erreur de parsing : affiche l'erreur avec localisation précise
            // Le format inclut : nom_fichier:ligne:colonne: error: message
            eprintln!("{}:{}:{}: error: {}", infile, err.span.line, err.span.col, err.msg); 
            
            // Gestion spéciale si le mode de récupération d'erreurs était activé
            if continue_on_error && p.has_recovered_errors() {
                eprintln!("Compilation terminée avec erreurs récupérables.");
            }
            
            // Termine avec code d'erreur pour indiquer l'échec
            std::process::exit(1); 
        }
    }
}