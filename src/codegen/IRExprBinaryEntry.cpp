#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Dispatches binary expressions to assignment/logical/regular emitters.
// FR: Dispatch les binaires vers assignation/logique/regular.
void IRGenerator::visit(AST::BinaryExpr& node) {
    if (emitBinaryAssign(node)) return;
    if (emitBinaryCompoundAssign(node)) return;
    if (emitBinaryLogicalShortCircuit(node)) return;
    emitBinaryRegular(node);
}

} 
