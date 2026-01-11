#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Member Access
// ============================================================================

void IRGenerator::visit(AST::MemberExpr& node) {
    // Evaluate base expression
    node.object->accept(*this);
    IRValue baseVal = lastValue_;

    // If base is a pointer, load it to get struct value
    bool isPtr = baseVal.type.size() > 1 && baseVal.type.back() == '*';

    // Determine struct type
    std::string structType = baseVal.derefType();

    // Handle pointer to struct
    if (isPtr) {
        // baseVal is pointer to struct, use it directly
        // structType already is dereferenced type
    } else {
        // baseVal is value, take address by storing to temp
        std::string tempAlloca = newTemp();
        emit(tempAlloca + " = alloca " + baseVal.type);
        emit("store " + baseVal.type + " " + baseVal.name + ", " + baseVal.type + "* " + tempAlloca);
        baseVal = IRValue(tempAlloca, baseVal.type + "*", true, false);
        structType = baseVal.derefType();
        isPtr = true;
    }

    // Look up struct layout
    std::string structName;
    if (structType.find("%struct.") == 0) {
        structName = structType.substr(8);
    }

    const StructLayout* layout = nullptr;
    if (!structName.empty()) {
        layout = getStructLayout(structName);
    }

    // Fallback: try inline struct member map if layout not found
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

    // Compute member address
    std::string memberPtr = newTemp();
    emit(memberPtr + " = getelementptr inbounds " + structType + ", " + structType + "* " + baseVal.name +
         ", i32 0, i32 " + std::to_string(memberIndex));

    IRValue memberVal(memberPtr, memberLLVMType + "*", true, false);

    // Bitfield support: turn into a bitfield ref if needed.
    if (layout && layout->bitfieldWidths.count(node.member)) {
        memberVal.isBitfieldRef = true;
        memberVal.bitfieldStorageType = memberLLVMType;
        memberVal.bitfieldOffset = layout->bitfieldOffsets.at(node.member);
        memberVal.bitfieldWidth = layout->bitfieldWidths.at(node.member);
        memberVal.bitfieldIsUnsigned = layout->bitfieldIsUnsigned.at(node.member);
    }

    lastValue_ = memberVal;
}

} // namespace cc1
