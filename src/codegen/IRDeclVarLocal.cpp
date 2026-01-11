#include <codegen/IRGenerator.hpp>

#include <iomanip>
#include <sstream>

namespace cc1 {

void IRGenerator::emitLocalVarDecl(AST::VarDecl& node, const std::string& llvmType) {
    // Local variable - allocate on stack
    // BUT: static local variables are like globals with unique names
    if (node.storageClass == AST::StorageClass::Static) {
        // Static local variable - emit as global with unique name
        std::string globalName = "@" + currentFunction_->name + "." + node.name + "." + std::to_string(staticVarCounter_++);

        std::string initValue;
        if (node.initializer) {
            if (llvmType == "float" || llvmType == "double") {
                double fpVal;
                if (evaluateConstantFloatExpr(node.initializer.get(), fpVal)) {
                    std::ostringstream oss;
                    if (llvmType == "float") {
                        initValue = formatLLVMFloatConstant(static_cast<float>(fpVal));
                    } else {
                        oss.setf(std::ios::scientific);
                        oss << std::setprecision(17) << fpVal;
                        initValue = oss.str();
                    }
                } else {
                    initValue = getDefaultValue(node.type.get());
                }
            } else {
                long long constVal;
                if (evaluateConstantExpr(node.initializer.get(), constVal)) {
                    initValue = std::to_string(constVal);
                } else {
                    initValue = getDefaultValue(node.type.get());
                }
            }
        } else {
            initValue = getDefaultValue(node.type.get());
        }

        emitGlobal(globalName + " = internal global " + llvmType + " " + initValue);

        // Register as global-like symbol
        IRSymbol sym;
        sym.name = node.name;
        sym.irName = globalName;
        sym.type = llvmType;
        sym.isGlobal = true;
        defineSymbol(node.name, sym);
        return;
    }

    // Use a unique suffix to handle variables with same name in different scopes
    std::string ptrName = "%" + node.name + ".addr" + std::to_string(tempCounter_++);

    emit(ptrName + " = alloca " + llvmType);

    // Initialize if present
    if (node.initializer) {
        std::string valReg;
        std::string valType = llvmType;

        // Special handling for InitializerList on array/struct types
        if (auto* initList = dynamic_cast<AST::InitializerList*>(node.initializer.get())) {
            // Generate the initializer value directly
            std::string initValue = generateInitializerValue(node.type.get(), initList);
            valReg = newTemp();
            // For arrays and structs, we need a way to initialize them on the stack
            // Use undef and insert the values, OR use a global constant and copy it
            // For now, generate the initialization directly as the value
            emit("store " + llvmType + " " + initValue + ", " + llvmType + "* " + ptrName);

            // Register symbol
            IRSymbol sym;
            sym.name = node.name;
            sym.irName = ptrName;
            sym.type = llvmType;
            defineSymbol(node.name, sym);
            return;
        }

        // For non-initializer-list expressions
        node.initializer->accept(*this);
        IRValue initVal = lastValue_;

        valReg = initVal.name;
        valType = initVal.type;

        // If initializer is a pointer (from load), we need to load it first
        if (initVal.isPointer && !initVal.isConstant) {
            valReg = newTemp();
            valType = initVal.derefType();
            emit(valReg + " = load " + valType + ", " + initVal.type + " " + initVal.name);
        }

        // Convert types if needed
        if (valType != llvmType) {
            std::string convertedReg = newTemp();
            // Float/double conversions
            if (valType == "double" && llvmType == "float") {
                emit(convertedReg + " = fptrunc double " + valReg + " to float");
                valReg = convertedReg;
                valType = "float";
            } else if (valType == "float" && llvmType == "double") {
                emit(convertedReg + " = fpext float " + valReg + " to double");
                valReg = convertedReg;
                valType = "double";
            }
            // Integer to float
            else if ((valType == "i32" || valType == "i16" || valType == "i8" || valType == "i64") &&
                     (llvmType == "float" || llvmType == "double")) {
                emit(convertedReg + " = sitofp " + valType + " " + valReg + " to " + llvmType);
                valReg = convertedReg;
                valType = llvmType;
            }
            // Float to integer
            else if ((llvmType == "i32" || llvmType == "i16" || llvmType == "i8" || llvmType == "i64") &&
                     (valType == "float" || valType == "double")) {
                emit(convertedReg + " = fptosi " + valType + " " + valReg + " to " + llvmType);
                valReg = convertedReg;
                valType = llvmType;
            }
            // Integer size conversions
            else {
                auto getIntSize = [](const std::string& t) -> int {
                    if (t == "i8") return 8;
                    if (t == "i16") return 16;
                    if (t == "i32") return 32;
                    if (t == "i64") return 64;
                    return 0;
                };
                int srcSize = getIntSize(valType);
                int dstSize = getIntSize(llvmType);
                if (srcSize > 0 && dstSize > 0) {
                    if (srcSize < dstSize) {
                        emit(convertedReg + " = sext " + valType + " " + valReg + " to " + llvmType);
                    } else {
                        emit(convertedReg + " = trunc " + valType + " " + valReg + " to " + llvmType);
                    }
                    valReg = convertedReg;
                    valType = llvmType;
                }
            }
        }

        emit("store " + llvmType + " " + valReg + ", " + llvmType + "* " + ptrName);
    }

    // Register symbol
    IRSymbol sym;
    sym.name = node.name;
    sym.irName = ptrName;
    sym.type = llvmType;
    defineSymbol(node.name, sym);
}

} // namespace cc1
