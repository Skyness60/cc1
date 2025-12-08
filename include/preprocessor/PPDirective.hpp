#pragma once

#include <string>
#include <vector>
#include <memory>

namespace cc1 {
namespace pp {

// Forward declarations
class DirectiveVisitor;

// ============================================================================
// Base Directive class
// ============================================================================

class Directive {
public:
    virtual ~Directive() = default;
    virtual void accept(DirectiveVisitor& visitor) = 0;
};

// ============================================================================
// Concrete Directive types
// ============================================================================

class DefineDirective : public Directive {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::string body;
    bool isFunctionLike;
    bool isVariadic;
    
    DefineDirective(const std::string& n, const std::vector<std::string>& params,
                    const std::string& b, bool func, bool var)
        : name(n), parameters(params), body(b), isFunctionLike(func), isVariadic(var) {}
    
    void accept(DirectiveVisitor& visitor) override;
};

class UndefDirective : public Directive {
public:
    std::string name;
    
    explicit UndefDirective(const std::string& n) : name(n) {}
    
    void accept(DirectiveVisitor& visitor) override;
};

class IncludeDirective : public Directive {
public:
    std::string filename;
    bool isSystemInclude;
    
    IncludeDirective(const std::string& f, bool sys)
        : filename(f), isSystemInclude(sys) {}
    
    void accept(DirectiveVisitor& visitor) override;
};

class IfDirective : public Directive {
public:
    std::string condition;
    
    explicit IfDirective(const std::string& cond) : condition(cond) {}
    
    void accept(DirectiveVisitor& visitor) override;
};

class IfdefDirective : public Directive {
public:
    std::string name;
    bool isIfndef;  // true for #ifndef
    
    IfdefDirective(const std::string& n, bool inv = false)
        : name(n), isIfndef(inv) {}
    
    void accept(DirectiveVisitor& visitor) override;
};

class ElifDirective : public Directive {
public:
    std::string condition;
    
    explicit ElifDirective(const std::string& cond) : condition(cond) {}
    
    void accept(DirectiveVisitor& visitor) override;
};

class ElseDirective : public Directive {
public:
    ElseDirective() = default;
    
    void accept(DirectiveVisitor& visitor) override;
};

class EndifDirective : public Directive {
public:
    EndifDirective() = default;
    
    void accept(DirectiveVisitor& visitor) override;
};

class ErrorDirective : public Directive {
public:
    std::string message;
    
    explicit ErrorDirective(const std::string& msg) : message(msg) {}
    
    void accept(DirectiveVisitor& visitor) override;
};

class PragmaDirective : public Directive {
public:
    std::string content;
    
    explicit PragmaDirective(const std::string& c) : content(c) {}
    
    void accept(DirectiveVisitor& visitor) override;
};

class LineDirective : public Directive {
public:
    int lineNumber;
    std::string filename;
    
    LineDirective(int line, const std::string& file = "")
        : lineNumber(line), filename(file) {}
    
    void accept(DirectiveVisitor& visitor) override;
};

// ============================================================================
// Visitor Interface
// ============================================================================

class DirectiveVisitor {
public:
    virtual ~DirectiveVisitor() = default;
    
    virtual void visit(DefineDirective& dir) = 0;
    virtual void visit(UndefDirective& dir) = 0;
    virtual void visit(IncludeDirective& dir) = 0;
    virtual void visit(IfDirective& dir) = 0;
    virtual void visit(IfdefDirective& dir) = 0;
    virtual void visit(ElifDirective& dir) = 0;
    virtual void visit(ElseDirective& dir) = 0;
    virtual void visit(EndifDirective& dir) = 0;
    virtual void visit(ErrorDirective& dir) = 0;
    virtual void visit(PragmaDirective& dir) = 0;
    virtual void visit(LineDirective& dir) = 0;
};

} // namespace pp
} // namespace cc1
