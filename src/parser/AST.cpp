#include <parser/AST.hpp>

void BinaryOpExpr::accept(Visitor& v)
{
    v.visit(*this);
}

void ReturnStatement::accept(Visitor& v)
{
    v.visit(*this);
}

namespace AST {
    void ProgramNode::accept(Visitor& v)
    {
        v.visit(*this);
    }
}