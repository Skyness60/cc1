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


// EN: Orchestrates preprocessing, lexing, parsing, semantics, and codegen.
// FR: Orchestre pretraitement, lexing, parsing, semantique et codegen.
class CompilerDriver
{
    private:
        std::vector<std::string> input_files_;
        std::string output_file_;
        std::string source_;  
        std::string display_source_;
        bool syntax_only_;
        bool preprocess_only_;
        bool debug_mode_;
        bool debug_info_;
        bool is64bit_;
        std::vector<std::string> defines_;
        std::vector<std::string> undefines_;
        std::vector<std::string> include_paths_;
        std::unique_ptr<AST::TranslationUnit> ast_;
        std::unique_ptr<SymbolTable> symbols_;
        std::vector<Token> tokens_;

    public:
        // EN: Constructs the driver from parsed compiler options.
        // FR: Construit le driver depuis les options du compilateur.
        CompilerDriver(const CompilerOptions& opts);
        // EN: Default destructor.
        // FR: Destructeur par defaut.
        ~CompilerDriver();

        // EN: Runs the compilation pipeline with phase controls.
        // FR: Execute le pipeline de compilation avec controle des phases.
        bool compile();

        // EN: Returns the source line buffer to display in diagnostics (prefers original if set).
        // FR: Renvoie la source a afficher pour les diagnostics (prefere l originale si definie).
        const std::string& diagnosticSource() const { return display_source_.empty() ? source_ : display_source_; }

        // EN: Override the source used for diagnostics (e.g., preprocessed vs original).
        // FR: Surcharge la source utilisee pour les diagnostics (ex: pretraitee vs originale).
        void setDisplaySource(const std::string& src) { display_source_ = src; }

    private:
        // EN: Runs preprocessing (includes, macros, -E).
        // FR: Lance le pretraitement (includes, macros, -E).
        bool runPreprocessing();
        // EN: Runs lexing and optional token dump.
        // FR: Lance le lexing et dump de tokens si demande.
        bool runLexing();
        // EN: Runs parsing and builds the AST.
        // FR: Lance le parsing et construit l AST.
        bool runParsing();
        // EN: Runs semantic analysis.
        // FR: Lance l analyse semantique.
        bool runSemantics();
        // EN: Runs LLVM IR generation.
        // FR: Lance la generation d IR LLVM.
        bool runCodeGen();
};
