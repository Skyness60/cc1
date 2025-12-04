#include <driver/CompilerDriver.hpp>
#include <parser/AST.hpp>
#include <semantics/SymbolTable.hpp>
#include <semantics/SemanticAnalyzer.hpp>
#include <codegen/IRGenerator.hpp>
#include <lexer/Lexer.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

CompilerDriver::CompilerDriver(std::vector<std::string>& input, const std::string& output)
    : input_files_(input),
      output_file_(output)
{
    // symbols_ and ast_ initialization moved to runParsing/runSemantics usually, 
    // but keeping constructor init for now as per existing code structure
    // Note: symbols_ might be better placed inside SemanticAnalyzer
    symbols_ = std::make_unique<SymbolTable>();
    ast_ = std::make_unique<AST::ProgramNode>();
}

CompilerDriver::~CompilerDriver() = default;

bool CompilerDriver::compile()
{
    if (!runLexing())
        return false;
    if (!runParsing())
        return false;
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
    std::string source = buffer.str();

    Lexer lexer(source, filename);
    tokens_ = lexer.tokenize();

    // Debug: Print tokens
    for (const auto& token : tokens_) {
        std::cout << token << std::endl;
    }

    return true;
}

bool CompilerDriver::runParsing()
{
    // Implementation of parsing phase
    return true; // Placeholder
}

bool CompilerDriver::runSemantics()
{
    SemanticAnalyzer analyzer;
    if (ast_) {
        ast_->accept(analyzer);
    }
    return true;
}

bool CompilerDriver::runCodeGen()
{
    IRGenerator generator;
    if (ast_) {
        ast_->accept(generator);
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
        // If no output file specified, maybe print to stdout or do nothing?
        // For now, let's print to stdout for debugging if no output file
        std::cout << generator.getIR();
    }
    return true;
}