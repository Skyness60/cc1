#include <preprocessor/Preprocessor.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>
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

void Preprocessor::setupBuiltinMacros() {
    // __DATE__
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);
    char dateBuf[32];
    std::strftime(dateBuf, sizeof(dateBuf), "\"%b %d %Y\"", tm);
    macroTable_.define("__DATE__", pp::MacroDefinition("__DATE__", std::vector<std::string>(), dateBuf, false, false));
    
    // __TIME__
    char timeBuf[32];
    std::strftime(timeBuf, sizeof(timeBuf), "\"%H:%M:%S\"", tm);
    macroTable_.define("__TIME__", pp::MacroDefinition("__TIME__", std::vector<std::string>(), timeBuf, false, false));
    
    // Standard C89 macros
    macroTable_.define("__STDC__", pp::MacroDefinition("__STDC__", std::vector<std::string>(), "1", false, false));
    macroTable_.define("__STDC_VERSION__", pp::MacroDefinition("__STDC_VERSION__", std::vector<std::string>(), "199409L", false, false));
    
    // Target specific
    macroTable_.define("__i386__", pp::MacroDefinition("__i386__", std::vector<std::string>(), "1", false, false));
    macroTable_.define("__i386", pp::MacroDefinition("__i386", std::vector<std::string>(), "1", false, false));
    macroTable_.define("i386", pp::MacroDefinition("i386", std::vector<std::string>(), "1", false, false));
    macroTable_.define("__linux__", pp::MacroDefinition("__linux__", std::vector<std::string>(), "1", false, false));
    macroTable_.define("__unix__", pp::MacroDefinition("__unix__", std::vector<std::string>(), "1", false, false));
}

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
    // Phase 1: Remove comments
    std::string processed = commentRemover_.remove(source);
    
    // Phase 2-4: Process directives and expand macros
    processed = processSource(processed, filename);
    
    // Phase 6: String literal concatenation
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
    std::string* prevOutput = currentOutput_;  // Save previous output pointer
    currentOutput_ = &output;
    
    std::string prevFile = currentFile_;
    int prevLine = currentLine_;
    
    currentFile_ = filename;
    currentLine_ = 1;
    fileHandler_->setCurrentFile(filename);
    
    // Update __FILE__ macro
    macroTable_.define("__FILE__", pp::MacroDefinition("__FILE__", std::vector<std::string>(), "\"" + filename + "\"", false, false));
    
    std::istringstream stream(source);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Handle line continuation
        while (!line.empty() && line.back() == '\\') {
            line.pop_back();
            std::string nextLine;
            if (std::getline(stream, nextLine)) {
                line += nextLine;
                currentLine_++;
            }
        }
        
        // Update __LINE__ macro
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
    
    currentOutput_ = prevOutput;  // Restore previous output pointer
    return output;
}

std::string Preprocessor::processLine(const std::string& line) {
    // Skip leading whitespace
    size_t start = 0;
    while (start < line.size() && (line[start] == ' ' || line[start] == '\t')) {
        start++;
    }
    
    // Check for directive
    if (start < line.size() && line[start] == '#') {
        std::unique_ptr<pp::Directive> directive = directiveParser_->parse(line);
        if (directive) {
            directive->accept(*this);
        }
        return "";  // Directive lines produce no output
    }
    
    // Regular line
    if (!isActive()) {
        return "";  // Skip if in inactive conditional branch
    }
    
    // Expand macros in the line
    return macroExpander_->expand(line);
}

bool Preprocessor::isActive() const {
    if (conditionalStack_.empty()) {
        return true;
    }
    return conditionalStack_.top().active;
}

// ============================================================================
// Directive Visitor Implementations
// ============================================================================

void Preprocessor::visit(pp::DefineDirective& dir) {
    if (!isActive()) return;
    
    macroTable_.define(dir.name, pp::MacroDefinition(
        dir.name, dir.parameters, dir.body, dir.isFunctionLike, dir.isVariadic
    ));
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
    
    // Remove comments from included file
    content = commentRemover_.remove(content);
    
    // Process included content
    std::string processed = processSource(content, fullPath);
    
    // Append to current output
    if (currentOutput_) {
        *currentOutput_ += processed;
    }
}

