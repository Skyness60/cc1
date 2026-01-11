#include <preprocessor/Preprocessor.hpp>

#include <iostream>

namespace cc1 {

void Preprocessor::visit(pp::ErrorDirective& dir) {
    if (!isActive()) return;

    error("#error " + dir.message);
}

void Preprocessor::visit(pp::PragmaDirective& dir) {
    if (!isActive()) return;

    if (dir.content == "once") {
        fileHandler_->markIncludedOnce(currentFile_);
    }
}

void Preprocessor::visit(pp::LineDirective& dir) {
    if (!isActive()) return;

    currentLine_ = dir.lineNumber;
    if (!dir.filename.empty()) {
        currentFile_ = dir.filename;
        fileHandler_->setCurrentFile(dir.filename);
        macroTable_.define("__FILE__", pp::MacroDefinition("__FILE__", std::vector<std::string>(), "\"" + dir.filename + "\"", false, false));
    }
}

void Preprocessor::error(const std::string& message) {
    hadError_ = true;
    std::cerr << currentFile_ << ":" << currentLine_ << ": error: " << message << "\n";
}

void Preprocessor::warning(const std::string& message) {
    std::cerr << currentFile_ << ":" << currentLine_ << ": warning: " << message << "\n";
}

} // namespace cc1
