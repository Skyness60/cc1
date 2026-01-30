#include <codegen/IRGenerator.hpp>

#include <iomanip>
#include <sstream>

namespace cc1 {

// EN: Emits IR for global variable declarations and initializers.
// FR: Genere l IR pour les variables globales et leurs init.
void IRGenerator::emitGlobalVarDecl(AST::VarDecl& node, const std::string& llvmType) {
    
    std::string globalName = "@" + node.name;

    
    if (node.storageClass == AST::StorageClass::Extern) {
        
        if (!declaredGlobals_.empty() && declaredGlobals_.count(node.name)) {
            return;
        }

        
        emitGlobal(globalName + " = external global " + llvmType);
        declaredGlobals_.insert(node.name);

        IRSymbol sym;
        sym.name = node.name;
        sym.irName = globalName;
        sym.type = llvmType;
        sym.isGlobal = true;
        defineSymbol(node.name, sym);
        return;
    }

    
    if (!declaredGlobals_.empty() && declaredGlobals_.count(node.name)) {
        return;
    }

    
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
            } else if (auto* strLit = dynamic_cast<AST::StringLiteral*>(node.initializer.get())) {
                
                std::string strName = newGlobalString(strLit->value);
                size_t len = strLit->value.size() + 1;
                std::string idxType = is64bit_ ? "i64" : "i32";
                initValue = "getelementptr inbounds ([" + std::to_string(len) + " x i8], [" +
                            std::to_string(len) + " x i8]* " + strName + ", " + idxType + " 0, " + idxType + " 0)";
            } else if (auto* initList = dynamic_cast<AST::InitializerList*>(node.initializer.get())) {
                
                
                bool isArrayOfStruct = false;
                if (auto* arrayType = dynamic_cast<AST::ArrayType*>(node.type.get())) {
                    auto* elemType = stripQualifiers(arrayType->elementType.get());
                    auto* structType = dynamic_cast<AST::StructType*>(elemType);
                    isArrayOfStruct = (structType != nullptr);
                }
                (void)isArrayOfStruct;

                
                initValue = generateInitializerValue(node.type.get(), initList);
            } else {
                initValue = getDefaultValue(node.type.get());
            }
        }
    } else {
        initValue = getDefaultValue(node.type.get());
    }

    
    std::string linkage = "dso_local global";

    emitGlobal(globalName + " = " + linkage + " " + llvmType + " " + initValue);
    declaredGlobals_.insert(node.name);

    
    
    IRSymbol sym;
    sym.name = node.name;
    sym.irName = globalName;
    sym.type = llvmType;  
    sym.isGlobal = true;
    defineSymbol(node.name, sym);
}

} 
