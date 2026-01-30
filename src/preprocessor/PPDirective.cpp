#include <preprocessor/PPDirective.hpp>

namespace cc1 {
namespace pp {

// EN: Accepts a visitor for #define handling.
// FR: Accepte un visiteur pour traiter #define.
void DefineDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

// EN: Accepts a visitor for #undef handling.
// FR: Accepte un visiteur pour traiter #undef.
void UndefDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

// EN: Accepts a visitor for #include handling.
// FR: Accepte un visiteur pour traiter #include.
void IncludeDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

// EN: Accepts a visitor for #if handling.
// FR: Accepte un visiteur pour traiter #if.
void IfDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

// EN: Accepts a visitor for #ifdef/#ifndef handling.
// FR: Accepte un visiteur pour traiter #ifdef/#ifndef.
void IfdefDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

// EN: Accepts a visitor for #elif handling.
// FR: Accepte un visiteur pour traiter #elif.
void ElifDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

// EN: Accepts a visitor for #else handling.
// FR: Accepte un visiteur pour traiter #else.
void ElseDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

// EN: Accepts a visitor for #endif handling.
// FR: Accepte un visiteur pour traiter #endif.
void EndifDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

// EN: Accepts a visitor for #error handling.
// FR: Accepte un visiteur pour traiter #error.
void ErrorDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

// EN: Accepts a visitor for #pragma handling.
// FR: Accepte un visiteur pour traiter #pragma.
void PragmaDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

// EN: Accepts a visitor for #line handling.
// FR: Accepte un visiteur pour traiter #line.
void LineDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

} 
} 

// TODO(cc1) EN: Add instrumentation hooks for directive visitation tracing.
// FR: Ajouter des hooks d instrumentation pour tracer les visites.
