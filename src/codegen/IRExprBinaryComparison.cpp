#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits comparison ops with integer/pointer ordering and i1->i32 extend.
// FR: Genere les comparaisons avec ordre entier/pointeur et extension i1->i32.
bool IRGenerator::emitBinaryComparison(AST::BinaryExpr& node,
                                      const IRValue& rhsVal,
                                      const std::string& lhsReg,
                                      const std::string& lhsType,
                                      const std::string& rhsReg,
                                      std::string& outResult,
                                      std::string& outResultType) {
    switch (node.op) {
        case AST::BinaryOp::Equal: {
            std::string cmp = newTemp();
            
            std::string rhsValue = rhsReg;
            if (!lhsType.empty() && lhsType.back() == '*' && rhsVal.isConstant) {
                try {
                    if (std::stoll(rhsReg) == 0) {
                        rhsValue = "null";
                    }
                } catch (...) {
                }
            }
            emit(cmp + " = icmp eq " + lhsType + " " + lhsReg + ", " + rhsValue);
            outResult = newTemp();
            emit(outResult + " = zext i1 " + cmp + " to i32");
            outResultType = "i32";
            return true;
        }
        case AST::BinaryOp::NotEqual: {
            std::string cmp = newTemp();
            std::string rhsValue = rhsReg;
            if (!lhsType.empty() && lhsType.back() == '*' && rhsVal.isConstant) {
                try {
                    if (std::stoll(rhsReg) == 0) {
                        rhsValue = "null";
                    }
                } catch (...) {
                }
            }
            emit(cmp + " = icmp ne " + lhsType + " " + lhsReg + ", " + rhsValue);
            outResult = newTemp();
            emit(outResult + " = zext i1 " + cmp + " to i32");
            outResultType = "i32";
            return true;
        }
        case AST::BinaryOp::Less: {
            std::string cmp = newTemp();
            
            if (!lhsType.empty() && lhsType.back() == '*') {
                emit(cmp + " = icmp ult " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(cmp + " = icmp slt " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            outResult = newTemp();
            emit(outResult + " = zext i1 " + cmp + " to i32");
            outResultType = "i32";
            return true;
        }
        case AST::BinaryOp::LessEqual: {
            std::string cmp = newTemp();
            if (!lhsType.empty() && lhsType.back() == '*') {
                emit(cmp + " = icmp ule " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(cmp + " = icmp sle " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            outResult = newTemp();
            emit(outResult + " = zext i1 " + cmp + " to i32");
            outResultType = "i32";
            return true;
        }
        case AST::BinaryOp::Greater: {
            std::string cmp = newTemp();
            if (!lhsType.empty() && lhsType.back() == '*') {
                emit(cmp + " = icmp ugt " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(cmp + " = icmp sgt " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            outResult = newTemp();
            emit(outResult + " = zext i1 " + cmp + " to i32");
            outResultType = "i32";
            return true;
        }
        case AST::BinaryOp::GreaterEqual: {
            std::string cmp = newTemp();
            if (!lhsType.empty() && lhsType.back() == '*') {
                emit(cmp + " = icmp uge " + lhsType + " " + lhsReg + ", " + rhsReg);
            } else {
                emit(cmp + " = icmp sge " + lhsType + " " + lhsReg + ", " + rhsReg);
            }
            outResult = newTemp();
            emit(outResult + " = zext i1 " + cmp + " to i32");
            outResultType = "i32";
            return true;
        }
        default:
            return false;
    }
}

} 
