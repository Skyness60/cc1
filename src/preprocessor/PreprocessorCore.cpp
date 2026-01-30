#include <preprocessor/Preprocessor.hpp>

#include <fstream>
#include <sstream>
#include <cctype>

namespace cc1 {

// EN: Initializes the preprocessor helpers and builtin macros so the instance
// is ready for immediate use. FR: Initialise les aides et macros internes pour
// une utilisation immediate.
Preprocessor::Preprocessor() {
    macroExpander_.reset(new pp::MacroExpander(macroTable_));
    exprEvaluator_.reset(new pp::ExpressionEvaluator(macroTable_));
    directiveParser_.reset(new pp::DirectiveParser());
    fileHandler_.reset(new pp::FileHandler());

    setupBuiltinMacros();
}

// EN: Defaulted destructor because ownership is via smart pointers.
// FR: Destructeur par defaut car l ownership est gere par smart pointers.
Preprocessor::~Preprocessor() = default;

// EN: Forwards include path configuration to the file handler.
// FR: Transmet l ajout de chemin d include au gestionnaire de fichiers.
void Preprocessor::addIncludePath(const std::string& path) {
    fileHandler_->addIncludePath(path);
}

// EN: Defines a simple object-like macro, mirroring -D usage.
// FR: Definit une macro simple de type objet, comme -D.
void Preprocessor::defineMacro(const std::string& name, const std::string& value) {
    macroTable_.define(name, pp::MacroDefinition(name, std::vector<std::string>(), value, false, false));
}

// EN: Removes a macro definition from the table.
// FR: Supprime une definition de macro de la table.
void Preprocessor::undefineMacro(const std::string& name) {
    macroTable_.undefine(name);
}

// EN: Loads a file and preprocesses it, emitting an error on missing files.
// FR: Charge un fichier et le pretraite, en signalant une erreur si absent.
std::string Preprocessor::preprocess(const std::string& filename) {
    std::string source;
    bool ok = fileHandler_->readFile(filename, source);
    if (!ok) {
        error("cannot open file: " + filename);
        return "";
    }

    return preprocessString(source, filename);
}

// EN: Preprocesses raw text by removing comments, applying directives/macros,
// and concatenating adjacent strings. FR: Pretraite un texte en supprimant les
// commentaires, en appliquant directives/macros, puis en concatenant les chaines.
std::string Preprocessor::preprocessString(const std::string& source, const std::string& filename) {
    std::string processed = commentRemover_.remove(source);
    processed = processSource(processed, filename);
    processed = stringConcatenator_.concatenate(processed);
    return processed;
}

// EN: Internal file preprocessing helper that skips diagnostic emission.
// FR: Aide interne pour pretraiter un fichier sans emission de diagnostic.
std::string Preprocessor::processFile(const std::string& filename) {
    std::string source;
    bool ok = fileHandler_->readFile(filename, source);
    if (!ok) {
        return "";
    }
    return processSource(source, filename);
}

// EN: Core source processing loop that updates __FILE__/__LINE__ and applies
// line continuations, directives, and macro expansion. FR: Boucle principale
// qui met a jour __FILE__/__LINE__ et applique continuations, directives et macros.
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

// EN: Processes a single line, dispatching directives or expanding macros.
// FR: Traite une ligne, en dispatchant les directives ou en expandant les macros.
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

// EN: Returns whether the current conditional state allows output.
// FR: Indique si l etat conditionnel autorise la sortie.
bool Preprocessor::isActive() const {
    if (conditionalStack_.empty()) {
        return true;
    }
    return conditionalStack_.top().active;
}

} 

// TODO(cc1) EN: Replace manual line continuation handling with a shared helper
// to keep behavior consistent across file and string inputs. FR: Remplacer la
// gestion manuelle des continuations par un helper commun.
