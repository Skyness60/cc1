#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Value Management (load)
// ============================================================================

IRValue IRGenerator::loadValue(const IRValue& val) {
    if (val.isBitfieldRef) {
        // Load and extract the bitfield value from its storage unit.
        std::string storagePtr = val.name;
        std::string storageTy = val.bitfieldStorageType.empty() ? val.derefType() : val.bitfieldStorageType;

        std::string loaded = newTemp();
        emit(loaded + " = load " + storageTy + ", " + storageTy + "* " + storagePtr);

        std::string shifted = loaded;
        if (val.bitfieldOffset != 0) {
            shifted = newTemp();
            emit(shifted + " = lshr " + storageTy + " " + loaded + ", " + std::to_string(val.bitfieldOffset));
        }

        long long mask = 0;
        if (val.bitfieldWidth >= 64) {
            mask = -1LL;
        } else if (val.bitfieldWidth <= 0) {
            mask = 0;
        } else {
            mask = (1LL << val.bitfieldWidth) - 1;
        }

        std::string masked = newTemp();
        emit(masked + " = and " + storageTy + " " + shifted + ", " + std::to_string(mask));

        // Normalize to i32 as the expression value.
        std::string asI32;
        if (storageTy == "i32") {
            asI32 = masked;
        } else {
            asI32 = newTemp();
            if (storageTy == "i64") {
                emit(asI32 + " = trunc i64 " + masked + " to i32");
            } else {
                emit(asI32 + " = zext " + storageTy + " " + masked + " to i32");
            }
        }

        if (!val.bitfieldIsUnsigned && val.bitfieldWidth > 0 && val.bitfieldWidth < 32) {
            int sh = 32 - val.bitfieldWidth;
            std::string shl = newTemp();
            emit(shl + " = shl i32 " + asI32 + ", " + std::to_string(sh));
            std::string ashr = newTemp();
            emit(ashr + " = ashr i32 " + shl + ", " + std::to_string(sh));
            return IRValue(ashr, "i32", false, false);
        }

        return IRValue(asI32, "i32", false, false);
    }

    if (!val.isPointer || val.isConstant) {
        return val;
    }

    std::string temp = newTemp();
    std::string loadType = val.derefType();

    // Emit: %temp = load type, type* ptr
    emit(temp + " = load " + loadType + ", " + val.type + " " + val.name);

    IRValue result;
    result.name = temp;
    result.type = loadType;
    result.isPointer = (!loadType.empty() && loadType.back() == '*');
    result.isConstant = false;

    return result;
}

} // namespace cc1
