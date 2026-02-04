#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for struct/union member access using GEP.
// FR: Genere l IR pour l acces membre via GEP.
void IRGenerator::visit(AST::MemberExpr& node) {
    
    node.object->accept(*this);
    IRValue baseVal = lastValue_;

    
    bool isPtr = baseVal.type.size() > 1 && baseVal.type.back() == '*';

    
    std::string structType = baseVal.derefType();

    
    if (isPtr) {
        
        
    } else {
        
        std::string tempAlloca = newTemp();
        emit(tempAlloca + " = alloca " + baseVal.type);
        emit("store " + baseVal.type + " " + baseVal.name + ", " + baseVal.type + "* " + tempAlloca);
        baseVal = IRValue(tempAlloca, baseVal.type + "*", true, false);
        structType = baseVal.derefType();
        isPtr = true;
    }

    
    std::string structName;
    if (structType.find("%struct.") == 0) {
        structName = structType.substr(8);
    }

    const StructLayout* layout = nullptr;
    if (!structName.empty()) {
        layout = getStructLayout(structName);
    }

    
    int memberIndex = -1;
    std::string memberLLVMType = "i32";

    if (layout) {
        auto it = layout->memberIndices.find(node.member);
        if (it != layout->memberIndices.end()) {
            memberIndex = it->second;
            memberLLVMType = layout->memberTypes.at(node.member);
        }
    } else {
        memberIndex = findMemberIndexInInlineStruct(structType, node.member);
        memberLLVMType = extractFieldTypeFromInlineStruct(structType, memberIndex);
    }

    if (memberIndex < 0) {
        hadError_ = true;
        lastValue_ = IRValue("0", "i32", false, true);
        return;
    }

    
    std::string memberPtr = newTemp();
    emit(memberPtr + " = getelementptr inbounds " + structType + ", " + structType + "* " + baseVal.name +
         ", i32 0, i32 " + std::to_string(memberIndex));

    // For unions, the LLVM representation uses a byte array, so after GEP we get
    // a pointer to the byte array. We need to cast it to the proper member type pointer.
    std::string finalMemberPtr = memberPtr;
    std::string finalMemberType = memberLLVMType;
    
    if (layout && layout->isUnion) {
        // The GEP gives us a pointer to the byte array. Cast it to pointer-to-member-type.
        finalMemberPtr = newTemp();
        std::string gepResultType = "[" + std::to_string(layout->totalSize) + " x i8]*";
        if (layout->totalSize <= 1) {
            gepResultType = "i8*";
        }
        emit(finalMemberPtr + " = bitcast " + gepResultType + " " + memberPtr + " to " + memberLLVMType + "*");
    }

    IRValue memberVal(finalMemberPtr, memberLLVMType + "*", true, false);

    
    if (layout && layout->bitfieldWidths.count(node.member)) {
        memberVal.isBitfieldRef = true;
        memberVal.bitfieldStorageType = memberLLVMType;
        memberVal.bitfieldOffset = layout->bitfieldOffsets.at(node.member);
        memberVal.bitfieldWidth = layout->bitfieldWidths.at(node.member);
        memberVal.bitfieldIsUnsigned = layout->bitfieldIsUnsigned.at(node.member);
    }

    lastValue_ = memberVal;
}

} 
