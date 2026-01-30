#include <codegen/IRGenerator.hpp>

#include <iomanip>
#include <sstream>

namespace cc1 {

// EN: Emits IR for local variable declarations and initialization.
// FR: Genere l IR pour les variables locales et leur init.
void IRGenerator::emitLocalVarDecl(AST::VarDecl& node, const std::string& llvmType) {
    
    
    if (node.storageClass == AST::StorageClass::Static) {
        
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

        
        IRSymbol sym;
        sym.name = node.name;
        sym.irName = globalName;
        sym.type = llvmType;
        sym.isGlobal = true;
        defineSymbol(node.name, sym);
        return;
    }

    
    std::string ptrName = "%" + node.name + ".addr" + std::to_string(tempCounter_++);

    emit(ptrName + " = alloca " + llvmType);

    
    if (node.initializer) {
        std::string valReg;
        std::string valType = llvmType;

        
        if (auto* initList = dynamic_cast<AST::InitializerList*>(node.initializer.get())) {
            
            std::string initValue = generateInitializerValue(node.type.get(), initList);
            valReg = newTemp();
            
            
            
            emit("store " + llvmType + " " + initValue + ", " + llvmType + "* " + ptrName);

            
            IRSymbol sym;
            sym.name = node.name;
            sym.irName = ptrName;
            sym.type = llvmType;
            defineSymbol(node.name, sym);
            return;
        }

        
        node.initializer->accept(*this);
        IRValue initVal = lastValue_;

        valReg = initVal.name;
        valType = initVal.type;

        
        if (initVal.isPointer && !initVal.isConstant) {
            valReg = newTemp();
            valType = initVal.derefType();
            emit(valReg + " = load " + valType + ", " + initVal.type + " " + initVal.name);
        }

        
        if (valType != llvmType) {
            std::string convertedReg = newTemp();
            
            if (valType == "double" && llvmType == "float") {
                emit(convertedReg + " = fptrunc double " + valReg + " to float");
                valReg = convertedReg;
                valType = "float";
            } else if (valType == "float" && llvmType == "double") {
                emit(convertedReg + " = fpext float " + valReg + " to double");
                valReg = convertedReg;
                valType = "double";
            }
            
            else if ((valType == "i32" || valType == "i16" || valType == "i8" || valType == "i64") &&
                     (llvmType == "float" || llvmType == "double")) {
                emit(convertedReg + " = sitofp " + valType + " " + valReg + " to " + llvmType);
                valReg = convertedReg;
                valType = llvmType;
            }
            
            else if ((llvmType == "i32" || llvmType == "i16" || llvmType == "i8" || llvmType == "i64") &&
                     (valType == "float" || valType == "double")) {
                emit(convertedReg + " = fptosi " + valType + " " + valReg + " to " + llvmType);
                valReg = convertedReg;
                valType = llvmType;
            }
            
            else {
                // EN: Maps integer LLVM types to bit-width for promotions.
                // FR: Mappe les types entiers LLVM vers la largeur en bits.
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

    
    IRSymbol sym;
    sym.name = node.name;
    sym.irName = ptrName;
    sym.type = llvmType;
    defineSymbol(node.name, sym);
}

} 
