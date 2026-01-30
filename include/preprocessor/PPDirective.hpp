#pragma once



#include <string>
#include <vector>
#include <memory>

namespace cc1 {
namespace pp {


class DirectiveVisitor;





// EN: Base class for all preprocessor directives.
// FR: Classe de base pour toutes les directives du preprocesseur.
class Directive {
public:
    // EN: Virtual destructor for polymorphic cleanup.
    // FR: Destructeur virtuel pour nettoyage polymorphique.
    virtual ~Directive() = default;
    // EN: Accepts a visitor to handle directive-specific behavior.
    // FR: Accepte un visiteur pour le comportement specifique.
    virtual void accept(DirectiveVisitor& visitor) = 0;
};





// EN: Represents a #define directive with optional params.
// FR: Represente une directive #define avec params optionnels.
class DefineDirective : public Directive {
public:
    std::string name;
    std::vector<std::string> parameters;
    std::string body;
    bool isFunctionLike;
    bool isVariadic;
    
    // EN: Builds a define directive node with its full payload.
    // FR: Construit un noeud #define avec ses donnees.
    DefineDirective(const std::string& n, const std::vector<std::string>& params,
                    const std::string& b, bool func, bool var)
        : name(n), parameters(params), body(b), isFunctionLike(func), isVariadic(var) {}
    
    // EN: Dispatches to the visitor for #define handling.
    // FR: Dispatch vers le visiteur pour #define.
    void accept(DirectiveVisitor& visitor) override;
};

// EN: Represents a #undef directive.
// FR: Represente une directive #undef.
class UndefDirective : public Directive {
public:
    std::string name;
    
    // EN: Builds an undef directive node.
    // FR: Construit un noeud #undef.
    explicit UndefDirective(const std::string& n) : name(n) {}
    
    // EN: Dispatches to the visitor for #undef handling.
    // FR: Dispatch vers le visiteur pour #undef.
    void accept(DirectiveVisitor& visitor) override;
};

// EN: Represents a #include directive with system/local flag.
// FR: Represente une directive #include avec flag systeme/local.
class IncludeDirective : public Directive {
public:
    std::string filename;
    bool isSystemInclude;
    
    // EN: Builds an include directive node.
    // FR: Construit un noeud #include.
    IncludeDirective(const std::string& f, bool sys)
        : filename(f), isSystemInclude(sys) {}
    
    // EN: Dispatches to the visitor for #include handling.
    // FR: Dispatch vers le visiteur pour #include.
    void accept(DirectiveVisitor& visitor) override;
};

// EN: Represents a #if directive with a raw condition string.
// FR: Represente une directive #if avec condition brute.
class IfDirective : public Directive {
public:
    std::string condition;
    
    // EN: Builds an if directive node.
    // FR: Construit un noeud #if.
    explicit IfDirective(const std::string& cond) : condition(cond) {}
    
    // EN: Dispatches to the visitor for #if handling.
    // FR: Dispatch vers le visiteur pour #if.
    void accept(DirectiveVisitor& visitor) override;
};

// EN: Represents a #ifdef/#ifndef directive.
// FR: Represente une directive #ifdef/#ifndef.
class IfdefDirective : public Directive {
public:
    std::string name;
    bool isIfndef;  
    
    // EN: Builds an ifdef/ifndef directive node.
    // FR: Construit un noeud #ifdef/#ifndef.
    IfdefDirective(const std::string& n, bool inv = false)
        : name(n), isIfndef(inv) {}
    
    // EN: Dispatches to the visitor for #ifdef/#ifndef handling.
    // FR: Dispatch vers le visiteur pour #ifdef/#ifndef.
    void accept(DirectiveVisitor& visitor) override;
};

// EN: Represents a #elif directive.
// FR: Represente une directive #elif.
class ElifDirective : public Directive {
public:
    std::string condition;
    
    // EN: Builds an elif directive node.
    // FR: Construit un noeud #elif.
    explicit ElifDirective(const std::string& cond) : condition(cond) {}
    
    // EN: Dispatches to the visitor for #elif handling.
    // FR: Dispatch vers le visiteur pour #elif.
    void accept(DirectiveVisitor& visitor) override;
};

// EN: Represents a #else directive.
// FR: Represente une directive #else.
class ElseDirective : public Directive {
public:
    // EN: Default constructor for #else.
    // FR: Constructeur par defaut pour #else.
    ElseDirective() = default;
    
