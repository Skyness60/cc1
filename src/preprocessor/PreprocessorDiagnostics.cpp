#include <preprocessor/Preprocessor.hpp>

#include <iostream>

namespace cc1 {

// EN: Emits an #error diagnostic when the current branch is active.
// FR: Emet un diagnostic #error quand la branche courante est active.
void Preprocessor::visit(pp::ErrorDirective& dir) {
    if (!isActive()) return;

    error("#error " + dir.message);
}

// EN: Handles #pragma; currently supports "once" include guarding.
// FR: Traite #pragma ; supporte actuellement "once".
void Preprocessor::visit(pp::PragmaDirective& dir) {
    if (!isActive()) return;

    if (dir.content == "once") {
        fileHandler_->markIncludedOnce(currentFile_);
    }
}

// EN: Applies #line remapping of line number and optional filename.
// FR: Applique #line pour remapper le numero de ligne et le nom de fichier.
void Preprocessor::visit(pp::LineDirective& dir) {
    if (!isActive()) return;

    currentLine_ = dir.lineNumber;
    if (!dir.filename.empty()) {
        currentFile_ = dir.filename;
        fileHandler_->setCurrentFile(dir.filename);
        macroTable_.define("__FILE__", pp::MacroDefinition("__FILE__", std::vector<std::string>(), "\"" + dir.filename + "\"", false, false));
    }
}

// EN: Records an error and prints it with the current logical location.
// FR: Enregistre une erreur et l affiche avec la position logique courante.
void Preprocessor::error(const std::string& message) {
    hadError_ = true;
    std::cerr << currentFile_ << ":" << currentLine_ << ": error: " << message << "\n";
}

// EN: Prints a warning with the current logical location.
// FR: Affiche un avertissement avec la position logique courante.
void Preprocessor::warning(const std::string& message) {
    std::cerr << currentFile_ << ":" << currentLine_ << ": warning: " << message << "\n";
}

// TODO(cc1) EN: Route diagnostics through a shared Diagnostic system instead of std::cerr.
// FR: Router les diagnostics via un systeme Diagnostic commun plutot que std::cerr.
} 
