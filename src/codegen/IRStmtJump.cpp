#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Break Statement
// ============================================================================

void IRGenerator::visit(AST::BreakStmt& /*node*/) {
    if (breakLabels_.empty()) {
        hadError_ = true;
        return;
    }

    emit("br label %" + breakLabels_.top());

    // Dead code after break - emit unreachable label
    std::string deadLabel = newLabel("break.dead");
    emitLabel(deadLabel);
}

// ============================================================================
// Continue Statement
// ============================================================================

void IRGenerator::visit(AST::ContinueStmt& /*node*/) {
    if (continueLabels_.empty()) {
        hadError_ = true;
        return;
    }

    emit("br label %" + continueLabels_.top());

    // Dead code after continue
    std::string deadLabel = newLabel("continue.dead");
    emitLabel(deadLabel);
}

// ============================================================================
// Return Statement
// ============================================================================

void IRGenerator::visit(AST::ReturnStmt& node) {
    if (node.value && currentFunctionReturnType_ != "void") {
        node.value->accept(*this);
        IRValue retVal = lastValue_;

        IRValue retLoaded = loadValue(retVal);
        std::string retReg = retLoaded.name;
        std::string retType = retLoaded.type;

        // Convert return value to function return type if necessary
        if (retType != currentFunctionReturnType_) {
            std::string converted = newTemp();
            // Get sizes for integer types
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

    // Dead code after return
    std::string deadLabel = newLabel("return.dead");
    emitLabel(deadLabel);
}

// ============================================================================
// Goto Statement
// ============================================================================

void IRGenerator::visit(AST::GotoStmt& node) {
    std::string targetLabel;

    auto it = gotoLabels_.find(node.label);
    if (it != gotoLabels_.end()) {
        targetLabel = it->second;
    } else {
        // Label not seen yet - create placeholder
        targetLabel = newLabel("label." + node.label);
        gotoLabels_[node.label] = targetLabel;
    }

    emit("br label %" + targetLabel);

    // Dead code after goto
    std::string deadLabel = newLabel("goto.dead");
    emitLabel(deadLabel);
}

// ============================================================================
// Label Statement
// ============================================================================

void IRGenerator::visit(AST::LabelStmt& node) {
    std::string labelName;

    auto it = gotoLabels_.find(node.label);
    if (it != gotoLabels_.end()) {
        labelName = it->second;
    } else {
        labelName = newLabel("label." + node.label);
        gotoLabels_[node.label] = labelName;
    }

    // Branch to label (for fall-through)
    emit("br label %" + labelName);
    emitLabel(labelName);

    // Process body
    if (node.body) {
        node.body->accept(*this);
    }
}

} // namespace cc1
