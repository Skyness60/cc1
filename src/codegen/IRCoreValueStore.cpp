#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Stores an IRValue into a pointer, handling bitfields and casts.
// FR: Stocke une IRValue dans un pointeur, avec bitfields et casts.
IRValue IRGenerator::storeValue(const IRValue& val, const IRValue& ptr) {
    if (ptr.isBitfieldRef) {
        
        std::string storagePtr = ptr.name;
        std::string storageTy = ptr.bitfieldStorageType.empty() ? ptr.derefType() : ptr.bitfieldStorageType;

        std::string oldV = newTemp();
        emit(oldV + " = load " + storageTy + ", " + storageTy + "* " + storagePtr);

        
        std::string rhsReg = val.name;
        std::string rhsTy = val.type;
        if (val.isPointer && !val.isConstant) {
            rhsReg = newTemp();
            rhsTy = val.derefType();
            emit(rhsReg + " = load " + rhsTy + ", " + val.type + " " + val.name);
        }
        if (rhsTy != "i32") {
            std::string tmp = newTemp();
            if (!rhsTy.empty() && rhsTy.back() == '*') {
                
                emit(tmp + " = ptrtoint " + rhsTy + " " + rhsReg + " to i32");
            } else {
                emit(tmp + " = trunc " + rhsTy + " " + rhsReg + " to i32");
            }
            rhsReg = tmp;
            rhsTy = "i32";
        }

        
        std::string rhsStorage = newTemp();
        if (storageTy == "i64") {
            emit(rhsStorage + " = zext i32 " + rhsReg + " to i64");
        } else if (storageTy == "i32") {
            rhsStorage = rhsReg;
        } else {
            emit(rhsStorage + " = trunc i32 " + rhsReg + " to " + storageTy);
        }

        long long lowMask = 0;
        if (ptr.bitfieldWidth >= 64) {
            lowMask = -1LL;
        } else if (ptr.bitfieldWidth <= 0) {
            lowMask = 0;
        } else {
            lowMask = (1LL << ptr.bitfieldWidth) - 1;
        }
        std::string masked = newTemp();
        emit(masked + " = and " + storageTy + " " + rhsStorage + ", " + std::to_string(lowMask));

        std::string shifted = masked;
        if (ptr.bitfieldOffset != 0) {
            shifted = newTemp();
            emit(shifted + " = shl " + storageTy + " " + masked + ", " + std::to_string(ptr.bitfieldOffset));
        }

        
        long long fieldMask = 0;
        if (ptr.bitfieldWidth >= 64) {
            fieldMask = -1LL;
        } else if (ptr.bitfieldWidth <= 0) {
            fieldMask = 0;
        } else {
            fieldMask = ((1LL << ptr.bitfieldWidth) - 1) << ptr.bitfieldOffset;
        }
        std::string cleared = newTemp();
        emit(cleared + " = and " + storageTy + " " + oldV + ", " + std::to_string(~fieldMask));

        std::string newV = newTemp();
        emit(newV + " = or " + storageTy + " " + cleared + ", " + shifted);
        emit("store " + storageTy + " " + newV + ", " + storageTy + "* " + storagePtr);

        return val;
    }

    std::string valueType = val.type;
    std::string ptrType = ptr.type;

    
    std::string destType = ptr.derefType();
    std::string srcReg = val.name;
    std::string srcType = valueType;

    if (srcType != destType) {
        // EN: Maps integer LLVM types to bit-width for casts.
        // FR: Mappe les types entiers LLVM vers la largeur en bits.
        auto intBits = [](const std::string& t) -> int {
            if (t == "i1") return 1;
            if (t == "i8") return 8;
            if (t == "i16") return 16;
            if (t == "i32") return 32;
            if (t == "i64") return 64;
            return 0;
        };

        int srcInt = intBits(srcType);
        int dstInt = intBits(destType);

        
        if (srcInt > 0 && dstInt > 0) {
            std::string casted = newTemp();
            if (dstInt < srcInt) {
                emit(casted + " = trunc " + srcType + " " + srcReg + " to " + destType);
            } else if (dstInt > srcInt) {
                emit(casted + " = sext " + srcType + " " + srcReg + " to " + destType);
            } else {
                emit(casted + " = bitcast " + srcType + " " + srcReg + " to " + destType);
            }
            srcReg = casted;
            srcType = destType;
        }
        
        else if (!srcType.empty() && srcType.back() == '*' && !destType.empty() && destType.back() == '*') {
            std::string casted = newTemp();
            emit(casted + " = bitcast " + srcType + " " + srcReg + " to " + destType);
            srcReg = casted;
            srcType = destType;
        }
        
        else if (srcInt > 0 && !destType.empty() && destType.back() == '*') {
            std::string casted = newTemp();
            emit(casted + " = inttoptr " + srcType + " " + srcReg + " to " + destType);
            srcReg = casted;
            srcType = destType;
        }
        
        else if (!srcType.empty() && srcType.back() == '*' && dstInt > 0) {
            std::string casted = newTemp();
            emit(casted + " = ptrtoint " + srcType + " " + srcReg + " to " + destType);
            srcReg = casted;
            srcType = destType;
        }
        
        else if ((srcType == "float" || srcType == "double") && (destType == "float" || destType == "double")) {
            std::string casted = newTemp();
            if (srcType == "float" && destType == "double") {
                emit(casted + " = fpext float " + srcReg + " to double");
            } else if (srcType == "double" && destType == "float") {
                emit(casted + " = fptrunc double " + srcReg + " to float");
            } else {
                emit(casted + " = bitcast " + srcType + " " + srcReg + " to " + destType);
            }
            srcReg = casted;
            srcType = destType;
        }
    }

    emit("store " + srcType + " " + srcReg + ", " + ptrType + " " + ptr.name);

    return val;
}

} 
