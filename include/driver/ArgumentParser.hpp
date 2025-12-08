#pragma once

#include <string>
#include <vector>

struct CompilerOptions {
    std::vector<std::string> inputFiles;
    std::string outputFile;
    bool showHelp = false;
    bool showVersion = false;
    bool syntaxOnly = false;
    bool preprocessOnly = false;  // -E option
    std::vector<std::string> defines;      // -D options
    std::vector<std::string> undefines;    // -U options
    std::vector<std::string> includePaths; // -I options
};

class ArgumentParser {
public:
    ArgumentParser(int argc, char** argv);
    
    CompilerOptions parse();
    
    static void printUsage(const char* programName);
    static void printVersion();

private:
    int argc_;
    char** argv_;
    
    void parseOutputOption(int& i, CompilerOptions& opts);
    bool isOption(const std::string& arg) const;
};
