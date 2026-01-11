#include <codegen/IRGenerator.hpp>

namespace cc1 {

std::string IRGenerator::generateArrayInitializerValue(AST::ArrayType* arrayType, AST::InitializerList* initList) {
    if (!arrayType || !initList) return "zeroinitializer";

    size_t arraySize = 0;

    // Try to get array size from size field
    if (arrayType->size >= 0) {
        arraySize = static_cast<size_t>(arrayType->size);
    } else if (arrayType->sizeExpr) {
        // Try to evaluate the size expression
        long long sz;
        if (evaluateConstantExpr(arrayType->sizeExpr.get(), sz)) {
            arraySize = static_cast<size_t>(sz);
        }
    }

    // If still no size, use the number of initializers
    if (arraySize == 0 && !initList->initializers.empty()) {
        arraySize = initList->initializers.size();
    }

    if (arraySize == 0 || initList->initializers.empty()) {
        return "zeroinitializer";
    }

    std::string elemType = typeToLLVM(arrayType->elementType.get());
    auto* structType = dynamic_cast<AST::StructType*>(stripQualifiers(arrayType->elementType.get()));

    if (structType) {
        return generateArrayInitializerStruct(arrayType, structType, initList, arraySize, elemType);
    }
    return generateArrayInitializerNonStruct(arrayType, structType, initList, arraySize, elemType);
}

std::string IRGenerator::generateArrayInitializerStruct(AST::ArrayType* arrayType, AST::StructType* structType,
                                                       AST::InitializerList* initList, size_t arraySize,
                                                       const std::string& elemType) {
    std::string result = "[";

    // Determine if this is per-element or flat initialization
    bool isPerElement = false;
    if (!initList->initializers.empty()) {
        isPerElement = dynamic_cast<AST::InitializerList*>(initList->initializers[0].get()) != nullptr;
    }

    if (isPerElement) {
        // Per-element initialization
        size_t initIdx = 0;
        for (size_t i = 0; i < arraySize; ++i) {
            if (i > 0) result += ", ";

            if (initIdx < initList->initializers.size()) {
                if (auto* elemInitList = dynamic_cast<AST::InitializerList*>(initList->initializers[initIdx].get())) {
                    // Explicit InitializerList for this element
                    result += elemType + " " + generateInitializerValue(arrayType->elementType.get(), elemInitList);
                    initIdx++;
                } else {
                    // Scalar - treat as start of flat initialization for this element
                    result += elemType + " " + generateStructInitializerFromFlatHelper(structType, initList, initIdx);
                }
            } else {
                result += elemType + " zeroinitializer";
            }
        }
    } else {
        // Flat initialization - all scalars, one shared stream for all elements
        size_t initIdx = 0;
        for (size_t i = 0; i < arraySize; ++i) {
            if (i > 0) result += ", ";
            result += elemType + " " + generateStructInitializerFromFlatHelper(structType, initList, initIdx);
        }
    }

    result += "]";
    return result;
}

std::string IRGenerator::generateArrayInitializerNonStruct(AST::ArrayType* arrayType, AST::StructType* structType,
                                                          AST::InitializerList* initList, size_t arraySize,
                                                          const std::string& elemType) {
    std::string result = "[";

    // Normal array handling (not struct array or properly nested)
    for (size_t i = 0; i < arraySize; ++i) {
        if (i > 0) result += ", ";

        if (i < initList->initializers.size()) {
            // Try to generate initializer for this element
            if (auto* elemInitList = dynamic_cast<AST::InitializerList*>(initList->initializers[i].get())) {
                result += elemType + " " + generateInitializerValue(arrayType->elementType.get(), elemInitList);
            } else {
                // Single value - check if element type needs structure
                if (structType) {
                    // For struct types with a single scalar value, generate a struct initializer
                    // with the value in the first field
                    long long val;
                    if (evaluateConstantExpr(initList->initializers[i].get(), val)) {
                        // Generate struct with first field = val, rest = defaults
                        std::string structInit = "{ ";
                        const auto& firstMember = structType->members[0];
                        std::string firstType = typeToLLVM(firstMember.type.get());
                        structInit += firstType + " " + std::to_string(val);
                        // Fill remaining members
                        for (size_t j = 1; j < structType->members.size(); ++j) {
                            structInit += ", ";
                            const auto& member = structType->members[j];
                            std::string memberType = typeToLLVM(member.type.get());
                            structInit += memberType + " " + getDefaultValue(member.type.get());
                        }
                        structInit += " }";
                        result += elemType + " " + structInit;
                    } else {
                        result += elemType + " " + "zeroinitializer";
                    }
                } else if (auto* arrayElemType =
                               dynamic_cast<AST::ArrayType*>(stripQualifiers(arrayType->elementType.get()))) {
                    // For array element types with a single scalar value
                    long long val;
                    if (evaluateConstantExpr(initList->initializers[i].get(), val)) {
                        // Generate array with first element = val, rest = zeros
                        size_t elemArraySize = 0;
                        if (arrayElemType->size >= 0) {
                            elemArraySize = static_cast<size_t>(arrayElemType->size);
                        } else if (arrayElemType->sizeExpr) {
                            long long sz;
                            if (evaluateConstantExpr(arrayElemType->sizeExpr.get(), sz)) {
                                elemArraySize = static_cast<size_t>(sz);
                            }
                        }
                        std::string elemArrayType = typeToLLVM(arrayElemType->elementType.get());
                        std::string arrayInit = "[";
                        for (size_t j = 0; j < elemArraySize; ++j) {
                            if (j > 0) arrayInit += ", ";
                            arrayInit += elemArrayType + " ";
                            arrayInit += (j == 0) ? std::to_string(val) : "0";
                        }
                        arrayInit += "]";
                        result += elemType + " " + arrayInit;
                    } else {
                        result += elemType + " " + "zeroinitializer";
                    }
                } else {
                    // Scalar type
                    long long val;
                    if (evaluateConstantExpr(initList->initializers[i].get(), val)) {
                        result += elemType + " " + std::to_string(val);
                    } else {
                        result += elemType + " " + "zeroinitializer";
                    }
                }
            }
        } else {
            // End of initializer list - fill remaining with zeros
            result += elemType + " 0";
        }
    }

    result += "]";
    return result;
}

} // namespace cc1
