#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Cast Expression
// ============================================================================

void IRGenerator::visit(AST::CastExpr& node) {
    // Evaluate expression
    node.operand->accept(*this);
    IRValue exprVal = loadValue(lastValue_);

    // Get target type
    std::string targetType = typeToLLVM(node.targetType.get());

    // If types match, no cast needed
    if (exprVal.type == targetType) {
        lastValue_ = exprVal;
        return;
    }

    std::string result = newTemp();

    // Integer casts
    if (exprVal.type[0] == 'i' && targetType[0] == 'i') {
        int fromBits = std::stoi(exprVal.type.substr(1));
        int toBits = std::stoi(targetType.substr(1));

        if (fromBits < toBits) {
            // Sign extend (TODO: handle unsigned)
            emit(result + " = sext " + exprVal.type + " " + exprVal.name + " to " + targetType);
        } else {
            // Truncate
            emit(result + " = trunc " + exprVal.type + " " + exprVal.name + " to " + targetType);
        }
    }
    // Float casts
    else if ((exprVal.type == "float" || exprVal.type == "double") && (targetType == "float" || targetType == "double")) {
        if (exprVal.type == "float" && targetType == "double") {
            emit(result + " = fpext float " + exprVal.name + " to double");
        } else {
            emit(result + " = fptrunc double " + exprVal.name + " to float");
        }
    }
    // Int to float
    else if (exprVal.type[0] == 'i' && (targetType == "float" || targetType == "double")) {
        emit(result + " = sitofp " + exprVal.type + " " + exprVal.name + " to " + targetType);
    }
    // Float to int
    else if ((exprVal.type == "float" || exprVal.type == "double") && targetType[0] == 'i') {
        emit(result + " = fptosi " + exprVal.type + " " + exprVal.name + " to " + targetType);
    }
    // Pointer casts
    else if (exprVal.type.back() == '*' && targetType.back() == '*') {
        emit(result + " = bitcast " + exprVal.type + " " + exprVal.name + " to " + targetType);
    } else {
        // Unsupported cast - just use original
        lastValue_ = exprVal;
        return;
    }

    lastValue_ = IRValue(result, targetType, false, false);
}

// ============================================================================
// Sizeof Expression
// ============================================================================

void IRGenerator::visit(AST::SizeofExpr& node) {
    int size = 0;

    if (node.operand) {
        // sizeof expression
        if (node.operand->resolvedType) {
            size = getTypeSize(node.operand->resolvedType.get());
        }
    } else if (node.targetType) {
        // sizeof(type)
        size = getTypeSize(node.targetType.get());
    }

    lastValue_ = IRValue(std::to_string(size), "i32", false, true);
}

} // namespace cc1
