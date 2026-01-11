#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Binary Expression
// ============================================================================

void IRGenerator::visit(AST::BinaryExpr& node) {
    if (emitBinaryAssign(node)) return;
    if (emitBinaryCompoundAssign(node)) return;
    if (emitBinaryLogicalShortCircuit(node)) return;
    emitBinaryRegular(node);
}

} // namespace cc1
