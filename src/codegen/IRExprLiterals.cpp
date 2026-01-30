#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for integer literals as immediate i32 constants.
// FR: Genere l IR pour les entiers litteraux en constantes i32.
void IRGenerator::visit(AST::IntegerLiteral& node) {
    lastValue_ = IRValue(std::to_string(node.value), "i32", false, true);
}

// EN: Emits IR for floating literals as immediate double constants.
// FR: Genere l IR pour les flottants litteraux en constantes double.
void IRGenerator::visit(AST::FloatLiteral& node) {
    
    lastValue_ = IRValue(std::to_string(node.value), "double", false, true);
}

// EN: Emits IR for char literals as immediate i8 constants.
// FR: Genere l IR pour les caracteres litteraux en constantes i8.
void IRGenerator::visit(AST::CharLiteral& node) {
    lastValue_ = IRValue(std::to_string(static_cast<int>(node.value)), "i8", false, true);
}

// EN: Emits IR for string literals via a global constant and GEP.
// FR: Genere l IR pour les chaines via globale constante et GEP.
void IRGenerator::visit(AST::StringLiteral& node) {
    std::string globalName = newGlobalString(node.value);
    size_t len = node.value.length() + 1;

    
    std::string idxType = is64bit_ ? "i64" : "i32";

    std::string temp = newTemp();
    emit(temp + " = getelementptr inbounds [" + std::to_string(len) + " x i8], [" +
         std::to_string(len) + " x i8]* " + globalName + ", " + idxType + " 0, " + idxType + " 0");

    lastValue_ = IRValue(temp, "i8*", false, false);
}

} 
