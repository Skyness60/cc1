#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for break statements using the loop/switch break label.
// FR: Genere l IR pour break via le label de sortie.
void IRGenerator::visit(AST::BreakStmt& ) {
    if (breakLabels_.empty()) {
        hadError_ = true;
        return;
    }

    emit("br label %" + breakLabels_.top());

    
    std::string deadLabel = newLabel("break.dead");
    emitLabel(deadLabel);
}

// EN: Emits IR for continue statements using the loop continue label.
// FR: Genere l IR pour continue via le label de reprise.
void IRGenerator::visit(AST::ContinueStmt& ) {
    if (continueLabels_.empty()) {
        hadError_ = true;
        return;
    }

    emit("br label %" + continueLabels_.top());

    
    std::string deadLabel = newLabel("continue.dead");
    emitLabel(deadLabel);
}

// EN: Emits IR for return statements and stores the return value.
// FR: Genere l IR pour return et stocke la valeur de retour.
void IRGenerator::visit(AST::ReturnStmt& node) {
    if (node.value && currentFunctionReturnType_ != "void") {
        node.value->accept(*this);
        IRValue retVal = lastValue_;

        IRValue retLoaded = loadValue(retVal);
        std::string retReg = retLoaded.name;
        std::string retType = retLoaded.type;

        
        if (retType != currentFunctionReturnType_) {
            std::string converted = newTemp();
            
            // EN: Maps integer LLVM types to bit-width for promotions.
            // FR: Mappe les types entiers LLVM vers la largeur en bits.
            auto getIntSize = [](const std::string& t) -> int {
                if (t == "i8") return 8;
                if (t == "i16") return 16;
                if (t == "i32") return 32;
                if (t == "i64") return 64;
                return 0;
            };
            int srcSize = getIntSize(retType);
            int dstSize = getIntSize(currentFunctionReturnType_);

            if (srcSize > 0 && dstSize > 0) {
                if (srcSize > dstSize) {
                    emit(converted + " = trunc " + retType + " " + retReg + " to " + currentFunctionReturnType_);
                } else {
                    emit(converted + " = sext " + retType + " " + retReg + " to " + currentFunctionReturnType_);
                }
                retReg = converted;
            }
        }

        emit("store " + currentFunctionReturnType_ + " " + retReg + ", " + currentFunctionReturnType_ + "* " + returnValuePtr_);
    }

    emit("br label %" + returnLabel_);

    
    std::string deadLabel = newLabel("return.dead");
    emitLabel(deadLabel);
}

// EN: Emits IR for goto by branching to a named label.
// FR: Genere l IR pour goto via branchement vers label.
void IRGenerator::visit(AST::GotoStmt& node) {
    std::string targetLabel;

    auto it = gotoLabels_.find(node.label);
    if (it != gotoLabels_.end()) {
        targetLabel = it->second;
    } else {
        
        targetLabel = newLabel("label." + node.label);
        gotoLabels_[node.label] = targetLabel;
    }

    emit("br label %" + targetLabel);

    
    std::string deadLabel = newLabel("goto.dead");
    emitLabel(deadLabel);
}

// EN: Emits IR for a label and its associated statement body.
// FR: Genere l IR pour un label et son corps.
void IRGenerator::visit(AST::LabelStmt& node) {
    std::string labelName;

    auto it = gotoLabels_.find(node.label);
    if (it != gotoLabels_.end()) {
        labelName = it->second;
    } else {
        labelName = newLabel("label." + node.label);
        gotoLabels_[node.label] = labelName;
    }

    
    emit("br label %" + labelName);
    emitLabel(labelName);

    
    if (node.body) {
        node.body->accept(*this);
    }
}

} 