void Preprocessor::visit(pp::IfDirective& dir) {
    if (!isActive()) {
        // Already in inactive branch, push inactive state
        conditionalStack_.push(ConditionalState{false, false, false});
        return;
    }
    
    // Expand macros in condition (including defined())
    std::string condition = dir.condition;
    
    // Handle defined() operator
    std::string expanded;
    size_t i = 0;
    while (i < condition.size()) {
        // Look for "defined"
        if (i + 7 <= condition.size() && condition.substr(i, 7) == "defined") {
            size_t j = i + 7;
            // Skip whitespace
            while (j < condition.size() && std::isspace(condition[j])) j++;
            
            bool hasParen = (j < condition.size() && condition[j] == '(');
            if (hasParen) j++;
            
            // Skip whitespace
            while (j < condition.size() && std::isspace(condition[j])) j++;
            
            // Get identifier
            std::string ident;
            while (j < condition.size() && (std::isalnum(condition[j]) || condition[j] == '_')) {
                ident += condition[j++];
            }
            
            // Skip whitespace and closing paren
            while (j < condition.size() && std::isspace(condition[j])) j++;
            if (hasParen && j < condition.size() && condition[j] == ')') j++;
            
            expanded += macroTable_.isDefined(ident) ? "1" : "0";
            i = j;
        } else {
            expanded += condition[i++];
        }
    }
    
    // Expand remaining macros
    expanded = macroExpander_->expand(expanded);
    
    // Evaluate
    long long result = exprEvaluator_->evaluate(expanded);
    bool conditionTrue = (result != 0);
    
    conditionalStack_.push(ConditionalState{conditionTrue, conditionTrue, false});
}

void Preprocessor::visit(pp::IfdefDirective& dir) {
    if (!isActive()) {
        conditionalStack_.push(ConditionalState{false, false, false});
        return;
    }
    
    bool defined = macroTable_.isDefined(dir.name);
    bool conditionTrue = dir.isIfndef ? !defined : defined;
    conditionalStack_.push(ConditionalState{conditionTrue, conditionTrue, false});
}

void Preprocessor::visit(pp::ElifDirective& dir) {
    if (conditionalStack_.empty()) {
        error("#elif without #if");
        return;
    }
    
    ConditionalState& state = conditionalStack_.top();
    
    if (state.isElse) {
        error("#elif after #else");
        return;
    }
    
    if (state.seenTrue) {
        // Already saw a true branch, this one is inactive
        state.active = false;
    } else {
        // Check parent stack
        bool parentActive = true;
        if (conditionalStack_.size() > 1) {
            std::stack<ConditionalState> temp;
            temp.push(conditionalStack_.top());
            conditionalStack_.pop();
            parentActive = isActive();
            conditionalStack_.push(temp.top());
        }
        
        if (parentActive) {
            // Expand and evaluate condition
            std::string condition = dir.condition;
            
            // Handle defined() operator
            std::string expanded;
            size_t i = 0;
            while (i < condition.size()) {
                if (i + 7 <= condition.size() && condition.substr(i, 7) == "defined") {
                    size_t j = i + 7;
                    while (j < condition.size() && std::isspace(condition[j])) j++;
                    bool hasParen = (j < condition.size() && condition[j] == '(');
                    if (hasParen) j++;
                    while (j < condition.size() && std::isspace(condition[j])) j++;
                    std::string ident;
                    while (j < condition.size() && (std::isalnum(condition[j]) || condition[j] == '_')) {
                        ident += condition[j++];
                    }
                    while (j < condition.size() && std::isspace(condition[j])) j++;
                    if (hasParen && j < condition.size() && condition[j] == ')') j++;
                    expanded += macroTable_.isDefined(ident) ? "1" : "0";
                    i = j;
                } else {
                    expanded += condition[i++];
                }
            }
            
            expanded = macroExpander_->expand(expanded);
            long long result = exprEvaluator_->evaluate(expanded);
            bool conditionTrue = (result != 0);
            
            state.active = conditionTrue;
            if (conditionTrue) {
                state.seenTrue = true;
            }
        } else {
            state.active = false;
        }
    }
}

void Preprocessor::visit(pp::ElseDirective&) {
    if (conditionalStack_.empty()) {
        error("#else without #if");
        return;
    }
    
    ConditionalState& state = conditionalStack_.top();
    
    if (state.isElse) {
        error("duplicate #else");
        return;
    }
    
    state.isElse = true;
    
    if (state.seenTrue) {
        state.active = false;
    } else {
        // Check parent is active
        bool parentActive = true;
        if (conditionalStack_.size() > 1) {
            std::stack<ConditionalState> temp;
            temp.push(conditionalStack_.top());
            conditionalStack_.pop();
            parentActive = isActive();
            conditionalStack_.push(temp.top());
        }
        
        state.active = parentActive;
        if (parentActive) {
            state.seenTrue = true;
        }
    }
}

void Preprocessor::visit(pp::EndifDirective&) {
    if (conditionalStack_.empty()) {
        error("#endif without #if");
        return;
    }
    
    conditionalStack_.pop();
}

void Preprocessor::visit(pp::ErrorDirective& dir) {
    if (!isActive()) return;
    
    error("#error " + dir.message);
}

void Preprocessor::visit(pp::PragmaDirective& dir) {
    if (!isActive()) return;
    
    // Handle "once" pragma
    if (dir.content == "once") {
        fileHandler_->markIncludedOnce(currentFile_);
    }
    
    // Other pragmas are implementation-defined, we can ignore them
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
