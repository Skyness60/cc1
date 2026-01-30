#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for while loops with condition/branch blocks.
// FR: Genere l IR pour les boucles while avec blocs condition.
void IRGenerator::visit(AST::WhileStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    std::string condLabel = newLabel("while.cond");
    std::string bodyLabel = newLabel("while.body");
    std::string endLabel = newLabel("while.end");

    
    breakLabels_.push(endLabel);
    continueLabels_.push(condLabel);

    
    emit("br label %" + condLabel);

    
    emitLabel(condLabel);
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
    emit("br i1 " + cmpReg + ", label %" + bodyLabel + ", label %" + endLabel);

    
    emitLabel(bodyLabel);
    node.body->accept(*this);
    emit("br label %" + condLabel);

    
    emitLabel(endLabel);

    
    breakLabels_.pop();
    continueLabels_.pop();
}

// EN: Emits IR for do-while loops with body-first execution.
// FR: Genere l IR pour les boucles do-while (corps d abord).
void IRGenerator::visit(AST::DoWhileStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    std::string bodyLabel = newLabel("do.body");
    std::string condLabel = newLabel("do.cond");
    std::string endLabel = newLabel("do.end");

    
    breakLabels_.push(endLabel);
    continueLabels_.push(condLabel);

    
    emit("br label %" + bodyLabel);

    
    emitLabel(bodyLabel);
    node.body->accept(*this);
    emit("br label %" + condLabel);

    
    emitLabel(condLabel);
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
    emit("br i1 " + cmpReg + ", label %" + bodyLabel + ", label %" + endLabel);

    
    emitLabel(endLabel);

    
    breakLabels_.pop();
    continueLabels_.pop();
}

// EN: Emits IR for for-loops with init/cond/inc blocks.
// FR: Genere l IR pour les boucles for (init/cond/inc).
void IRGenerator::visit(AST::ForStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    std::string condLabel = newLabel("for.cond");
    std::string bodyLabel = newLabel("for.body");
    std::string incLabel = newLabel("for.inc");
    std::string endLabel = newLabel("for.end");

    enterScope();

    
    if (node.init) {
        node.init->accept(*this);
    }
    emit("br label %" + condLabel);

    
    breakLabels_.push(endLabel);
    continueLabels_.push(incLabel);

    
    emitLabel(condLabel);
    if (node.condition) {
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
        emit("br i1 " + cmpReg + ", label %" + bodyLabel + ", label %" + endLabel);
    } else {
        emit("br label %" + bodyLabel);
    }

    
    emitLabel(bodyLabel);
    if (node.body) {
        node.body->accept(*this);
    }
    emit("br label %" + incLabel);

    
    emitLabel(incLabel);
    if (node.increment) {
        node.increment->accept(*this);
    }
    emit("br label %" + condLabel);

    
    emitLabel(endLabel);

    
    breakLabels_.pop();
    continueLabels_.pop();

    exitScope();
}

} 
