#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Compound Statement
// ============================================================================

void IRGenerator::visit(AST::CompoundStmt& node) {
    enterScope();
    
    // Process declarations
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
    
    // Process statements
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
    
    exitScope();
}

// ============================================================================
// Expression Statement
// ============================================================================

void IRGenerator::visit(AST::ExpressionStmt& node) {
    if (node.expression) {
        node.expression->accept(*this);
        // Result is discarded
    }
}

// ============================================================================
// If Statement
// ============================================================================

void IRGenerator::visit(AST::IfStmt& node) {
    std::string thenLabel = newLabel("if.then");
    std::string elseLabel = node.elseBranch ? newLabel("if.else") : "";
    std::string endLabel = newLabel("if.end");
    
    // Evaluate condition
    node.condition->accept(*this);
    IRValue condVal = lastValue_;
    
    // Load if pointer to a value (not a pointer result like from function call)
    std::string condReg = condVal.name;
    std::string condType = condVal.type;
    if (condVal.isPointer && !condVal.isConstant) {
        condReg = newTemp();
        condType = condVal.derefType();
        emit(condReg + " = load " + condType + ", " + condVal.type + " " + condVal.name);
    }
    
    // Compare to zero (C: non-zero is true)
    // For pointers, compare against null
    std::string cmpReg = newTemp();
    if (condType.back() == '*') {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", null");
    } else {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", 0");
    }
    
    // Branch
    if (node.elseBranch) {
        emit("br i1 " + cmpReg + ", label %" + thenLabel + ", label %" + elseLabel);
    } else {
        emit("br i1 " + cmpReg + ", label %" + thenLabel + ", label %" + endLabel);
    }
    
    // Then block
    emitLabel(thenLabel);
    node.thenBranch->accept(*this);
    emit("br label %" + endLabel);
    
    // Else block
    if (node.elseBranch) {
        emitLabel(elseLabel);
        node.elseBranch->accept(*this);
        emit("br label %" + endLabel);
    }
    
    // End label
    emitLabel(endLabel);
}

// ============================================================================
// While Statement
// ============================================================================

void IRGenerator::visit(AST::WhileStmt& node) {
    std::string condLabel = newLabel("while.cond");
    std::string bodyLabel = newLabel("while.body");
    std::string endLabel = newLabel("while.end");
    
    // Push break/continue targets
    breakLabels_.push(endLabel);
    continueLabels_.push(condLabel);
    
    // Branch to condition
    emit("br label %" + condLabel);
    
    // Condition block
    emitLabel(condLabel);
    node.condition->accept(*this);
    IRValue condVal = lastValue_;
    
    std::string condReg = condVal.name;
    std::string condType = condVal.type;
    if (condVal.isPointer && !condVal.isConstant) {
        condReg = newTemp();
        condType = condVal.derefType();
        emit(condReg + " = load " + condType + ", " + condVal.type + " " + condVal.name);
    }
    
    std::string cmpReg = newTemp();
    if (condType.back() == '*') {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", null");
    } else {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", 0");
    }
    emit("br i1 " + cmpReg + ", label %" + bodyLabel + ", label %" + endLabel);
    
    // Body block
    emitLabel(bodyLabel);
    node.body->accept(*this);
    emit("br label %" + condLabel);
    
    // End block
    emitLabel(endLabel);
    
    // Pop break/continue targets
    breakLabels_.pop();
    continueLabels_.pop();
}

// ============================================================================
// Do-While Statement
// ============================================================================

void IRGenerator::visit(AST::DoWhileStmt& node) {
    std::string bodyLabel = newLabel("do.body");
    std::string condLabel = newLabel("do.cond");
    std::string endLabel = newLabel("do.end");
    
    // Push break/continue targets
    breakLabels_.push(endLabel);
    continueLabels_.push(condLabel);
    
    // Branch to body
    emit("br label %" + bodyLabel);
    
    // Body block
    emitLabel(bodyLabel);
    node.body->accept(*this);
    emit("br label %" + condLabel);
    
    // Condition block
    emitLabel(condLabel);
    node.condition->accept(*this);
    IRValue condVal = lastValue_;
    
    std::string condReg = condVal.name;
    std::string condType = condVal.type;
    if (condVal.isPointer && !condVal.isConstant) {
        condReg = newTemp();
        condType = condVal.derefType();
        emit(condReg + " = load " + condType + ", " + condVal.type + " " + condVal.name);
    }
    
    std::string cmpReg = newTemp();
    if (condType.back() == '*') {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", null");
    } else {
        emit(cmpReg + " = icmp ne " + condType + " " + condReg + ", 0");
    }
    emit("br i1 " + cmpReg + ", label %" + bodyLabel + ", label %" + endLabel);
    
    // End block
    emitLabel(endLabel);
    
    // Pop break/continue targets
    breakLabels_.pop();
    continueLabels_.pop();
}

// ============================================================================
// For Statement
// ============================================================================

