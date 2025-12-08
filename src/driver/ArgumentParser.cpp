#include <driver/ArgumentParser.hpp>
#include <utils/color.hpp>
#include <stdexcept>
#include <iostream>

ArgumentParser::ArgumentParser(int argc, char** argv)
    : argc_(argc), argv_(argv) {}

CompilerOptions ArgumentParser::parse() {
    CompilerOptions opts;
    
    if (argc_ == 1) {
        std::cout << utils::color_text("cc1: ", GREEN)
        << utils::color_text("error: ", RED)
        << utils::color_text("no input file", WHITE);
        throw std::invalid_argument("");
    }
    
    for (int i = 1; i < argc_; ++i) {
        std::string arg = argv_[i];
        
        if (arg == "--help" || arg == "-h") {
            opts.showHelp = true;
            return opts;
        }
        
        if (arg == "--version" || arg == "-v") {
            opts.showVersion = true;
            return opts;
        }
        
        if (arg == "-fsyntax-only") {
            opts.syntaxOnly = true;
            continue;
        }
        
        if (arg == "-E") {
            opts.preprocessOnly = true;
            continue;
        }
        
        // -D macro definition
        if (arg.substr(0, 2) == "-D") {
            if (arg.length() > 2) {
                opts.defines.push_back(arg.substr(2));
            } else if (i + 1 < argc_) {
                opts.defines.push_back(argv_[++i]);
            }
            continue;
        }
        
        // -U macro undefinition
        if (arg.substr(0, 2) == "-U") {
            if (arg.length() > 2) {
                opts.undefines.push_back(arg.substr(2));
            } else if (i + 1 < argc_) {
                opts.undefines.push_back(argv_[++i]);
            }
            continue;
        }
        
        // -I include path
        if (arg.substr(0, 2) == "-I") {
            if (arg.length() > 2) {
                opts.includePaths.push_back(arg.substr(2));
            } else if (i + 1 < argc_) {
                opts.includePaths.push_back(argv_[++i]);
            }
            continue;
        }
        
        if (arg == "-o") {
            parseOutputOption(i, opts);
        } else if (arg.substr(0, 2) == "-o") {
            opts.outputFile = arg.substr(2);
        } else if (isOption(arg)) {
            throw std::invalid_argument("unknown option: " + arg);
        } else {
            opts.inputFiles.emplace_back(arg);
        }
    }
    
    if (opts.inputFiles.empty()) {
        throw std::invalid_argument("no input file");
    }
    
    return opts;
}

void ArgumentParser::parseOutputOption(int& i, CompilerOptions& opts) {
    if (i + 1 >= argc_) {
        throw std::invalid_argument("missing filename after '-o'");
    }
    opts.outputFile = argv_[++i];
}

bool ArgumentParser::isOption(const std::string& arg) const {
    return !arg.empty() && arg[0] == '-';
}

void ArgumentParser::printUsage(const char* programName) {
    std::cout << WHITE "NAME" RESET "\n"
              << "       " << programName << " - C89 to LLVM IR compiler\n"
              << "\n"
              << WHITE "SYNOPSIS" RESET "\n"
              << "       " WHITE << programName << RESET " [" CYAN "OPTIONS" RESET "] " GREEN "<input-files>" RESET "...\n"
              << "\n"
              << WHITE "DESCRIPTION" RESET "\n"
              << "       Compile C89/ANSI C source files to LLVM IR.\n"
              << "\n"
              << WHITE "OPTIONS" RESET "\n"
              << "       " CYAN "-o" RESET " " GREEN "<file>" RESET "\n"
              << "              Write output to " GREEN "<file>" RESET ".\n"
              << "\n"
              << "       " CYAN "-E" RESET "\n"
              << "              Preprocess only; do not compile.\n"
              << "\n"
              << "       " CYAN "-D" RESET " " GREEN "<macro>[=value]" RESET "\n"
              << "              Define a preprocessor macro.\n"
              << "\n"
              << "       " CYAN "-U" RESET " " GREEN "<macro>" RESET "\n"
              << "              Undefine a preprocessor macro.\n"
              << "\n"
              << "       " CYAN "-I" RESET " " GREEN "<dir>" RESET "\n"
              << "              Add directory to include search path.\n"
              << "\n"
              << "       " CYAN "-h" RESET ", " CYAN "--help" RESET "\n"
              << "              Display this help message and exit.\n"
              << "\n"
              << "       " CYAN "-v" RESET ", " CYAN "--version" RESET "\n"
              << "              Display version information and exit.\n"
              << "\n"
              << "       " CYAN "-fsyntax-only" RESET "\n"
              << "              Only run the lexer and parser, then stop.\n"
              << "\n"
              << WHITE "AUTHOR" RESET "\n"
              << "       Written by " MAGENTA "Sperron | Skyness" RESET ".\n";
}

void ArgumentParser::printVersion() {
    std::cout << WHITE "cc1" RESET " version " GREEN "0.1.0" RESET "\n"
              << "Target: " CYAN "i386-linux-gnu" RESET "\n"
              << "C standard: " CYAN "C89/ANSI C" RESET "\n"
              << "By " MAGENTA "Sperron | Skyness" RESET "\n";
}
