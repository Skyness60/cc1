#include <codegen/IRGenerator.hpp>

namespace cc1 {

static int alignTo(int offset, int alignment) {
    if (alignment <= 0) return offset;
    int rem = offset % alignment;
    return rem == 0 ? offset : (offset + (alignment - rem));
}

IRGenerator::StructLayout IRGenerator::computeStructLayout(AST::StructDecl* decl) {
    StructLayout layout;

    // Prefer the preserved StructType if available (keeps bitfield widths).
    if (decl && decl->declaredType) {
        layout = computeStructLayout(decl->declaredType.get());
        layout.llvmType = "%struct." + decl->name + " = type " + layout.llvmType;
        return layout;
    }

    if (decl->isUnion) {
        // Union: all members overlap at offset 0; alignment is max member alignment.
        layout.llvmType = "%struct." + decl->name + " = type { ";
        int maxSize = 1;
        int maxAlign = 1;
        std::string chosenType = "i8";
        int chosenSize = 1;

        if (!decl->members.empty() && decl->members[0] && decl->members[0]->type) {
            AST::Type* firstType = stripQualifiers(decl->members[0]->type.get());
            chosenType = typeToLLVM(firstType);
            chosenSize = std::max(1, getTypeSize(firstType));
            maxAlign = std::max(1, getTypeAlign(firstType));
            maxSize = chosenSize;
        }

        for (size_t i = 0; i < decl->members.size(); ++i) {
            auto& member = decl->members[i];
            AST::Type* memberTypeAst = stripQualifiers(member->type.get());
            std::string memberType = typeToLLVM(memberTypeAst);
            int memberSize = getTypeSize(memberTypeAst);
            int memberAlign = getTypeAlign(memberTypeAst);

            layout.memberIndices[member->name] = 0;
            layout.memberTypes[member->name] = memberType;

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
    } else {
        // Struct: add explicit padding fields to match ABI alignment.
        layout.llvmType = "%struct." + decl->name + " = type { ";
        int offset = 0;
        int maxAlign = 1;
        int fieldIndex = 0;
        bool firstField = true;

        for (size_t i = 0; i < decl->members.size(); ++i) {
            auto& member = decl->members[i];
            std::string memberType = typeToLLVM(member->type.get());
            int memberSize = getTypeSize(member->type.get());
            int memberAlign = getTypeAlign(member->type.get());

            int aligned = alignTo(offset, memberAlign);
            int pad = aligned - offset;
            if (pad > 0) {
                if (!firstField) layout.llvmType += ", ";
                firstField = false;
                layout.llvmType += "[" + std::to_string(pad) + " x i8]";
                offset += pad;
                fieldIndex++;
            }

            if (!firstField) layout.llvmType += ", ";
            firstField = false;
            layout.llvmType += memberType;

            layout.memberIndices[member->name] = fieldIndex;
            layout.memberTypes[member->name] = memberType;

            offset += memberSize;
            maxAlign = std::max(maxAlign, memberAlign);
            fieldIndex++;
        }

        int finalSize = alignTo(offset, maxAlign);
        int tailPad = finalSize - offset;
        if (tailPad > 0) {
            if (!firstField) layout.llvmType += ", ";
            layout.llvmType += "[" + std::to_string(tailPad) + " x i8]";
            fieldIndex++;
        }

        layout.llvmType += " }";
        layout.totalSize = finalSize > 0 ? finalSize : 1;
        layout.alignment = maxAlign;
    }

    return layout;
}

} // namespace cc1
