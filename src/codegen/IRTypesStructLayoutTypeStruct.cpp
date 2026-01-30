#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Aligns an offset up to the requested alignment.
// FR: Aligne un offset vers le haut selon l alignement.
static int alignTo(int offset, int alignment) {
    if (alignment <= 0) return offset;
    int rem = offset % alignment;
    return rem == 0 ? offset : (offset + (alignment - rem));
}

// EN: Computes struct layout including padding and bitfields.
// FR: Calcule le layout struct avec padding et bitfields.
IRGenerator::StructLayout IRGenerator::computeStructLayoutStruct(AST::StructType* type) {
    StructLayout layout;

    layout.llvmType = "{ ";
    int offset = 0;
    int maxAlign = 1;
    int fieldIndex = 0;
    bool firstField = true;

    
    int currentUnitSize = 0;
    int currentUnitAlign = 1;
    int currentUnitBitsTotal = 0;
    int currentUnitBitsUsed = 0;
    int currentUnitFieldIndex = -1;
    std::string currentUnitLLVMType;

    // EN: Flushes current bitfield storage unit into the layout.
    // FR: Vide l unite de stockage des bitfields dans le layout.
    auto flushUnit = [&]() {
        if (currentUnitBitsUsed > 0 && currentUnitSize > 0) {
            offset += currentUnitSize;
        }
        currentUnitSize = 0;
        currentUnitAlign = 1;
        currentUnitBitsTotal = 0;
        currentUnitBitsUsed = 0;
        currentUnitFieldIndex = -1;
        currentUnitLLVMType.clear();
    };

    // EN: Inserts padding to satisfy alignment before a field.
    // FR: Insere du padding pour respecter l alignement.
    auto ensurePaddingTo = [&](int alignment) {
        int aligned = alignTo(offset, alignment);
        int pad = aligned - offset;
        if (pad > 0) {
            if (!firstField) layout.llvmType += ", ";
            firstField = false;
            layout.llvmType += "[" + std::to_string(pad) + " x i8]";
            offset += pad;
            fieldIndex++;
        }
    };

    // EN: Opens a storage unit for bitfields if compatible.
    // FR: Ouvre une unite de stockage pour bitfields si compatible.
    auto openStorageUnitIfNeeded = [&](int unitSize, int unitAlign, const std::string& unitLLVMType) {
        if (currentUnitBitsTotal == 0 || currentUnitSize != unitSize || currentUnitAlign != unitAlign) {
            flushUnit();
            ensurePaddingTo(unitAlign);

            if (!firstField) layout.llvmType += ", ";
            firstField = false;
            layout.llvmType += unitLLVMType;
            currentUnitFieldIndex = fieldIndex;
            fieldIndex++;

            currentUnitSize = unitSize;
            currentUnitAlign = unitAlign;
            currentUnitBitsTotal = unitSize * 8;
            currentUnitBitsUsed = 0;
            currentUnitLLVMType = unitLLVMType;
            maxAlign = std::max(maxAlign, unitAlign);
        }
    };

    for (size_t i = 0; i < type->members.size(); ++i) {
        const auto& member = type->members[i];

        if (member.isBitfield()) {
            int unitSize = getTypeSize(member.type.get());
            int unitAlign = getTypeAlign(member.type.get());
            if (unitSize <= 0) unitSize = 1;
            if (unitAlign <= 0) unitAlign = 1;
            std::string unitLLVMType;
            if (unitSize == 1) unitLLVMType = "i8";
            else if (unitSize == 2) unitLLVMType = "i16";
            else if (unitSize == 4) unitLLVMType = "i32";
            else unitLLVMType = "i64";

            
            if (member.bitWidth == 0) {
                flushUnit();
                ensurePaddingTo(unitAlign);
                maxAlign = std::max(maxAlign, unitAlign);
                continue;
            }

            
            openStorageUnitIfNeeded(unitSize, unitAlign, unitLLVMType);

            
            if (currentUnitBitsUsed + member.bitWidth > currentUnitBitsTotal) {
                flushUnit();
                openStorageUnitIfNeeded(unitSize, unitAlign, unitLLVMType);
            }

            if (!member.name.empty()) {
                layout.memberIndices[member.name] = currentUnitFieldIndex;
                layout.memberTypes[member.name] = currentUnitLLVMType;
                layout.bitfieldOffsets[member.name] = currentUnitBitsUsed;
                layout.bitfieldWidths[member.name] = member.bitWidth;

                bool isUnsigned = false;
                if (auto* prim = dynamic_cast<AST::PrimitiveType*>(stripQualifiers(member.type.get()))) {
                    switch (prim->kind) {
                        case AST::PrimitiveKind::UnsignedChar:
                        case AST::PrimitiveKind::UnsignedShort:
                        case AST::PrimitiveKind::UnsignedInt:
                        case AST::PrimitiveKind::UnsignedLong:
                        case AST::PrimitiveKind::UnsignedLongLong:
                            isUnsigned = true;
                            break;
                        default:
                            isUnsigned = false;
                            break;
                    }
                }
                layout.bitfieldIsUnsigned[member.name] = isUnsigned;
            }

            currentUnitBitsUsed += member.bitWidth;
            if (currentUnitBitsUsed == currentUnitBitsTotal) {
                flushUnit();
            }
            continue;
        }

        
        flushUnit();

        std::string memberType = typeToLLVM(member.type.get());
        int memberSize = getTypeSize(member.type.get());
        int memberAlign = getTypeAlign(member.type.get());

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

        layout.memberIndices[member.name] = fieldIndex;
        layout.memberTypes[member.name] = memberType;

        offset += memberSize;
        maxAlign = std::max(maxAlign, memberAlign);
        fieldIndex++;
    }

    
    flushUnit();

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

    return layout;
}

} 
