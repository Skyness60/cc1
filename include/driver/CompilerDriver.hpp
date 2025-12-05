#pragma once

#include <memory>
#include <string>
#include <vector>
#include <lexer/Token.hpp>

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
        std::unique_ptr<AST::TranslationUnit> ast_;
        std::unique_ptr<SymbolTable> symbols_;
        std::vector<Token> tokens_;

    public:
        CompilerDriver(std::vector<std::string>& input, const std::string& output = "", bool syntaxOnly = false);
        ~CompilerDriver();

        bool compile();

    private:
        bool runLexing();
        bool runParsing();
        bool runSemantics();
        bool runCodeGen();
};