    // EN: Dispatches to the visitor for #else handling.
    // FR: Dispatch vers le visiteur pour #else.
    void accept(DirectiveVisitor& visitor) override;
};

// EN: Represents a #endif directive.
// FR: Represente une directive #endif.
class EndifDirective : public Directive {
public:
    // EN: Default constructor for #endif.
    // FR: Constructeur par defaut pour #endif.
    EndifDirective() = default;
    
    // EN: Dispatches to the visitor for #endif handling.
    // FR: Dispatch vers le visiteur pour #endif.
    void accept(DirectiveVisitor& visitor) override;
};

// EN: Represents a #error directive with a message.
// FR: Represente une directive #error avec message.
class ErrorDirective : public Directive {
public:
    std::string message;
    
    // EN: Builds an error directive node.
    // FR: Construit un noeud #error.
    explicit ErrorDirective(const std::string& msg) : message(msg) {}
    
    // EN: Dispatches to the visitor for #error handling.
    // FR: Dispatch vers le visiteur pour #error.
    void accept(DirectiveVisitor& visitor) override;
};

// EN: Represents a #pragma directive with raw content.
// FR: Represente une directive #pragma avec contenu brut.
class PragmaDirective : public Directive {
public:
    std::string content;
    
    // EN: Builds a pragma directive node.
    // FR: Construit un noeud #pragma.
    explicit PragmaDirective(const std::string& c) : content(c) {}
    
    // EN: Dispatches to the visitor for #pragma handling.
    // FR: Dispatch vers le visiteur pour #pragma.
    void accept(DirectiveVisitor& visitor) override;
};

// EN: Represents a #line directive for logical file/line remapping.
// FR: Represente une directive #line pour remapper fichier/ligne.
class LineDirective : public Directive {
public:
    int lineNumber;
    std::string filename;
    
    // EN: Builds a line directive node with optional filename.
    // FR: Construit un noeud #line avec nom optionnel.
    LineDirective(int line, const std::string& file = "")
        : lineNumber(line), filename(file) {}
    
    // EN: Dispatches to the visitor for #line handling.
    // FR: Dispatch vers le visiteur pour #line.
    void accept(DirectiveVisitor& visitor) override;
};





// EN: Visitor interface to handle each directive type.
// FR: Interface visiteur pour traiter chaque type de directive.
class DirectiveVisitor {
public:
    // EN: Virtual destructor for polymorphic cleanup.
    // FR: Destructeur virtuel pour nettoyage polymorphique.
    virtual ~DirectiveVisitor() = default;
    
    // EN: Visits a #define directive node.
    // FR: Visite un noeud #define.
    virtual void visit(DefineDirective& dir) = 0;
    // EN: Visits a #undef directive node.
    // FR: Visite un noeud #undef.
    virtual void visit(UndefDirective& dir) = 0;
    // EN: Visits a #include directive node.
    // FR: Visite un noeud #include.
    virtual void visit(IncludeDirective& dir) = 0;
    // EN: Visits a #if directive node.
    // FR: Visite un noeud #if.
    virtual void visit(IfDirective& dir) = 0;
    // EN: Visits a #ifdef/#ifndef directive node.
    // FR: Visite un noeud #ifdef/#ifndef.
    virtual void visit(IfdefDirective& dir) = 0;
    // EN: Visits a #elif directive node.
    // FR: Visite un noeud #elif.
    virtual void visit(ElifDirective& dir) = 0;
    // EN: Visits a #else directive node.
    // FR: Visite un noeud #else.
    virtual void visit(ElseDirective& dir) = 0;
    // EN: Visits a #endif directive node.
    // FR: Visite un noeud #endif.
    virtual void visit(EndifDirective& dir) = 0;
    // EN: Visits a #error directive node.
    // FR: Visite un noeud #error.
    virtual void visit(ErrorDirective& dir) = 0;
    // EN: Visits a #pragma directive node.
    // FR: Visite un noeud #pragma.
    virtual void visit(PragmaDirective& dir) = 0;
    // EN: Visits a #line directive node.
    // FR: Visite un noeud #line.
    virtual void visit(LineDirective& dir) = 0;
};

// TODO(cc1) EN: Add directive source span metadata for richer diagnostics.
// FR: Ajouter des spans source aux directives pour de meilleurs diagnostics.
} 
} 
