#pragma once

#include <string>
#include <vector>
#include <stack>
#include <memory>

#include <preprocessor/PPDirective.hpp>
#include <preprocessor/PPMacro.hpp>
#include <preprocessor/PPExprEval.hpp>
#include <preprocessor/PPParser.hpp>
#include <preprocessor/PPFileHandler.hpp>
#include <preprocessor/PPUtils.hpp>

namespace cc1 {

// ============================================================================
// Conditional State for #if/#else/#endif tracking
// ============================================================================

struct ConditionalState {
    bool active;      // Is this branch being processed?
    bool seenTrue;    // Have we seen a true branch yet?
    bool isElse;      // Are we in #else?
};

// ============================================================================
// Preprocessor - Main class implementing DirectiveVisitor
// ============================================================================

class Preprocessor : public pp::DirectiveVisitor {
public:
    Preprocessor();
    ~Preprocessor();
    
    // Main entry point: preprocess a file
    std::string preprocess(const std::string& filename);
    
    // Preprocess from string (for testing)
    std::string preprocessString(const std::string& source, const std::string& filename = "<string>");
    
    // Add include search paths
    void addIncludePath(const std::string& path);
    
    // Predefined macros (-D option)
    void defineMacro(const std::string& name, const std::string& value = "1");
    
    // Undefine macro (-U option)
    void undefineMacro(const std::string& name);
    
    // Check for errors
    bool hadError() const { return hadError_; }
    
    // DirectiveVisitor implementation
    void visit(pp::DefineDirective& dir) override;
    void visit(pp::UndefDirective& dir) override;
    void visit(pp::IncludeDirective& dir) override;
    void visit(pp::IfDirective& dir) override;
    void visit(pp::IfdefDirective& dir) override;
    void visit(pp::ElifDirective& dir) override;
    void visit(pp::ElseDirective& dir) override;
    void visit(pp::EndifDirective& dir) override;
    void visit(pp::ErrorDirective& dir) override;
    void visit(pp::PragmaDirective& dir) override;
    void visit(pp::LineDirective& dir) override;
    
private:
    // Components (SRP)
    pp::MacroTable macroTable_;
    std::unique_ptr<pp::MacroExpander> macroExpander_;
    std::unique_ptr<pp::ExpressionEvaluator> exprEvaluator_;
    std::unique_ptr<pp::DirectiveParser> directiveParser_;
    std::unique_ptr<pp::FileHandler> fileHandler_;
    pp::CommentRemover commentRemover_;
    pp::StringConcatenator stringConcatenator_;
    
    // State
    std::string currentFile_;
    int currentLine_ = 1;
    bool hadError_ = false;
    std::stack<ConditionalState> conditionalStack_;
    std::string* currentOutput_ = nullptr;  // Output buffer for include processing
    
    // Core processing
    std::string processFile(const std::string& filename);
    std::string processSource(const std::string& source, const std::string& filename);
    std::string processLine(const std::string& line);
    
    // Check if currently in an active preprocessing branch
    bool isActive() const;
    
    // Setup built-in macros
    void setupBuiltinMacros();
    
    // Error reporting
    void error(const std::string& message);
    void warning(const std::string& message);
};

} // namespace cc1
