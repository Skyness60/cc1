#include <driver/CompilerDriver.hpp>
#include <parser/ast/AST.hpp>
#include <parser/Parser.hpp>
#include <parser/ParseError.hpp>
#include <semantics/SymbolTable.hpp>
#include <semantics/SemanticAnalyzer.hpp>
#include <codegen/IRGenerator.hpp>
#include <lexer/Lexer.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

CompilerDriver::CompilerDriver(std::vector<std::string>& input, const std::string& output, bool syntaxOnly)
    : input_files_(input),
      output_file_(output),
      syntax_only_(syntaxOnly)
{
    symbols_.reset(new SymbolTable());
}

CompilerDriver::~CompilerDriver() = default;

bool CompilerDriver::compile()
{
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

bool CompilerDriver::runLexing()
{
    if (input_files_.empty()) {
        std::cerr << "Error: No input files provided." << std::endl;
        return false;
    }

    // For now, we only handle the first file as per standard cc1 behavior (usually one TU)
    std::string filename = input_files_[0];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    source_ = buffer.str();

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
    cc1::SemanticAnalyzer analyzer(filename, source_);
    analyzer.analyze(*ast_);
    
    return !analyzer.hadError();
}

bool CompilerDriver::runCodeGen()
{
    // Code generation not yet implemented with new AST
    // IRGenerator generator;
    // if (ast_) {
    //     ast_->accept(generator);
    // }

    // if (!output_file_.empty()) {
    //     std::ofstream out(output_file_);
    //     if (out.is_open()) {
    //         out << generator.getIR();
    //         out.close();
    //     } else {
    //         std::cerr << "Error: Could not open output file " << output_file_ << std::endl;
    //         return false;
    //     }
    // } else {
    //     std::cout << generator.getIR();
    // }
    return true;
}