#include <driver/CompilerDriver.hpp>

#include <preprocessor/Preprocessor.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

// EN: Checks if a string ends with a given suffix.
// FR: Verifie si une chaine se termine par un suffixe.
static bool endsWith(const std::string& s, const std::string& suffix)
{
    if (s.size() < suffix.size())
        return false;
    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// EN: Reads an entire file into a string.
// FR: Lit un fichier entier dans une chaine.
static bool readWholeFile(const std::string& filename, std::string& out)
{
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open input file " << filename << std::endl;
        return false;
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    out = ss.str();
    return true;
}

// EN: Runs preprocessing for all input files and handles -E output.
// FR: Lance le pretraitement pour tous les fichiers et gere -E.
bool CompilerDriver::runPreprocessing()
{
    if (input_files_.empty()) {
        std::cerr << "Error: No input files provided." << std::endl;
        return false;
    }

    cc1::Preprocessor preprocessor;

    
    for (const auto& path : include_paths_) {
        preprocessor.addIncludePath(path);
    }

    
    for (const auto& def : defines_) {
        size_t eq = def.find('=');
        if (eq != std::string::npos) {
            preprocessor.defineMacro(def.substr(0, eq), def.substr(eq + 1));
        } else {
            preprocessor.defineMacro(def, "1");
        }
    }

    
    for (const auto& undef : undefines_) {
        preprocessor.undefineMacro(undef);
    }

    
    source_.clear();
    for (const auto& filename : input_files_) {
        std::string fileSource;

        
        
        if (endsWith(filename, ".i")) {
            if (!readWholeFile(filename, fileSource))
                return false;
            fileSource = preprocessor.preprocessString(fileSource, filename);
            if (preprocessor.hadError()) {
                return false;
            }
            source_ += fileSource;
            continue;
        }

        fileSource = preprocessor.preprocess(filename);

        if (preprocessor.hadError()) {
            return false;
        }

        source_ += fileSource;
    }

    
    if (preprocess_only_) {
        // Treat '-' as stdout. If output_file_ is non-empty and not "-", write to file;
        // otherwise print to stdout.
        if (!output_file_.empty() && output_file_ != "-") {
            std::ofstream out(output_file_);
            if (out.is_open()) {
                out << source_;
                out.close();
            } else {
                std::cerr << "Error: Could not open output file " << output_file_ << std::endl;
                return false;
            }
        } else {
            // empty or '-' -> stdout
            std::cout << source_;
        }
    }


    return true;
}
