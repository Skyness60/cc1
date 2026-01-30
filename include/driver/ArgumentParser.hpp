#pragma once

#include <string>
#include <vector>




// EN: Captures parsed compiler options and flags.
// FR: Regroupe les options et flags du compilateur.
struct CompilerOptions {
    std::vector<std::string> inputFiles;
    std::string outputFile;
    bool showHelp = false;
    bool showVersion = false;
    bool syntaxOnly = false;
    bool preprocessOnly = false;  
    bool debugMode = false;        
    bool debugInfo = false;        
    bool is64bit = false;          
    std::vector<std::string> defines;      
    std::vector<std::string> undefines;    
    std::vector<std::string> includePaths; 
    std::vector<std::string> ignoredOptions; 
};


// EN: Parses command-line arguments into CompilerOptions.
// FR: Parse la ligne de commande vers CompilerOptions.
class ArgumentParser {
public:
    // EN: Constructs the parser with argc/argv.
    // FR: Construit le parseur avec argc/argv.
    ArgumentParser(int argc, char** argv);
    
    // EN: Parses all arguments and returns options.
    // FR: Parse tous les arguments et renvoie les options.
    CompilerOptions parse();
    
    // EN: Prints usage information to stdout.
    // FR: Affiche l aide/usage sur stdout.
    static void printUsage(const char* programName);
    // EN: Prints version information to stdout.
    // FR: Affiche la version sur stdout.
    static void printVersion();

private:
    int argc_;
    char** argv_;
    
    // EN: Parses the -o option requiring a filename.
    // FR: Parse l option -o avec un nom de fichier requis.
    void parseOutputOption(int& i, CompilerOptions& opts);
    // EN: Checks whether a token is an option switch.
    // FR: Verifie si un token est une option.
    bool isOption(const std::string& arg) const;
};
