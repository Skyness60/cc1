#include <codegen/IRGenerator.hpp>

#include <iomanip>
#include <sstream>

namespace cc1 {

void IRGenerator::emitGlobalVarDecl(AST::VarDecl& node, const std::string& llvmType) {
    // Global variable
    std::string globalName = "@" + node.name;

    // Handle extern declarations
    if (node.storageClass == AST::StorageClass::Extern) {
        // Skip if already declared
        if (!declaredGlobals_.empty() && declaredGlobals_.count(node.name)) {
            return;
        }

        // Extern variable - just declare it
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

    // Skip if already declared (definition)
    if (!declaredGlobals_.empty() && declaredGlobals_.count(node.name)) {
        return;
    }

    // Determine initial value
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
                // String literal initializer for char arrays
                std::string strName = newGlobalString(strLit->value);
                (void)strName;
                // This is tricky - need to handle differently
                initValue = getDefaultValue(node.type.get());
            } else if (auto* initList = dynamic_cast<AST::InitializerList*>(node.initializer.get())) {
                // InitializerList - try to generate the proper initializer
                // For arrays of structs, use flattening (works with or without bitfields)
                bool isArrayOfStruct = false;
                if (auto* arrayType = dynamic_cast<AST::ArrayType*>(node.type.get())) {
                    auto* elemType = stripQualifiers(arrayType->elementType.get());
                    auto* structType = dynamic_cast<AST::StructType*>(elemType);
                    isArrayOfStruct = (structType != nullptr);
                }
                (void)isArrayOfStruct;

                // Use generateInitializerValue for all array types
                initValue = generateInitializerValue(node.type.get(), initList);
            } else {
                initValue = getDefaultValue(node.type.get());
            }
        }
    } else {
        initValue = getDefaultValue(node.type.get());
    }

    // Check for static/extern
    std::string linkage = "dso_local global";

    emitGlobal(globalName + " = " + linkage + " " + llvmType + " " + initValue);
    declaredGlobals_.insert(node.name);

    // Register symbol
    // Global variables are accessed via their address (@name is a pointer to the value)
    IRSymbol sym;
    sym.name = node.name;
    sym.irName = globalName;
    sym.type = llvmType;  // Store the value type, not pointer type
    sym.isGlobal = true;
    defineSymbol(node.name, sym);
}

} // namespace cc1
