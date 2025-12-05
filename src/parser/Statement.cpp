#include <parser/ast/Statement.hpp>
#include <parser/ast/Visitor.hpp>

namespace AST {

void CompoundStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void ExpressionStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void IfStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void WhileStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void DoWhileStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void ForStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void SwitchStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void CaseStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void DefaultStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void BreakStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void ContinueStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void ReturnStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void GotoStmt::accept(Visitor& visitor) { visitor.visit(*this); }
void LabelStmt::accept(Visitor& visitor) { visitor.visit(*this); }

} // namespace AST
