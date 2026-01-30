#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Builds a flattened struct initializer from a possibly nested list.
// FR: Construit un init de struct flatten depuis une liste imbriquee.
std::string IRGenerator::generateStructFromInitForFlattened(AST::StructType* stype, AST::InitializerList* typeInitList) {
    std::string res = "{ ";
    bool first = true;

    
    size_t memberCount = stype->isUnion ? 1 : stype->members.size();

    size_t initIdx = 0;  
    size_t j = 0;        

    while (j < memberCount && (!typeInitList || initIdx < typeInitList->initializers.size())) {
        if (!first) res += ", ";
        first = false;

        const auto& member = stype->members[j];

        
        if (member.isBitfield()) {
            
            int totalBits = member.bitWidth;
            size_t k = j + 1;
            while (k < memberCount && stype->members[k].isBitfield()) {
                totalBits += stype->members[k].bitWidth;
                k++;
            }

            
            std::string packType;
            if (totalBits <= 8) {
                packType = "i8";
            } else if (totalBits <= 16) {
                packType = "i16";
            } else if (totalBits <= 32) {
                packType = "i32";
            } else {
                packType = "i64";
            }

            res += packType + " ";

            
            long long packedValue = 0;
            int bitOffset = 0;
            for (size_t m = j; m < k; ++m) {
                if (typeInitList && initIdx < typeInitList->initializers.size()) {
                    long long val;
                    if (evaluateConstantExpr(typeInitList->initializers[initIdx++].get(), val)) {
                        
                        long long mask = (1LL << stype->members[m].bitWidth) - 1;
                        packedValue |= ((val & mask) << bitOffset);
                    }
                }
                bitOffset += stype->members[m].bitWidth;
            }
            res += std::to_string(packedValue);

            
            int packingBytes = totalBits / 8;
            if (packingBytes == 1) {
                
                res += ", [3 x i8] [i8 0, i8 0, i8 0]";
            }

            j = k;
        } else {
            auto* memberType = stripQualifiers(member.type.get());
            std::string memberTypeLLVM = typeToLLVM(memberType);
            res += memberTypeLLVM + " ";

            
            if (auto* nestedStruct = dynamic_cast<AST::StructType*>(memberType)) {
                if (typeInitList && initIdx < typeInitList->initializers.size()) {
                    if (auto* subList = dynamic_cast<AST::InitializerList*>(typeInitList->initializers[initIdx].get())) {
                        
                        res += generateStructFromInitForFlattened(nestedStruct, subList);
                    } else {
                        
                        res += generateStructFromInitForFlattened(nestedStruct, nullptr);
                    }
                    initIdx++;
                } else {
                    res += generateStructFromInitForFlattened(nestedStruct, nullptr);
                }
            }
            
            else if (auto* arrType = dynamic_cast<AST::ArrayType*>(memberType)) {
                size_t elemCount = 0;
                if (arrType->size >= 0) {
                    elemCount = static_cast<size_t>(arrType->size);
                }
                res += "[";

                
                AST::InitializerList* arrayInit = nullptr;
                if (typeInitList && initIdx < typeInitList->initializers.size()) {
                    if (auto* subList = dynamic_cast<AST::InitializerList*>(typeInitList->initializers[initIdx].get())) {
                        arrayInit = subList;
                    }
                    initIdx++;
                }

                for (size_t k = 0; k < elemCount; ++k) {
                    if (k > 0) res += ", ";
                    auto* elemType2 = stripQualifiers(arrType->elementType.get());
                    res += typeToLLVM(elemType2) + " ";

                    if (arrayInit && k < arrayInit->initializers.size()) {
                        long long val;
                        if (evaluateConstantExpr(arrayInit->initializers[k].get(), val)) {
                            res += std::to_string(val);
                        } else {
                            res += "0";
                        }
                    } else {
                        res += "0";
                    }
                }
                res += "]";
            }
            
            else {
                if (typeInitList && initIdx < typeInitList->initializers.size()) {
                    long long val;
                    if (evaluateConstantExpr(typeInitList->initializers[initIdx++].get(), val)) {
                        
                        if (auto* primType = dynamic_cast<AST::PrimitiveType*>(memberType)) {
                            if (primType->kind == AST::PrimitiveKind::Float) {
                                res += std::to_string(val) + ".0";
                            } else {
                                res += std::to_string(val);
                            }
                        } else {
                            res += std::to_string(val);
                        }
                    } else {
                        res += "0";
                    }
                } else {
                    res += "0";
                }
            }
            j++;
        }
    }

    
    while (j < memberCount) {
        if (!first) res += ", ";
        first = false;

        const auto& member = stype->members[j];
        auto* memberType = stripQualifiers(member.type.get());
        res += typeToLLVM(memberType) + " ";

        if (member.isBitfield()) {
            res += "0";
        } else if (dynamic_cast<AST::ArrayType*>(memberType)) {
            res += "zeroinitializer";
        } else {
            res += "0";
        }
        j++;
    }

    res += " }";
    return res;
}

} 
