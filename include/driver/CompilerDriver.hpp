#pragma once

#include <memory>
#include <string>
#include <vector>
#include <lexer/Token.hpp>

namespace AST {
    class ProgramNode;
}

class SymbolTable;

class CompilerDriver
{
    private:
        std::vector<std::string> input_files_;
        std::string output_file_;
        std::unique_ptr<AST::ProgramNode> ast_;
        std::unique_ptr<SymbolTable> symbols_;
        std::vector<Token> tokens_;

    public:
        CompilerDriver(std::vector<std::string>& input, const std::string& output = "");
        ~CompilerDriver();

        bool compile();

    private:
        bool runLexing();
        bool runParsing();
        bool runSemantics();
        bool runCodeGen();
};