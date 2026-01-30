#include <semantics/SemanticAnalyzer.hpp>
#include <utils/Diagnostic.hpp>
#include <sstream>

namespace cc1 {





// EN: Initializes semantic analysis context and global scope.
// FR: Initialise le contexte semantique et le scope global.
SemanticAnalyzer::SemanticAnalyzer(const std::string& filename, const std::string& source, bool is64bit)
    : filename_(filename), source_(source), is64bit_(is64bit)
{
    
    enterScope(false);
}

// EN: Entry point to analyze the translation unit.
// FR: Point d entree pour analyser l unite de traduction.
void SemanticAnalyzer::analyze(AST::TranslationUnit& unit) {
    unit.accept(*this);
}





// EN: Retrieves a specific source line for diagnostics.
// FR: Recupere une ligne source pour diagnostics.
std::string SemanticAnalyzer::getSourceLine(int line) const {
    std::istringstream stream(source_);
    std::string currentLine;
    int currentNum = 0;
    while (std::getline(stream, currentLine)) {
        if (++currentNum == line) return currentLine;
    }
    return "";
}

// EN: Reports an error and marks analysis as failed.
// FR: Signale une erreur et marque l analyse comme echouee.
void SemanticAnalyzer::error(int line, int column, const std::string& message) {
    hadError_ = true;
    SourceLocation loc(filename_, line, column);
    DiagnosticPrinter::print(DiagnosticKind::Error, loc, message, getSourceLine(line));
}

// EN: Reports a warning diagnostic.
// FR: Signale un avertissement.
void SemanticAnalyzer::warning(int line, int column, const std::string& message) {
    SourceLocation loc(filename_, line, column);
    DiagnosticPrinter::print(DiagnosticKind::Warning, loc, message, getSourceLine(line));
}





// EN: Pushes a new scope and marks function scope if needed.
// FR: Empile un nouveau scope et marque function scope si besoin.
void SemanticAnalyzer::enterScope(bool isFunction) {
    auto scope = std::unique_ptr<Scope>(new Scope());
    scope->parent = currentScope_;
    scope->isFunction = isFunction;
    currentScope_ = scope.get();
    scopes_.push_back(std::move(scope));
}

// EN: Pops the current scope, returning to the parent.
// FR: Depile le scope courant et revient au parent.
void SemanticAnalyzer::exitScope() {
    if (currentScope_ && currentScope_->parent) {
        currentScope_ = currentScope_->parent;
    }
}

} 

// TODO(cc1) EN: Track scope depth for better diagnostic context.
// FR: Suivre la profondeur de scope pour meilleur contexte diagnostic.
