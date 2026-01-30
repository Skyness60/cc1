#include <semantics/SemanticConstExprVisitors.hpp>

namespace cc1 {

// EN: Builds a constant-expression evaluator tied to a result slot.
// FR: Construit l evaluateur d expression constante avec sortie cible.
SemanticAnalyzer::ConstExprEvalVisitor::ConstExprEvalVisitor(SemanticAnalyzer& s, long long& out)
    : sema(s), result(out) {}

} 
