#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Builds a flattened initializer for arrays of structs.
// FR: Construit un initialiseur flatten pour tableaux de structs.
std::string IRGenerator::generateFlattenedInitializer(AST::ArrayType* arrayType, AST::InitializerList* initList) {
    if (!arrayType || !initList) return "zeroinitializer";

    auto* elemType = stripQualifiers(arrayType->elementType.get());
    auto* structType = dynamic_cast<AST::StructType*>(elemType);

    if (!structType) {
        
        return generateInitializerValue(arrayType, initList);
    }

    
    size_t arraySize = 0;
    if (arrayType->size >= 0) {
        arraySize = static_cast<size_t>(arrayType->size);
    } else if (arrayType->sizeExpr) {
        long long sz;
        if (evaluateConstantExpr(arrayType->sizeExpr.get(), sz)) {
            arraySize = static_cast<size_t>(sz);
        }
    }

    if (arraySize == 0) {
        return "zeroinitializer";
    }

    std::string result = "[";
    std::string elemTypeLLVM = typeToLLVM(arrayType->elementType.get());

    
    for (size_t i = 0; i < arraySize && i < initList->initializers.size(); ++i) {
        if (i > 0) result += ", ";
        result += elemTypeLLVM + " ";

        const auto& init = initList->initializers[i];

        if (auto* subList = dynamic_cast<AST::InitializerList*>(init.get())) {
            
            result += generateStructFromInitForFlattened(structType, subList);
        } else {
            
            AST::InitializerList tempList(0, 0);
            tempList.initializers.push_back(std::unique_ptr<AST::Expression>(const_cast<AST::Expression*>(init.get())));
            result += generateStructFromInitForFlattened(structType, &tempList);
        }
    }

    
    for (size_t i = initList->initializers.size(); i < arraySize; ++i) {
        if (i > 0) result += ", ";
        result += elemTypeLLVM + " ";
        result += generateStructFromInitForFlattened(structType, nullptr);
    }

    result += "]";
    return result;
}

} 
