#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for if/else control flow using branches.
// FR: Genere l IR pour if/else via branches.
void IRGenerator::visit(AST::IfStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    std::string thenLabel = newLabel("if.then");
    std::string elseLabel = node.elseBranch ? newLabel("if.else") : "";
    std::string endLabel = newLabel("if.end");

    
    node.condition->accept(*this);
    IRValue condVal = lastValue_;

    
    IRValue condLoaded = loadValue(condVal);
    std::string condReg = condLoaded.name;
    std::string condType = condLoaded.type;

    
    
    std::string cmpReg = newTemp();
    if (condType.back() == '*') {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", null");
    } else {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", 0");
    }

    
    if (node.elseBranch) {
        emit("br i1 " + cmpReg + ", label %" + thenLabel + ", label %" + elseLabel);
    } else {
        emit("br i1 " + cmpReg + ", label %" + thenLabel + ", label %" + endLabel);
    }

    
    emitLabel(thenLabel);
    node.thenBranch->accept(*this);
    emit("br label %" + endLabel);

    
    if (node.elseBranch) {
        emitLabel(elseLabel);
        node.elseBranch->accept(*this);
        emit("br label %" + endLabel);
    }

    
    emitLabel(endLabel);
}

} 
