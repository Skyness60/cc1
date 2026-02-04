#include <driver/ArgumentParser.hpp>
#include <utils/color.hpp>
#include <stdexcept>
#include <iostream>

// EN: Stores argc/argv for later parsing.
// FR: Stocke argc/argv pour parsing ulterieur.
ArgumentParser::ArgumentParser(int argc, char** argv)
    : argc_(argc), argv_(argv) {}

// EN: Parses CLI arguments into CompilerOptions, with early exits for help/version.
// FR: Parse les arguments CLI en CompilerOptions, avec sorties help/version.
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
        
        if (arg == "-debug") {
            opts.debugMode = true;
            continue;
        }

        if (arg == "-g") {
            opts.debugInfo = true;
            continue;
        }



        if (arg == "-m32") {
            opts.is64bit = false;
            continue;
        }

        if (arg == "-m64") {
            opts.is64bit = true;
            continue;
        }
        
        
        if (arg.substr(0, 2) == "-D") {
            if (arg.length() > 2) {
                opts.defines.push_back(arg.substr(2));
            } else if (i + 1 < argc_) {
                opts.defines.push_back(argv_[++i]);
            }
            continue;
        }
        
        
        if (arg.substr(0, 2) == "-U") {
            if (arg.length() > 2) {
                opts.undefines.push_back(arg.substr(2));
            } else if (i + 1 < argc_) {
                opts.undefines.push_back(argv_[++i]);
            }
            continue;
        }
        
        
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
            // Validate input file extensions:
            // - allow '-' for stdin
            // - accept C sources: .c or .C
            // - accept preprocessed sources: .i
            if (arg != "-" &&
                !(arg.size() >= 2 && (arg.substr(arg.size() - 2) == ".c" ||
                                     arg.substr(arg.size() - 2) == ".C" ||
                                     arg.substr(arg.size() - 2) == ".i"))) {
                throw std::invalid_argument("input file must have .c or .i extension: " + arg);
            }
            opts.inputFiles.emplace_back(arg);
        }
    }
    
    if (opts.inputFiles.empty()) {
        throw std::invalid_argument("no input file");
    }
    
    return opts;
}

// EN: Parses the -o option with a required filename.
// FR: Parse l option -o avec un nom de fichier requis.
void ArgumentParser::parseOutputOption(int& i, CompilerOptions& opts) {
    if (i + 1 >= argc_) {
        throw std::invalid_argument("missing filename after '-o'");
    }
    opts.outputFile = argv_[++i];
}

// EN: Returns true when an argument looks like an option.
// FR: Indique si un argument ressemble a une option.
bool ArgumentParser::isOption(const std::string& arg) const {
    // EN: '-' is not an option, it means stdin
    // FR: '-' n'est pas une option, cela signifie stdin
    if (arg == "-") return false;
    return !arg.empty() && arg[0] == '-';
}

// EN: Prints usage/help text to stdout.
// FR: Affiche l aide/usage sur stdout.
void ArgumentParser::printUsage(const char* programName) {
    std::cout << WHITE "NAME" RESET "\n"
              << "       " << programName << " - C89 to LLVM IR compiler (backend only)\n"
              << "\n"
              << WHITE "SYNOPSIS" RESET "\n"
              << "       " WHITE << programName << RESET " [" CYAN "OPTIONS" RESET "] " GREEN "<input-files>" RESET "...\n"
              << "       " WHITE << programName << RESET " [" CYAN "OPTIONS" RESET "] -       (read from stdin)\n"
              << "\n"
              << WHITE "DESCRIPTION" RESET "\n"
              << "       Compile C89/ANSI C source files to LLVM IR (Intermediate Representation).\n"
              << "       This is a compiler backend. Use the fcc wrapper for full compilation\n"
              << "       including preprocessing, assembly, linking, and output format selection.\n"
              << "\n"
              << WHITE "OPTIONS" RESET "\n"
              << "       " CYAN "-E" RESET "\n"
              << "              Preprocess only; write preprocessed source to stdout (or -o).\n"
              << "\n"
              << "       " CYAN "-o" RESET " " GREEN "<file>" RESET "\n"
              << "              Write output to " GREEN "<file>" RESET ".\n"
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
              << "       " CYAN "-g" RESET "\n"
              << "              Emit debug information (LLVM debug metadata).\n"
              << "\n"
              << "       " CYAN "-m32" RESET ", " CYAN "-m64" RESET "\n"
              << "              Target i386 (32-bit) or x86_64 (64-bit).\n"
              << "\n"
              << "       " CYAN "-h" RESET ", " CYAN "--help" RESET "\n"
              << "              Display this help message and exit.\n"
              << "\n"
              << "       " CYAN "-v" RESET ", " CYAN "--version" RESET "\n"
              << "              Display version information and exit.\n"
              << "\n"
              << WHITE "OPERANDS" RESET "\n"
              << "       One or more C source files " GREEN "<input-files>" RESET ". Use '-' to read from stdin.\n"
              << "\n"
              << WHITE "INPUT FILES" RESET "\n"
              << "       C89 sources with .c extension or preprocessed sources with .i extension.\n"
              << "\n"
              << WHITE "STDOUT/OUTPUT" RESET "\n"
              << "       Default output is LLVM IR. With -E, writes preprocessed source.\n"
              << "       Use -o to specify output file; otherwise writes to stdout.\n"
              << "\n"
              << WHITE "STDERR" RESET "\n"
              << "       Diagnostics and error messages are written to stderr.\n"
              << "\n"
              << WHITE "EXIT STATUS" RESET "\n"
              << "       0 on success; non-zero on any error.\n"
              << "\n"
              << WHITE "SEE ALSO" RESET "\n"
              << "       fcc(1) - C89 compiler driver (frontend + backend)\n"
              << "\n"
              << WHITE "AUTHOR" RESET "\n"
              << "       Written by " MAGENTA "Sperron | Skyness" RESET ".\n";
}

// EN: Prints version information to stdout.
// FR: Affiche les infos de version sur stdout.
void ArgumentParser::printVersion() {
    std::cout << WHITE "cc1" RESET " version " GREEN "0.1.0" RESET "\n"
              << "Target: " CYAN "i386-linux-gnu / x86_64-linux-gnu (select with -m32/-m64)" RESET "\n"
              << "C standard: " CYAN "C89/ANSI C" RESET "\n"
              << "By " MAGENTA "Sperron | Skyness" RESET "\n";
}
