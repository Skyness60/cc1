#include <parser/ast/Expression.hpp>
#include <parser/ast/Visitor.hpp>

namespace AST {

// EN: Dispatches visitor for integer literal nodes.
// FR: Dispatch le visiteur pour les litteraux entiers.
void IntegerLiteral::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for float literal nodes.
// FR: Dispatch le visiteur pour les litteraux flottants.
void FloatLiteral::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for char literal nodes.
// FR: Dispatch le visiteur pour les litteraux caractere.
void CharLiteral::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for string literal nodes.
// FR: Dispatch le visiteur pour les litteraux chaine.
void StringLiteral::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for identifier nodes.
// FR: Dispatch le visiteur pour les identifiants.
void Identifier::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for binary expression nodes.
// FR: Dispatch le visiteur pour les expressions binaires.
void BinaryExpr::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for unary expression nodes.
// FR: Dispatch le visiteur pour les expressions unaires.
void UnaryExpr::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for cast expression nodes.
// FR: Dispatch le visiteur pour les expressions de cast.
void CastExpr::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for sizeof expression nodes.
// FR: Dispatch le visiteur pour les expressions sizeof.
void SizeofExpr::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for call expression nodes.
// FR: Dispatch le visiteur pour les appels.
void CallExpr::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for member access nodes.
// FR: Dispatch le visiteur pour les acces membre.
void MemberExpr::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for index expression nodes.
// FR: Dispatch le visiteur pour les indexations.
void IndexExpr::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for ternary expression nodes.
// FR: Dispatch le visiteur pour les ternaires.
void TernaryExpr::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for initializer list nodes.
// FR: Dispatch le visiteur pour les listes d initialisation.
void InitializerList::accept(Visitor& visitor) { visitor.visit(*this); }

} 

// TODO(cc1) EN: Add accept methods for any future expression node types.
// FR: Ajouter les accept pour futurs types d expressions.