void IRGenerator::visit(AST::ForStmt& node) {
    std::string condLabel = newLabel("for.cond");
    std::string bodyLabel = newLabel("for.body");
    std::string incLabel = newLabel("for.inc");
    std::string endLabel = newLabel("for.end");
    
    enterScope();
    
    // Init
    if (node.init) {
        node.init->accept(*this);
    }
    emit("br label %" + condLabel);
    
    // Push break/continue targets
    breakLabels_.push(endLabel);
    continueLabels_.push(incLabel);
    
    // Condition
    emitLabel(condLabel);
    if (node.condition) {
        node.condition->accept(*this);
        IRValue condVal = lastValue_;
        
        std::string condReg = condVal.name;
        std::string condType = condVal.type;
        if (condVal.isPointer && !condVal.isConstant) {
            condReg = newTemp();
            condType = condVal.derefType();
            emit(condReg + " = load " + condType + ", " + condVal.type + " " + condVal.name);
        }
        
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
    
    // Body
    emitLabel(bodyLabel);
    if (node.body) {
        node.body->accept(*this);
    }
    emit("br label %" + incLabel);
    
    // Increment
    emitLabel(incLabel);
    if (node.increment) {
        node.increment->accept(*this);
    }
    emit("br label %" + condLabel);
    
    // End
    emitLabel(endLabel);
    
    // Pop break/continue targets
    breakLabels_.pop();
    continueLabels_.pop();
    
    exitScope();
}

// ============================================================================
// Switch Statement
// ============================================================================

void IRGenerator::visit(AST::SwitchStmt& node) {
    // Evaluate switch expression
    node.expression->accept(*this);
    IRValue switchVal = lastValue_;
    
    std::string switchReg = switchVal.name;
    std::string switchType = switchVal.type;
    if (switchVal.isPointer && !switchVal.isConstant) {
        switchReg = newTemp();
        switchType = switchVal.derefType();
        emit(switchReg + " = load " + switchType + ", " + switchVal.type + " " + switchVal.name);
    }
    
    std::string defaultLabel = newLabel("sw.default");
    std::string endLabel = newLabel("sw.end");
    
    // Push switch context
    SwitchContext ctx;
    ctx.switchVar = switchReg;
    ctx.switchType = switchType;
    ctx.defaultLabel = defaultLabel;
    ctx.endLabel = endLabel;
    switchStack_.push(ctx);
    
    // Push break target
    breakLabels_.push(endLabel);
    
    // Process body to collect cases (first pass)
    // We need to scan the body for case statements
    std::string bodyLabel = newLabel("sw.body");
    emit("br label %" + bodyLabel);
    
    emitLabel(bodyLabel);
    if (node.body) {
        node.body->accept(*this);
    }
    emit("br label %" + endLabel);
    
    // Default label (if not hit by case)
    emitLabel(defaultLabel);
    emit("br label %" + endLabel);
    
    // End label
    emitLabel(endLabel);
    
    // Pop break target and switch context
    breakLabels_.pop();
    switchStack_.pop();
}

// ============================================================================
// Case Statement
// ============================================================================

void IRGenerator::visit(AST::CaseStmt& node) {
    if (switchStack_.empty()) {
        // Error: case outside switch
        hadError_ = true;
        return;
    }
    
    SwitchContext& ctx = switchStack_.top();
    
    // Evaluate case value
    long long caseVal = 0;
    if (node.value) {
        evaluateConstantExpr(node.value.get(), caseVal);
    }
    
    std::string caseLabel = newLabel("sw.case");
    std::string nextLabel = newLabel("sw.next");
    
    // Compare switch value to case value
    std::string cmpReg = newTemp();
    emit(cmpReg + " = icmp eq " + ctx.switchType + " " + ctx.switchVar + ", " + std::to_string(caseVal));
    emit("br i1 " + cmpReg + ", label %" + caseLabel + ", label %" + nextLabel);
    
    // Case body
    emitLabel(caseLabel);
    if (node.body) {
        node.body->accept(*this);
    }
    // Fall through to next case (emit explicit branch)
    emit("br label %" + nextLabel);
    
    // Next label for comparison chain
    emitLabel(nextLabel);
}

// ============================================================================
// Default Statement
// ============================================================================

void IRGenerator::visit(AST::DefaultStmt& node) {
    if (switchStack_.empty()) {
        hadError_ = true;
        return;
    }
    
    // The default label is emitted after all cases
    if (node.body) {
        node.body->accept(*this);
    }
}

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
        
        std::string retReg = retVal.name;
        std::string retType = retVal.type;
        if (retVal.isPointer && !retVal.isConstant) {
            retReg = newTemp();
            retType = retVal.derefType();
            emit(retReg + " = load " + retType + ", " + retVal.type + " " + retVal.name);
        }
        
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
        
        emit("store " + currentFunctionReturnType_ + " " + retReg + ", " + 
             currentFunctionReturnType_ + "* " + returnValuePtr_);
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
