#include <parser/ast/Statement.hpp>
#include <parser/ast/Visitor.hpp>

namespace AST {

// EN: Dispatches visitor for compound statement nodes.
// FR: Dispatch le visiteur pour les blocs composes.
void CompoundStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for expression statement nodes.
// FR: Dispatch le visiteur pour les instructions expression.
void ExpressionStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for if statement nodes.
// FR: Dispatch le visiteur pour les instructions if.
void IfStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for while statement nodes.
// FR: Dispatch le visiteur pour les instructions while.
void WhileStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for do-while statement nodes.
// FR: Dispatch le visiteur pour les instructions do-while.
void DoWhileStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for for statement nodes.
// FR: Dispatch le visiteur pour les instructions for.
void ForStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for switch statement nodes.
// FR: Dispatch le visiteur pour les instructions switch.
void SwitchStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for case statement nodes.
// FR: Dispatch le visiteur pour les instructions case.
void CaseStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for default statement nodes.
// FR: Dispatch le visiteur pour les instructions default.
void DefaultStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for break statement nodes.
// FR: Dispatch le visiteur pour les instructions break.
void BreakStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for continue statement nodes.
// FR: Dispatch le visiteur pour les instructions continue.
void ContinueStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for return statement nodes.
// FR: Dispatch le visiteur pour les instructions return.
void ReturnStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for goto statement nodes.
// FR: Dispatch le visiteur pour les instructions goto.
void GotoStmt::accept(Visitor& visitor) { visitor.visit(*this); }
// EN: Dispatches visitor for labeled statement nodes.
// FR: Dispatch le visiteur pour les instructions etiquetees.
void LabelStmt::accept(Visitor& visitor) { visitor.visit(*this); }

} 

// TODO(cc1) EN: Add accept methods for future statement node types.
// FR: Ajouter les accept pour futurs types d instructions.
