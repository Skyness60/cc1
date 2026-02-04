#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Aligns an offset up to the requested alignment.
// FR: Aligne un offset vers le haut selon l alignement.
static int alignTo(int offset, int alignment) {
    if (alignment <= 0) return offset;
    int rem = offset % alignment;
    return rem == 0 ? offset : (offset + (alignment - rem));
}

// EN: Computes union layout by selecting max-size/alignment member.
// FR: Calcule le layout union via membre max taille/alignement.
IRGenerator::StructLayout IRGenerator::computeStructLayoutUnion(AST::StructType* type) {
    StructLayout layout;

    layout.llvmType = "{ ";
    int maxSize = 1;
    int maxAlign = 1;
    std::string chosenType = "i8";
    int chosenSize = 1;

    if (!type->members.empty() && type->members[0].type) {
        AST::Type* firstType = stripQualifiers(type->members[0].type.get());
        chosenType = typeToLLVM(firstType);
        chosenSize = std::max(1, getTypeSize(firstType));
        maxAlign = std::max(1, getTypeAlign(firstType));
        maxSize = chosenSize;
    }

    for (size_t i = 0; i < type->members.size(); ++i) {
        auto& member = type->members[i];
        AST::Type* memberTypeAst = stripQualifiers(member.type.get());
        std::string memberType = typeToLLVM(memberTypeAst);
        int memberSize = getTypeSize(memberTypeAst);
        int memberAlign = getTypeAlign(memberTypeAst);

        layout.memberIndices[member.name] = 0;
        // In unions, all members share the same storage at offset 0.
        // Store the member's declared type initially, but we'll update after choosing the container type.
        layout.memberTypes[member.name] = memberType;

        maxSize = std::max(maxSize, memberSize);
        if (memberAlign > maxAlign) {
            maxAlign = memberAlign;
            chosenType = memberType;
            chosenSize = memberSize;
        } else if (memberAlign == maxAlign && memberSize > chosenSize) {
            chosenType = memberType;
            chosenSize = memberSize;
        }
    }

    // For union members, if the declared type doesn't match the chosen container type,
    // we need to store the proper type. For nested structs in unions that are wrapped,
    // we should store the base type to use in member access expressions.
    int desiredSize = alignTo(maxSize > 0 ? maxSize : 1, maxAlign);
    
    // For the LLVM representation, use a byte array large enough to hold any member.
    // This avoids issues with nested structs in unions by representing the union as
    // a simple byte buffer that all members can overlay.
    if (desiredSize <= 1) {
        layout.llvmType += "i8";
    } else {
        layout.llvmType += "[" + std::to_string(desiredSize) + " x i8]";
    }
    layout.llvmType += " }";
    layout.totalSize = desiredSize;
    layout.alignment = maxAlign;
    layout.isUnion = true;

    return layout;
}

} 
