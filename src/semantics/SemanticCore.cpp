#include <semantics/SemanticAnalyzer.hpp>
#include <utils/Diagnostic.hpp>
#include <sstream>

namespace cc1 {

// ============================================================================
// SemanticAnalyzer Constructor & Destructor
// ============================================================================

SemanticAnalyzer::SemanticAnalyzer(const std::string& filename, const std::string& source)
    : filename_(filename), source_(source) 
{
    // Create global scope
    enterScope(false);
}

void SemanticAnalyzer::analyze(AST::TranslationUnit& unit) {
    unit.accept(*this);
}

// ============================================================================
// Error Reporting
// ============================================================================

std::string SemanticAnalyzer::getSourceLine(int line) const {
    std::istringstream stream(source_);
    std::string currentLine;
    int currentNum = 0;
    while (std::getline(stream, currentLine)) {
        if (++currentNum == line) return currentLine;
    }
    return "";
}

void SemanticAnalyzer::error(int line, int column, const std::string& message) {
    hadError_ = true;
    SourceLocation loc(filename_, line, column);
    DiagnosticPrinter::print(DiagnosticKind::Error, loc, message, getSourceLine(line));
}

void SemanticAnalyzer::warning(int line, int column, const std::string& message) {
    SourceLocation loc(filename_, line, column);
    DiagnosticPrinter::print(DiagnosticKind::Warning, loc, message, getSourceLine(line));
}

// ============================================================================
// Scope Management
// ============================================================================

void SemanticAnalyzer::enterScope(bool isFunction) {
    auto scope = std::unique_ptr<Scope>(new Scope());
    scope->parent = currentScope_;
    scope->isFunction = isFunction;
    currentScope_ = scope.get();
    scopes_.push_back(std::move(scope));
}

void SemanticAnalyzer::exitScope() {
    if (currentScope_ && currentScope_->parent) {
        currentScope_ = currentScope_->parent;
    }
}

} // namespace cc1
