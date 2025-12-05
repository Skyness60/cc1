#include <parser/ast/Expression.hpp>
#include <parser/ast/Visitor.hpp>

namespace AST {

void IntegerLiteral::accept(Visitor& visitor) { visitor.visit(*this); }
void FloatLiteral::accept(Visitor& visitor) { visitor.visit(*this); }
void CharLiteral::accept(Visitor& visitor) { visitor.visit(*this); }
void StringLiteral::accept(Visitor& visitor) { visitor.visit(*this); }
void Identifier::accept(Visitor& visitor) { visitor.visit(*this); }
void BinaryExpr::accept(Visitor& visitor) { visitor.visit(*this); }
void UnaryExpr::accept(Visitor& visitor) { visitor.visit(*this); }
void CastExpr::accept(Visitor& visitor) { visitor.visit(*this); }
void SizeofExpr::accept(Visitor& visitor) { visitor.visit(*this); }
void CallExpr::accept(Visitor& visitor) { visitor.visit(*this); }
void MemberExpr::accept(Visitor& visitor) { visitor.visit(*this); }
void IndexExpr::accept(Visitor& visitor) { visitor.visit(*this); }
void TernaryExpr::accept(Visitor& visitor) { visitor.visit(*this); }
void InitializerList::accept(Visitor& visitor) { visitor.visit(*this); }

} // namespace AST
