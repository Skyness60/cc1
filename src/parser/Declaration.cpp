#include <parser/ast/Declaration.hpp>
#include <parser/ast/Visitor.hpp>

namespace AST {

// EN: Dispatches visitor for variable declarations.
// FR: Dispatch le visiteur pour les declarations de variables.
void VarDecl::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for parameter declarations.
// FR: Dispatch le visiteur pour les declarations de parametres.
void ParamDecl::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for function declarations/definitions.
// FR: Dispatch le visiteur pour les declarations/definitions de fonctions.
void FunctionDecl::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for struct/union declarations.
// FR: Dispatch le visiteur pour les declarations struct/union.
void StructDecl::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for enum declarations.
// FR: Dispatch le visiteur pour les declarations enum.
void EnumDecl::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for typedef declarations.
// FR: Dispatch le visiteur pour les declarations typedef.
void TypedefDecl::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for translation unit nodes.
// FR: Dispatch le visiteur pour les unites de traduction.
void TranslationUnit::accept(Visitor& visitor) { visitor.visit(*this); }

} 

// TODO(cc1) EN: Add accept methods for new declaration nodes if added.
// FR: Ajouter les accept pour nouveaux noeuds de declaration.
