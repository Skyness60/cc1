#include <parser/ast/Declaration.hpp>
#include <parser/ast/Visitor.hpp>

namespace AST {

void VarDecl::accept(Visitor& visitor) { visitor.visit(*this); }
void ParamDecl::accept(Visitor& visitor) { visitor.visit(*this); }
void FunctionDecl::accept(Visitor& visitor) { visitor.visit(*this); }
void StructDecl::accept(Visitor& visitor) { visitor.visit(*this); }
void EnumDecl::accept(Visitor& visitor) { visitor.visit(*this); }
void TypedefDecl::accept(Visitor& visitor) { visitor.visit(*this); }
void TranslationUnit::accept(Visitor& visitor) { visitor.visit(*this); }

} // namespace AST
