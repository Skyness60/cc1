#include <semantics/SemanticConstExprVisitors.hpp>

namespace cc1 {

SemanticAnalyzer::ConstExprEvalVisitor::ConstExprEvalVisitor(SemanticAnalyzer& s, long long& out)
    : sema(s), result(out) {}

} // namespace cc1
