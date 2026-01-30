#include <preprocessor/Preprocessor.hpp>

namespace cc1 {

// EN: Applies a #define when the current conditional state is active.
// FR: Applique un #define si la condition courante est active.
void Preprocessor::visit(pp::DefineDirective& dir) {
    if (!isActive()) return;

    macroTable_.define(dir.name, pp::MacroDefinition(dir.name, dir.parameters, dir.body, dir.isFunctionLike, dir.isVariadic));
}

// EN: Applies a #undef when active to remove macro definitions.
// FR: Applique un #undef en contexte actif pour supprimer la macro.
void Preprocessor::visit(pp::UndefDirective& dir) {
    if (!isActive()) return;

    macroTable_.undefine(dir.name);
}

// EN: Handles #include by resolving the file, preprocessing it, and splicing
// the result into the current output. FR: Traite #include en resolvant le
// fichier, en le pretraitant, puis en l inserant dans la sortie courante.
void Preprocessor::visit(pp::IncludeDirective& dir) {
    if (!isActive()) return;

    std::string fullPath = fileHandler_->findInclude(dir.filename, dir.isSystemInclude);
    if (fullPath.empty()) {
        error("cannot find include file: " + dir.filename);
        return;
    }

    std::string content;
    bool ok = fileHandler_->readFile(fullPath, content);
    if (!ok) {
        error("cannot read include file: " + fullPath);
        return;
    }
    if (content.empty()) {
        return;
    }

    content = commentRemover_.remove(content);
    std::string processed = processSource(content, fullPath);

    if (currentOutput_) {
        *currentOutput_ += processed;
    }
}

// TODO(cc1) EN: Track include stack depth to detect recursive includes earlier.
// FR: Suivre la profondeur d include pour detecter les recursions plus tot.
} 
