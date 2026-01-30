#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for non-assignment binary operators.
// FR: Genere l IR pour les binaires non-assignation.
void IRGenerator::emitBinaryRegular(AST::BinaryExpr& node) {
    
    node.left->accept(*this);
    IRValue lhsVal = loadValue(lastValue_);

    node.right->accept(*this);
    IRValue rhsVal = loadValue(lastValue_);

    std::string lhsReg = lhsVal.name;
    std::string lhsType = lhsVal.type;

    std::string rhsReg = rhsVal.name;
    std::string rhsType = rhsVal.type;

    
    // EN: Maps integer LLVM types to bit-width for promotions.
    // FR: Mappe les types entiers LLVM vers la largeur en bits.
    auto getIntSize = [](const std::string& t) -> int {
        if (t == "i8") return 8;
        if (t == "i16") return 16;
        if (t == "i32") return 32;
        if (t == "i64") return 64;
        return 0;
    };

    int lhsSize = getIntSize(lhsType);
    int rhsSize = getIntSize(rhsType);

    
    if (lhsSize > 0 && rhsSize > 0 && lhsSize != rhsSize) {
        if (lhsSize > rhsSize) {
            std::string promoted = newTemp();
            emit(promoted + " = sext " + rhsType + " " + rhsReg + " to " + lhsType);
            rhsReg = promoted;
            rhsType = lhsType;
        } else {
            std::string promoted = newTemp();
            emit(promoted + " = sext " + lhsType + " " + lhsReg + " to " + rhsType);
            lhsReg = promoted;
            lhsType = rhsType;
        }
    }

    std::string result;
    std::string resultType = lhsType;

    if (emitBinaryArithmetic(node, lhsReg, lhsType, rhsReg, rhsType, result, resultType)) {
        lastValue_ = IRValue(result, resultType, false, false);
        return;
    }

    if (emitBinaryComparison(node, rhsVal, lhsReg, lhsType, rhsReg, result, resultType)) {
        lastValue_ = IRValue(result, resultType, false, false);
        return;
    }

    if (node.op == AST::BinaryOp::Comma) {
        
        lastValue_ = IRValue(rhsReg, rhsVal.derefType(), false, false);
        return;
    }

    result = newTemp();
    emit(result + " = add " + lhsType + " " + lhsReg + ", " + rhsReg);
    lastValue_ = IRValue(result, lhsType, false, false);
}

} 
