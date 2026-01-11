#include <driver/CompilerDriver.hpp>

#include <preprocessor/Preprocessor.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

static bool endsWith(const std::string& s, const std::string& suffix)
{
    if (s.size() < suffix.size())
        return false;
    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

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

bool CompilerDriver::runPreprocessing()
{
    if (input_files_.empty()) {
        std::cerr << "Error: No input files provided." << std::endl;
        return false;
    }

    cc1::Preprocessor preprocessor;

    // Add include paths
    for (const auto& path : include_paths_) {
        preprocessor.addIncludePath(path);
    }

    // Process -D options
    for (const auto& def : defines_) {
        size_t eq = def.find('=');
        if (eq != std::string::npos) {
            preprocessor.defineMacro(def.substr(0, eq), def.substr(eq + 1));
        } else {
            preprocessor.defineMacro(def, "1");
        }
    }

    // Process -U options
    for (const auto& undef : undefines_) {
        preprocessor.undefineMacro(undef);
    }

    // Process all input files
    source_.clear();
    for (const auto& filename : input_files_) {
        std::string fileSource;

        // POSIX c17: a .i operand is already the output of preprocessing (-E).
        // It shall not be preprocessed again when compiled.
        if (endsWith(filename, ".i")) {
            if (!readWholeFile(filename, fileSource))
                return false;
            source_ += fileSource;
            continue;
        }

        fileSource = preprocessor.preprocess(filename);

        if (preprocessor.hadError()) {
            return false;
        }

        source_ += fileSource;
    }

    // If -E flag, output preprocessed source
    if (preprocess_only_) {
        if (!output_file_.empty()) {
            std::ofstream out(output_file_);
            if (out.is_open()) {
                out << source_;
                out.close();
            } else {
                std::cerr << "Error: Could not open output file " << output_file_ << std::endl;
                return false;
            }
        } else {
            std::cout << source_;
        }
    }

    return true;
}
