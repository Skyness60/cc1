#include <preprocessor/Preprocessor.hpp>

namespace cc1 {

void Preprocessor::visit(pp::DefineDirective& dir) {
    if (!isActive()) return;

    macroTable_.define(dir.name, pp::MacroDefinition(dir.name, dir.parameters, dir.body, dir.isFunctionLike, dir.isVariadic));
}

void Preprocessor::visit(pp::UndefDirective& dir) {
    if (!isActive()) return;

    macroTable_.undefine(dir.name);
}

void Preprocessor::visit(pp::IncludeDirective& dir) {
    if (!isActive()) return;

    std::string fullPath = fileHandler_->findInclude(dir.filename, dir.isSystemInclude);
    if (fullPath.empty()) {
        error("cannot find include file: " + dir.filename);
        return;
    }

    std::string content = fileHandler_->readFile(fullPath);
    if (content.empty()) {
        error("cannot read include file: " + fullPath);
        return;
    }

    content = commentRemover_.remove(content);
    std::string processed = processSource(content, fullPath);

    if (currentOutput_) {
        *currentOutput_ += processed;
    }
}

} // namespace cc1
