#include <codegen/IRGenerator.hpp>

#include <functional>
#include <set>
#include <sstream>
#include <vector>

namespace cc1 {

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

} // namespace cc1
