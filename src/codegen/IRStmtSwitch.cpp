#include <codegen/IRGenerator.hpp>

#include <functional>
#include <set>
#include <sstream>
#include <vector>

namespace cc1 {

// EN: Emits IR for switch statements, collecting case labels and dispatch.
// FR: Genere l IR pour switch, collecte les cases et le dispatch.
void IRGenerator::visit(AST::SwitchStmt& node) {
    
    node.expression->accept(*this);
    IRValue switchLoaded = loadValue(lastValue_);

    // EN: Checks if the switch expression is unsigned for promotions.
    // FR: Verifie si l expression switch est non signee pour promotions.
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

    
    std::string switchReg = switchLoaded.name;
    std::string switchType = switchLoaded.type;
    if (switchType != "i32") {
        // EN: Maps integer LLVM types to bit-width for promotions.
        // FR: Mappe les types entiers LLVM vers la largeur en bits.
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
            
            promoted = switchReg;
        }
        switchReg = promoted;
        switchType = "i32";
    }

    std::string endLabel = newLabel("sw.end");

    
    struct LabelInfo {
        AST::Statement* node;
        bool isDefault;
        long long caseValue;
        LabelInfo() : node(nullptr), isDefault(false), caseValue(0) {}
    };
    std::vector<LabelInfo> labels;
    labels.reserve(8);

    std::function<void(AST::Statement*)> collect;
    // EN: Walks the switch body to collect case/default labels.
    // FR: Parcourt le corps pour collecter les labels case/default.
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
        
    };

    collect(node.body.get());

    
    SwitchContext ctx;
    ctx.switchVar = switchReg;
    ctx.switchType = switchType;
    ctx.endLabel = endLabel;

    
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

    
    switchStack_.push(ctx);
    breakLabels_.push(endLabel);

    
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

    
    
    
    std::string afterDispatch = newLabel("sw.after");
    emitLabel(afterDispatch);
    emit("br label %" + endLabel);

    
    
    
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
            
            if (dynamic_cast<AST::CaseStmt*>(node.body.get()) || dynamic_cast<AST::DefaultStmt*>(node.body.get())) {
                node.body->accept(*this);
            }
        }
    }

    
    
    if (!blockTerminated_) {
        emit("br label %" + endLabel);
    }

    
    emitLabel(endLabel);

    breakLabels_.pop();
    switchStack_.pop();
}

} 
