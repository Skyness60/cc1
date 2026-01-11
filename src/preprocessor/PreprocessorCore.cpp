#include <preprocessor/Preprocessor.hpp>

#include <fstream>
#include <sstream>
#include <cctype>

namespace cc1 {

Preprocessor::Preprocessor() {
    macroExpander_.reset(new pp::MacroExpander(macroTable_));
    exprEvaluator_.reset(new pp::ExpressionEvaluator(macroTable_));
    directiveParser_.reset(new pp::DirectiveParser());
    fileHandler_.reset(new pp::FileHandler());

    setupBuiltinMacros();
}

Preprocessor::~Preprocessor() = default;

void Preprocessor::addIncludePath(const std::string& path) {
    fileHandler_->addIncludePath(path);
}

void Preprocessor::defineMacro(const std::string& name, const std::string& value) {
    macroTable_.define(name, pp::MacroDefinition(name, std::vector<std::string>(), value, false, false));
}

void Preprocessor::undefineMacro(const std::string& name) {
    macroTable_.undefine(name);
}

std::string Preprocessor::preprocess(const std::string& filename) {
    std::string source = fileHandler_->readFile(filename);
    if (source.empty()) {
        error("cannot open file: " + filename);
        return "";
    }

    return preprocessString(source, filename);
}

std::string Preprocessor::preprocessString(const std::string& source, const std::string& filename) {
    std::string processed = commentRemover_.remove(source);
    processed = processSource(processed, filename);
    processed = stringConcatenator_.concatenate(processed);
    return processed;
}

std::string Preprocessor::processFile(const std::string& filename) {
    std::string source = fileHandler_->readFile(filename);
    if (source.empty()) {
        return "";
    }
    return processSource(source, filename);
}

std::string Preprocessor::processSource(const std::string& source, const std::string& filename) {
    std::string output;
    std::string* prevOutput = currentOutput_;
    currentOutput_ = &output;

    std::string prevFile = currentFile_;
    int prevLine = currentLine_;

    currentFile_ = filename;
    currentLine_ = 1;
    fileHandler_->setCurrentFile(filename);

    macroTable_.define("__FILE__", pp::MacroDefinition("__FILE__", std::vector<std::string>(), "\"" + filename + "\"", false, false));

    std::istringstream stream(source);
    std::string line;

    while (std::getline(stream, line)) {
        while (!line.empty() && line.back() == '\\') {
            line.pop_back();
            std::string nextLine;
            if (std::getline(stream, nextLine)) {
                line += nextLine;
                currentLine_++;
            }
        }

        macroTable_.define("__LINE__", pp::MacroDefinition("__LINE__", std::vector<std::string>(), std::to_string(currentLine_), false, false));

        std::string processed = processLine(line);
        if (!processed.empty() || isActive()) {
            output += processed;
            output += "\n";
        }

        currentLine_++;
    }

    currentFile_ = prevFile;
    currentLine_ = prevLine;

    if (!prevFile.empty()) {
        fileHandler_->setCurrentFile(prevFile);
        macroTable_.define("__FILE__", pp::MacroDefinition("__FILE__", std::vector<std::string>(), "\"" + prevFile + "\"", false, false));
    }

    currentOutput_ = prevOutput;
    return output;
}

std::string Preprocessor::processLine(const std::string& line) {
    size_t start = 0;
    while (start < line.size() && (line[start] == ' ' || line[start] == '\t')) {
        start++;
    }

    if (start < line.size() && line[start] == '#') {
        std::unique_ptr<pp::Directive> directive = directiveParser_->parse(line);
        if (directive) {
            directive->accept(*this);
        }
        return "";
    }

    if (!isActive()) {
        return "";
    }

    return macroExpander_->expand(line);
}

bool Preprocessor::isActive() const {
    if (conditionalStack_.empty()) {
        return true;
    }
    return conditionalStack_.top().active;
}

} // namespace cc1
