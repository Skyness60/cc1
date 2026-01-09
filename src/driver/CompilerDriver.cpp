#include <driver/CompilerDriver.hpp>
#include <parser/ast/AST.hpp>
#include <parser/Parser.hpp>
#include <parser/ParseError.hpp>
#include <semantics/SymbolTable.hpp>
#include <semantics/SemanticAnalyzer.hpp>
#include <codegen/IRGenerator.hpp>
#include <preprocessor/Preprocessor.hpp>
#include <lexer/Lexer.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

using cc1::DebugLogger;

static bool endsWith(const std::string& s, const std::string& suffix)
{
    if (s.size() < suffix.size())
        return false;
    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static bool readWholeFile(const std::string& filename, std::string& out)
{
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open input file " << filename << std::endl;
        return false;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    out = ss.str();
    return true;
}

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
    
    // Enable debug logging if requested
    if (debug_mode_) {
        DebugLogger::instance().setEnabled(true);
        DebugLogger::instance().setOutputFile("cc1_debug.log");
    }
}

CompilerDriver::~CompilerDriver() = default;

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

bool CompilerDriver::runPreprocessing()
{
    if (input_files_.empty()) {
        std::cerr << "Error: No input files provided." << std::endl;
        return false;
    }

    cc1::Preprocessor preprocessor;
    
    // Add include paths
    for (const auto& path : include_paths_) {
        preprocessor.addIncludePath(path);
    }
    
    // Process -D options
    for (const auto& def : defines_) {
        size_t eq = def.find('=');
        if (eq != std::string::npos) {
            preprocessor.defineMacro(def.substr(0, eq), def.substr(eq + 1));
        } else {
            preprocessor.defineMacro(def, "1");
        }
    }
    
    // Process -U options
    for (const auto& undef : undefines_) {
        preprocessor.undefineMacro(undef);
    }

    // Process all input files
    source_.clear();
    for (const auto& filename : input_files_) {
        std::string fileSource;

        // POSIX c17: a .i operand is already the output of preprocessing (-E).
        // It shall not be preprocessed again when compiled.
        if (endsWith(filename, ".i")) {
            if (!readWholeFile(filename, fileSource))
                return false;
            source_ += fileSource;
            continue;
        }

        fileSource = preprocessor.preprocess(filename);
        
        if (preprocessor.hadError()) {
            return false;
        }
        
        source_ += fileSource;
    }
    
    // If -E flag, output preprocessed source
    if (preprocess_only_) {
        if (!output_file_.empty()) {
            std::ofstream out(output_file_);
            if (out.is_open()) {
                out << source_;
                out.close();
            } else {
                std::cerr << "Error: Could not open output file " << output_file_ << std::endl;
                return false;
            }
        } else {
            std::cout << source_;
        }
    }
    
    return true;
}

bool CompilerDriver::runLexing()
{
    if (input_files_.empty()) {
        std::cerr << "Error: No input files provided." << std::endl;
        return false;
    }

    std::string filename = input_files_[0];
    
    // source_ is already preprocessed
    Lexer lexer(source_, filename);
    tokens_ = lexer.tokenize();

    if (syntax_only_) {
        for (const auto& token : tokens_) {
            std::cout << token << std::endl;
        }
    }

    return true;
}

bool CompilerDriver::runParsing()
{
    if (tokens_.empty()) {
        return true;  // Nothing to parse
    }
    
    std::string filename = input_files_.empty() ? "<input>" : input_files_[0];
    
    try {
        cc1::Parser parser(tokens_, filename, source_);
        ast_ = parser.parse();
        
        // Check if any errors were encountered during parsing
        if (parser.hadError()) {
            return false;
        }
        return true;
    } catch (const ParseError& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }
}

bool CompilerDriver::runSemantics()
{
    if (!ast_) return true;
    
    std::string filename = input_files_.empty() ? "<input>" : input_files_[0];
    cc1::SemanticAnalyzer analyzer(filename, source_, is64bit_);
    analyzer.analyze(*ast_);
    
    return !analyzer.hadError();
}

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