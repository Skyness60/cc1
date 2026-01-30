#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for array/pointer indexing with GEP.
// FR: Genere l IR pour l indexation tableau/pointeur via GEP.
void IRGenerator::visit(AST::IndexExpr& node) {
    node.array->accept(*this);
    IRValue arrVal = lastValue_;

    
    node.index->accept(*this);
    IRValue idxVal = loadValue(lastValue_);

    
    if (arrVal.isPointer) {
        std::string dt = arrVal.derefType();
        
        if (!dt.empty() && dt.back() == '*' && (dt.empty() || dt.front() != '[')) {
            arrVal = loadValue(arrVal);
        }
    }

    
    if (arrVal.type.empty() || arrVal.type.back() != '*') {
        std::string tempAlloca = newTemp();
        emit(tempAlloca + " = alloca " + arrVal.type);
        emit("store " + arrVal.type + " " + arrVal.name + ", " + arrVal.type + "* " + tempAlloca);
        arrVal = IRValue(tempAlloca, arrVal.type + "*", true, false);
    }

    
    std::string elemType;
    if (!arrVal.type.empty() && arrVal.type.back() == '*') {
        elemType = arrVal.derefType();
        if (!elemType.empty() && elemType.front() == '[') {
            
            auto xPos = elemType.find('x');
            if (xPos != std::string::npos) {
                elemType = elemType.substr(xPos + 2);
                if (!elemType.empty() && elemType.back() == ']') {
                    elemType.pop_back();
                }
            }
        }
    }

    
    std::string elemPtr = newTemp();
    std::string idxType = is64bit_ ? "i64" : "i32";
    std::string idxReg = idxVal.name;
    if (idxType != idxVal.type) {
        std::string castIdx = newTemp();
        emit(castIdx + " = sext " + idxVal.type + " " + idxVal.name + " to " + idxType);
        idxReg = castIdx;
    }

    if (!arrVal.derefType().empty() && arrVal.derefType().front() == '[') {
        emit(elemPtr + " = getelementptr inbounds " + arrVal.derefType() + ", " + arrVal.type + " " + arrVal.name +
             ", " + idxType + " 0, " + idxType + " " + idxReg);
    } else {
        emit(elemPtr + " = getelementptr inbounds " + arrVal.derefType() + ", " + arrVal.type + " " + arrVal.name +
             ", " + idxType + " " + idxReg);
    }

    lastValue_ = IRValue(elemPtr, elemType + "*", true, false);
}

} 
