#include <codegen/IRGenerator.hpp>

namespace cc1 {

bool IRGenerator::emitBinaryArithmetic(AST::BinaryExpr& node,
                                      const std::string& lhsReg,
                                      const std::string& lhsType,
                                      const std::string& rhsReg,
                                      const std::string& rhsType,
                                      std::string& outResult,
                                      std::string& outResultType) {
    // Helper to check if type is floating point
    auto isFloatType = [](const std::string& t) -> bool {
        return t == "float" || t == "double";
    };

    switch (node.op) {
        case AST::BinaryOp::Add: {
            outResult = newTemp();
            // Handle pointer arithmetic: ptr + int or int + ptr
            if (!lhsType.empty() && lhsType.back() == '*') {
                std::string elemType = lhsType.substr(0, lhsType.size() - 1);
                emit(outResult + " = getelementptr inbounds " + elemType + ", " + lhsType + " " + lhsReg + ", i32 " + rhsReg);
                outResultType = lhsType;
            } else if (!rhsType.empty() && rhsType.back() == '*') {
                std::string elemType = rhsType.substr(0, rhsType.size() - 1);
                emit(outResult + " = getelementptr inbounds " + elemType + ", " + rhsType + " " + rhsReg + ", i32 " + lhsReg);
                outResultType = rhsType;
            } else if (isFloatType(lhsType)) {
                emit(outResult + " = fadd " + lhsType + " " + lhsReg + ", " + rhsReg);
                outResultType = lhsType;
            } else {
                emit(outResult + " = add " + lhsType + " " + lhsReg + ", " + rhsReg);
                outResultType = lhsType;
            }
            return true;
        }
        case AST::BinaryOp::Sub: {
            outResult = newTemp();
            // Handle pointer arithmetic: ptr - int or ptr - ptr
            if (!lhsType.empty() && lhsType.back() == '*') {
                if (!rhsType.empty() && rhsType.back() == '*') {
                    std::string lhsInt = newTemp();
                    std::string rhsInt = newTemp();
                    emit(lhsInt + " = ptrtoint " + lhsType + " " + lhsReg + " to i32");
                    emit(rhsInt + " = ptrtoint " + rhsType + " " + rhsReg + " to i32");
                    emit(outResult + " = sub i32 " + lhsInt + ", " + rhsInt);
                    outResultType = "i32";
                } else {
                    std::string elemType = lhsType.substr(0, lhsType.size() - 1);
                    std::string negIdx = newTemp();
                    emit(negIdx + " = sub i32 0, " + rhsReg);
                    emit(outResult + " = getelementptr inbounds " + elemType + ", " + lhsType + " " + lhsReg + ", i32 " + negIdx);
                    outResultType = lhsType;
                }
            } else if (isFloatType(lhsType)) {
                emit(outResult + " = fsub " + lhsType + " " + lhsReg + ", " + rhsReg);
                outResultType = lhsType;
            } else {
                emit(outResult + " = sub " + lhsType + " " + lhsReg + ", " + rhsReg);
                outResultType = lhsType;
            }
            return true;
        }
        case AST::BinaryOp::Mul:
            outResult = newTemp();
            if (isFloatType(lhsType)) {
                emit(outResult + " = fmul " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(outResult + " = mul " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            outResultType = lhsType;
            return true;
        case AST::BinaryOp::Div:
            outResult = newTemp();
            if (isFloatType(lhsType)) {
                emit(outResult + " = fdiv " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(outResult + " = sdiv " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            outResultType = lhsType;
            return true;
        case AST::BinaryOp::Mod:
            outResult = newTemp();
            if (isFloatType(lhsType)) {
                emit(outResult + " = frem " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(outResult + " = srem " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            outResultType = lhsType;
            return true;
        case AST::BinaryOp::BitwiseAnd:
            outResult = newTemp();
            emit(outResult + " = and " + lhsType + " " + lhsReg + ", " + rhsReg);
            outResultType = lhsType;
            return true;
        case AST::BinaryOp::BitwiseOr:
            outResult = newTemp();
            emit(outResult + " = or " + lhsType + " " + lhsReg + ", " + rhsReg);
            outResultType = lhsType;
            return true;
        case AST::BinaryOp::BitwiseXor:
            outResult = newTemp();
            emit(outResult + " = xor " + lhsType + " " + lhsReg + ", " + rhsReg);
            outResultType = lhsType;
            return true;
        case AST::BinaryOp::LeftShift:
            outResult = newTemp();
            emit(outResult + " = shl " + lhsType + " " + lhsReg + ", " + rhsReg);
            outResultType = lhsType;
            return true;
        case AST::BinaryOp::RightShift:
            outResult = newTemp();
            emit(outResult + " = ashr " + lhsType + " " + lhsReg + ", " + rhsReg);
            outResultType = lhsType;
            return true;
        default:
            return false;
    }
}

} // namespace cc1
