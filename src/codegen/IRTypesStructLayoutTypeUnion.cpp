#include <codegen/IRGenerator.hpp>

namespace cc1 {

static int alignTo(int offset, int alignment) {
    if (alignment <= 0) return offset;
    int rem = offset % alignment;
    return rem == 0 ? offset : (offset + (alignment - rem));
}

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

    int desiredSize = alignTo(maxSize > 0 ? maxSize : 1, maxAlign);
    layout.llvmType += chosenType;
    int pad = desiredSize - chosenSize;
    if (pad > 0) {
        layout.llvmType += ", [" + std::to_string(pad) + " x i8]";
    }
    layout.llvmType += " }";
    layout.totalSize = desiredSize;
    layout.alignment = maxAlign;

    return layout;
}

} // namespace cc1
