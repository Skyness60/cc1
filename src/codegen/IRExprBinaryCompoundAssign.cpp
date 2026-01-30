#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for compound assignments like +=, -=, etc.
// FR: Genere l IR pour les assignations composees (+=, -=, etc.).
bool IRGenerator::emitBinaryCompoundAssign(AST::BinaryExpr& node) {
    if (node.op < AST::BinaryOp::AddAssign || node.op > AST::BinaryOp::RightShiftAssign) {
        return false;
    }

    
    node.left->accept(*this);
    IRValue lhsPtr = lastValue_;

    
    IRValue lhsLoaded = loadValue(lhsPtr);
    std::string lhsReg = lhsLoaded.name;
    std::string lhsType = lhsLoaded.type;

    
    node.right->accept(*this);
    IRValue rhsVal = loadValue(lastValue_);
    std::string rhsReg = rhsVal.name;

    
    if (rhsVal.type != lhsType) {
        // EN: Maps integer LLVM types to bit-width for promotions.
        // FR: Mappe les types entiers LLVM vers la largeur en bits.
        auto getIntSize = [](const std::string& t) -> int {
            if (t == "i8") return 8;
            if (t == "i16") return 16;
            if (t == "i32") return 32;
            if (t == "i64") return 64;
            return 0;
        };
        int rhsSize = getIntSize(rhsVal.type);
        int lhsSize = getIntSize(lhsType);
        if (rhsSize > 0 && lhsSize > 0 && rhsSize != lhsSize) {
            std::string promoted = newTemp();
            if (rhsSize > lhsSize) {
                emit(promoted + " = trunc " + rhsVal.type + " " + rhsReg + " to " + lhsType);
            } else {
                emit(promoted + " = sext " + rhsVal.type + " " + rhsReg + " to " + lhsType);
            }
            rhsReg = promoted;
        }
    }

    
    std::string resultReg = newTemp();
    std::string opName;
    switch (node.op) {
        case AST::BinaryOp::AddAssign: opName = "add"; break;
        case AST::BinaryOp::SubAssign: opName = "sub"; break;
        case AST::BinaryOp::MulAssign: opName = "mul"; break;
        case AST::BinaryOp::DivAssign: opName = "sdiv"; break;
        case AST::BinaryOp::ModAssign: opName = "srem"; break;
        case AST::BinaryOp::AndAssign: opName = "and"; break;
        case AST::BinaryOp::OrAssign: opName = "or"; break;
        case AST::BinaryOp::XorAssign: opName = "xor"; break;
        case AST::BinaryOp::LeftShiftAssign: opName = "shl"; break;
        case AST::BinaryOp::RightShiftAssign: opName = "ashr"; break;
        default: opName = "add"; break;
    }
    emit(resultReg + " = " + opName + " " + lhsType + " " + lhsReg + ", " + rhsReg);

    
    storeValue(IRValue(resultReg, lhsType, false, false), lhsPtr);

    lastValue_ = IRValue(resultReg, lhsType, false, false);
    return true;
}

} 
