#include <parser/AST.hpp>

// EN: Dispatches visitor for binary operation expression.
// FR: Dispatch le visiteur pour expression binaire.
void BinaryOpExpr::accept(Visitor& v)
{
    v.visit(*this);
}

// EN: Dispatches visitor for return statement.
// FR: Dispatch le visiteur pour instruction return.
void ReturnStatement::accept(Visitor& v)
{
    v.visit(*this);
}

namespace AST {
    // EN: Dispatches visitor for program node.
    // FR: Dispatch le visiteur pour noeud programme.
    void ProgramNode::accept(Visitor& v)
    {
        v.visit(*this);
    }
}

// TODO(cc1) EN: Expand AST.cpp if more legacy AST nodes are reintroduced.
// FR: Etendre AST.cpp si plus de noeuds AST legacy reviennent.
