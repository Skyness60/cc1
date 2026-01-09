#include <codegen/IRGenerator.hpp>

#include <functional>

namespace cc1 {

// ============================================================================
// Compound Statement
// ============================================================================

void IRGenerator::visit(AST::CompoundStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
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
    DebugLocGuard loc(*this, node.line, node.column);
    if (node.expression) {
        node.expression->accept(*this);
        // Result is discarded
    }
}

// ============================================================================
// If Statement
// ============================================================================

void IRGenerator::visit(AST::IfStmt& node) {
    DebugLocGuard loc(*this, node.line, node.column);
    std::string thenLabel = newLabel("if.then");
    std::string elseLabel = node.elseBranch ? newLabel("if.else") : "";
    std::string endLabel = newLabel("if.end");
    
    // Evaluate condition
    node.condition->accept(*this);
    IRValue condVal = lastValue_;

    // Convert to rvalue (also extracts bitfields).
    IRValue condLoaded = loadValue(condVal);
    std::string condReg = condLoaded.name;
    std::string condType = condLoaded.type;
    
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
    DebugLocGuard loc(*this, node.line, node.column);
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

    // Convert to rvalue (also extracts bitfields).
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
    DebugLocGuard loc(*this, node.line, node.column);
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

    // Convert to rvalue (also extracts bitfields).
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
    DebugLocGuard loc(*this, node.line, node.column);
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

        // Convert to rvalue (also extracts bitfields).
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
    IRValue switchLoaded = loadValue(lastValue_);

    auto isUnsignedSwitchExpr = [&]() -> bool {
        if (!node.expression || !node.expression->resolvedType) return false;
        if (auto* prim = dynamic_cast<AST::PrimitiveType*>(stripQualifiers(node.expression->resolvedType.get()))) {
            switch (prim->kind) {
                case AST::PrimitiveKind::UnsignedChar:
                case AST::PrimitiveKind::UnsignedShort:
                case AST::PrimitiveKind::UnsignedInt:
                case AST::PrimitiveKind::UnsignedLong:
                case AST::PrimitiveKind::UnsignedLongLong:
                    return true;
                default:
                    return false;
            }
        }
        return false;
    };

    // Normalize the switch condition to i32 (C integer promotions).
    std::string switchReg = switchLoaded.name;
    std::string switchType = switchLoaded.type;
    if (switchType != "i32") {
        auto intBits = [](const std::string& t) -> int {
            if (t == "i8") return 8;
            if (t == "i16") return 16;
            if (t == "i32") return 32;
            if (t == "i64") return 64;
            return 0;
        };
        int bits = intBits(switchType);
        std::string promoted = newTemp();
        if (bits > 0 && bits < 32) {
            emit(promoted + " = " + std::string(isUnsignedSwitchExpr() ? "zext" : "sext") + " " + switchType + " " + switchReg + " to i32");
        } else if (bits == 64) {
            emit(promoted + " = trunc i64 " + switchReg + " to i32");
        } else {
            // Fallback: treat as i32 already.
            promoted = switchReg;
        }
        switchReg = promoted;
        switchType = "i32";
    }

    std::string endLabel = newLabel("sw.end");

    // Collect case/default labels in source order.
    struct LabelInfo {
        AST::Statement* node;
        bool isDefault;
        long long caseValue;
        LabelInfo() : node(nullptr), isDefault(false), caseValue(0) {}
    };
    std::vector<LabelInfo> labels;
    labels.reserve(8);

    std::function<void(AST::Statement*)> collect;
    collect = [&](AST::Statement* stmt) {
        if (!stmt) return;
        if (auto* c = dynamic_cast<AST::CaseStmt*>(stmt)) {
            long long v = 0;
            if (c->value) {
                evaluateConstantExpr(c->value.get(), v);
            }
            LabelInfo li;
            li.node = c;
            li.isDefault = false;
            li.caseValue = v;
            labels.push_back(li);
            collect(c->body.get());
            return;
        }
        if (auto* d = dynamic_cast<AST::DefaultStmt*>(stmt)) {
            LabelInfo li;
            li.node = d;
            li.isDefault = true;
            li.caseValue = 0;
            labels.push_back(li);
            collect(d->body.get());
            return;
        }
        if (auto* block = dynamic_cast<AST::CompoundStmt*>(stmt)) {
            for (auto& s : block->statements) {
                collect(s.get());
            }
            return;
        }
        if (auto* lab = dynamic_cast<AST::LabelStmt*>(stmt)) {
            collect(lab->body.get());
            return;
        }
        // Other statement kinds don't introduce switch labels.
    };

    collect(node.body.get());

    // Build switch context with concrete labels.
    SwitchContext ctx;
    ctx.switchVar = switchReg;
    ctx.switchType = switchType;
    ctx.endLabel = endLabel;

    // Assign labels (and choose default target).
    std::string defaultTarget = endLabel;
    for (auto& li : labels) {
        std::string lbl = newLabel(li.isDefault ? "sw.default" : "sw.case");
        ctx.labelOrder.push_back(li.node);
        ctx.labelForNode[li.node] = lbl;
        if (li.isDefault) {
            ctx.defaultLabel = lbl;
            defaultTarget = lbl;
        } else {
            ctx.cases.push_back(std::make_pair(li.caseValue, lbl));
        }
    }

    // Push switch context and break target.
    switchStack_.push(ctx);
    breakLabels_.push(endLabel);

    // Emit dispatch (LLVM switch terminator).
    {
        std::stringstream sw;
        sw << "switch " << switchType << " " << switchReg << ", label %" << defaultTarget << " [";
        std::set<long long> seen;
        for (auto& kv : ctx.cases) {
            if (seen.insert(kv.first).second) {
                sw << " " << switchType << " " << kv.first << ", label %" << kv.second;
            }
        }
        sw << " ]";
        emit(sw.str());
    }

    // After a terminator, we must start a new block before defining other blocks.
    // This block is unreachable from the dispatch (switch is a terminator), and just
    // provides a well-formed place to continue emitting subsequent IR.
    std::string afterDispatch = newLabel("sw.after");
    emitLabel(afterDispatch);
    emit("br label %" + endLabel);

    // Emit labeled blocks in source order.
    // Any statements before the first case/default label are unreachable in C switch semantics,
    // so we skip them.
    if (node.body) {
        if (auto* block = dynamic_cast<AST::CompoundStmt*>(node.body.get())) {
            bool started = false;
            for (auto& st : block->statements) {
                if (!st) continue;
                if (!started) {
                    if (dynamic_cast<AST::CaseStmt*>(st.get()) || dynamic_cast<AST::DefaultStmt*>(st.get())) {
                        started = true;
                    } else {
                        continue;
                    }
                }
                st->accept(*this);
            }
        } else {
            // If the body isn't a compound, only emit if it starts with a label.
            if (dynamic_cast<AST::CaseStmt*>(node.body.get()) || dynamic_cast<AST::DefaultStmt*>(node.body.get())) {
                node.body->accept(*this);
            }
        }
    }

    // If control reaches the end of the switch body without a break/return/goto,
    // it implicitly exits the switch.
    if (!blockTerminated_) {
        emit("br label %" + endLabel);
    }

    // End label (code after the switch continues here).
    emitLabel(endLabel);

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
    auto it = ctx.labelForNode.find(&node);
    if (it == ctx.labelForNode.end()) {
        hadError_ = true;
        return;
    }
    const std::string& caseLabel = it->second;

    // Fallthrough to this labeled statement.
    if (!blockTerminated_) {
        emit("br label %" + caseLabel);
    }
    emitLabel(caseLabel);

    if (node.body) {
        node.body->accept(*this);
    }
}

// ============================================================================
// Default Statement
// ============================================================================

void IRGenerator::visit(AST::DefaultStmt& node) {
    if (switchStack_.empty()) {
        hadError_ = true;
        return;
    }

    SwitchContext& ctx = switchStack_.top();
    auto it = ctx.labelForNode.find(&node);
    if (it == ctx.labelForNode.end()) {
        hadError_ = true;
        return;
    }
    const std::string& defaultLabel = it->second;

    if (!blockTerminated_) {
        emit("br label %" + defaultLabel);
    }
    emitLabel(defaultLabel);

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
