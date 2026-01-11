#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Parameter Declaration
// ============================================================================

void IRGenerator::visit(AST::ParamDecl& /*node*/) {
    // Parameters are handled in FunctionDecl
}

// ============================================================================
// Struct Declaration
// ============================================================================

void IRGenerator::visit(AST::StructDecl& node) {
    if (node.members.empty()) {
        // Forward declaration - nothing to emit
        return;
    }

    // Check if already processed
    if (structLayouts_.find(node.name) != structLayouts_.end()) {
        return;
    }

    // Compute and cache layout
    StructLayout layout = computeStructLayout(&node);
    structLayouts_[node.name] = layout;

    // Emit type definition if not already done
    emitGlobal(layout.llvmType);
}

// ============================================================================
// Enum Declaration
// ============================================================================

void IRGenerator::visit(AST::EnumDecl& node) {
    // Use pre-computed enum values from semantic analysis
    for (auto& enumerator : node.enumerators) {
        enumValues_[enumerator.name] = enumerator.computedValue;
    }
}

} // namespace cc1
