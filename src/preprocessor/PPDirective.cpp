#include <preprocessor/PPDirective.hpp>

namespace cc1 {
namespace pp {

void DefineDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

void UndefDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

void IncludeDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

void IfDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

void IfdefDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

void ElifDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

void ElseDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

void EndifDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

void ErrorDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

void PragmaDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

void LineDirective::accept(DirectiveVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace pp
} // namespace cc1
