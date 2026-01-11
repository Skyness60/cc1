#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Array Index
// ============================================================================

void IRGenerator::visit(AST::IndexExpr& node) {
    // Evaluate array expression
    node.array->accept(*this);
    IRValue arrVal = lastValue_;

    // Evaluate index expression
    node.index->accept(*this);
    IRValue idxVal = loadValue(lastValue_);

    // If arrVal is an array value (not pointer), take its address
    if (!arrVal.isPointer) {
        std::string tempAlloca = newTemp();
        emit(tempAlloca + " = alloca " + arrVal.type);
        emit("store " + arrVal.type + " " + arrVal.name + ", " + arrVal.type + "* " + tempAlloca);
        arrVal = IRValue(tempAlloca, arrVal.type + "*", true, false);
    }

    // Determine element type
    std::string elemType;
    if (!arrVal.type.empty() && arrVal.type.back() == '*') {
        elemType = arrVal.derefType();
        if (!elemType.empty() && elemType.front() == '[') {
            // array pointer, element type is inside brackets
            auto xPos = elemType.find('x');
            if (xPos != std::string::npos) {
                elemType = elemType.substr(xPos + 2);
                if (!elemType.empty() && elemType.back() == ']') {
                    elemType.pop_back();
                }
            }
        }
    }

    // Compute element address
    std::string elemPtr = newTemp();
    emit(elemPtr + " = getelementptr inbounds " + arrVal.derefType() + ", " + arrVal.type + " " + arrVal.name +
         ", i32 0, i32 " + idxVal.name);

    lastValue_ = IRValue(elemPtr, elemType + "*", true, false);
}

} // namespace cc1
