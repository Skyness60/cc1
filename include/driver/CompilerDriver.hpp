#pragma once

#include <memory>
#include <string>
#include <vector>
#include <lexer/Token.hpp>
#include <driver/ArgumentParser.hpp>

namespace AST {
    class TranslationUnit;
}

class SymbolTable;

class CompilerDriver
{
    private:
        std::vector<std::string> input_files_;
        std::string output_file_;
        std::string source_;  // Source code for error reporting
        bool syntax_only_;
        bool preprocess_only_;
        std::vector<std::string> defines_;
        std::vector<std::string> undefines_;
        std::vector<std::string> include_paths_;
        std::unique_ptr<AST::TranslationUnit> ast_;
        std::unique_ptr<SymbolTable> symbols_;
        std::vector<Token> tokens_;

    public:
        CompilerDriver(const CompilerOptions& opts);
        ~CompilerDriver();

        bool compile();

    private:
        bool runPreprocessing();
        bool runLexing();
        bool runParsing();
        bool runSemantics();
        bool runCodeGen();
};