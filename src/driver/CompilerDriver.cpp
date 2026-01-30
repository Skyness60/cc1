#include <driver/CompilerDriver.hpp>
#include <parser/ast/AST.hpp>
#include <parser/Parser.hpp>
#include <parser/ParseError.hpp>
#include <semantics/SymbolTable.hpp>
#include <semantics/SemanticAnalyzer.hpp>
#include <codegen/IRGenerator.hpp>
#include <lexer/Lexer.hpp>
#include <iostream>

using cc1::DebugLogger;

// EN: Builds the driver from parsed options and sets debug logging if enabled.
// FR: Construit le driver depuis les options et active le debug si besoin.
CompilerDriver::CompilerDriver(const CompilerOptions& opts)
    : input_files_(opts.inputFiles),
      output_file_(opts.outputFile),
      syntax_only_(opts.syntaxOnly),
      preprocess_only_(opts.preprocessOnly),
      debug_mode_(opts.debugMode),
            debug_info_(opts.debugInfo),
    is64bit_(opts.is64bit),
      defines_(opts.defines),
      undefines_(opts.undefines),
      include_paths_(opts.includePaths)
{
    symbols_.reset(new SymbolTable());
    
    // Treat '-' as meaning stdout for output. Normalise by clearing the
    // output filename so downstream code that writes to stdout (when
    // output_file_ is empty) will be used for both preprocessing and
    // code generation.
    if (output_file_ == "-") {
        output_file_.clear();
    }

    if (debug_mode_) {
        DebugLogger::instance().setEnabled(true);
        DebugLogger::instance().setOutputFile("cc1_debug.log");
    }
}

// EN: Defaulted destructor since resources are RAII-managed.
// FR: Destructeur par defaut car les ressources sont RAII.
CompilerDriver::~CompilerDriver() = default;

// EN: Runs the full compile pipeline with early exits on requested phases.
// FR: Execute le pipeline complet avec arrets selon options.
bool CompilerDriver::compile()
{
    if (!runPreprocessing())
        return false;
    if (preprocess_only_)
        return true;
    if (!runLexing())
        return false;
    if (!runParsing())
        return false;
    if (syntax_only_)
        return true;
    if (!runSemantics())
        return false;
    if (!runCodeGen())
        return false;
    return true;
}

// EN: Runs lexing on the preprocessed source and optionally dumps tokens.
// FR: Lance le lexing sur la source pretraitee et dump les tokens si demande.
bool CompilerDriver::runLexing()
{
    if (input_files_.empty()) {
        std::cerr << "Error: No input files provided." << std::endl;
        return false;
    }

    std::string filename = input_files_[0];
    
    
    Lexer lexer(source_, filename);
    tokens_ = lexer.tokenize();

    if (syntax_only_) {
        for (const auto& token : tokens_) {
            std::cout << token << std::endl;
        }
    }

    return true;
}

// EN: Parses the token stream into an AST, handling parse errors.
// FR: Parse le flux de tokens en AST et gere les erreurs.
bool CompilerDriver::runParsing()
{
    if (tokens_.empty()) {
        return true;  
    }
    
    std::string filename = input_files_.empty() ? "<input>" : input_files_[0];
    
    try {
        cc1::Parser parser(tokens_, filename, source_);
        ast_ = parser.parse();
        
        
        if (parser.hadError()) {
            return false;
        }
        return true;
    } catch (const ParseError& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

// EN: Runs semantic analysis on the AST and reports errors.
// FR: Lance l analyse semantique sur l AST et rapporte les erreurs.
bool CompilerDriver::runSemantics()
{
    if (!ast_) return true;
    
    std::string filename = input_files_.empty() ? "<input>" : input_files_[0];
    cc1::SemanticAnalyzer analyzer(filename, source_, is64bit_);
    analyzer.analyze(*ast_);
    
    return !analyzer.hadError();
}

// EN: Generates LLVM IR and writes it to file or stdout.
// FR: Genere l IR LLVM et l ecrit en fichier ou stdout.
bool CompilerDriver::runCodeGen()
{
    if (!ast_) return true;
    
    cc1::IRGenerator generator(is64bit_);
    if (debug_info_) {
        std::string filename = input_files_.empty() ? std::string("<input>") : input_files_[0];
        generator.setDebugInfo(true, filename);
    }
    generator.generate(*ast_);
    
    if (generator.hadError()) {
        return false;
    }

    if (!output_file_.empty()) {
        std::ofstream out(output_file_);
        if (out.is_open()) {
            out << generator.getIR();
            out.close();
        } else {
            std::cerr << "Error: Could not open output file " << output_file_ << std::endl;
            return false;
        }
    } else {
        std::cout << generator.getIR();
    }
    return true;
}
