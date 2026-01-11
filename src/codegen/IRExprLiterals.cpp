#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Integer Literal
// ============================================================================

void IRGenerator::visit(AST::IntegerLiteral& node) {
    lastValue_ = IRValue(std::to_string(node.value), "i32", false, true);
}

// ============================================================================
// Float Literal
// ============================================================================

void IRGenerator::visit(AST::FloatLiteral& node) {
    // LLVM uses hex representation for float constants
    lastValue_ = IRValue(std::to_string(node.value), "double", false, true);
}

// ============================================================================
// Char Literal
// ============================================================================

void IRGenerator::visit(AST::CharLiteral& node) {
    lastValue_ = IRValue(std::to_string(static_cast<int>(node.value)), "i8", false, true);
}

// ============================================================================
// String Literal
// ============================================================================

void IRGenerator::visit(AST::StringLiteral& node) {
    std::string globalName = newGlobalString(node.value);
    size_t len = node.value.length() + 1;

    // Get pointer to first element
    std::string temp = newTemp();
    emit(temp + " = getelementptr inbounds [" + std::to_string(len) + " x i8], [" +
         std::to_string(len) + " x i8]* " + globalName + ", i32 0, i32 0");

    lastValue_ = IRValue(temp, "i8*", false, false);
}

} // namespace cc1
