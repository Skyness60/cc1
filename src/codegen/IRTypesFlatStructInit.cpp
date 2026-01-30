#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Builds a struct initializer from a flattened initializer list.
// FR: Construit un init de struct depuis une liste flat.
std::string IRGenerator::generateStructInitializerFromFlatHelper(AST::StructType* st, AST::InitializerList* flatList,
                                                                size_t& idx) {
    if (!st || !flatList) return "zeroinitializer";
    if (st->members.empty()) return "zeroinitializer";

    std::string result = "{ ";
    bool first = true;

    size_t i = 0;
    while (i < st->members.size()) {
        if (!first) result += ", ";
        first = false;

        const auto& member = st->members[i];
        AST::Type* memberType = stripQualifiers(member.type.get());
        std::string memberTypeStr = typeToLLVM(memberType);

        
        if (member.isBitfield()) {
            
            int totalBits = member.bitWidth;
            size_t j = i + 1;
            while (j < st->members.size() && st->members[j].isBitfield()) {
                totalBits += st->members[j].bitWidth;
                j++;
            }

            
            long long packedValue = 0;
            int bitOffset = 0;
            for (size_t k = i; k < j; ++k) {
                if (idx < flatList->initializers.size()) {
                    long long val;
                    if (evaluateConstantExpr(flatList->initializers[idx].get(), val)) {
                        
                        long long mask = (1LL << st->members[k].bitWidth) - 1;
                        packedValue |= (val & mask) << bitOffset;
                    }
                    idx++;
                }
                bitOffset += st->members[k].bitWidth;
            }
            result += memberTypeStr + " " + std::to_string(packedValue);

            
            int packingBytes = totalBits / 8;
            if (packingBytes == 1 && j < st->members.size()) {
                
                result += ", [3 x i8] zeroinitializer";
            }

            i = j;
        } else {
            
            
            size_t memberInitCount = countFlattedMembers(memberType);

            
            result += memberTypeStr + " ";

            if (idx >= flatList->initializers.size()) {
                
                result += getDefaultValue(memberType);
            } else if (memberInitCount == 1) {
                
                long long val;
                if (evaluateConstantExpr(flatList->initializers[idx].get(), val)) {
                    
                    if (auto* structMemberType = dynamic_cast<AST::StructType*>(memberType)) {
                        
                        if (structMemberType->members.empty()) {
                            result += getDefaultValue(memberType);
                        } else {
                            result += "{ ";
                            if (!structMemberType->members.empty()) {
                                const auto& firstMember = structMemberType->members[0];
                                std::string firstMemberType = typeToLLVM(stripQualifiers(firstMember.type.get()));
                                result += firstMemberType + " ";
                                
                                if (auto* primType =
                                        dynamic_cast<AST::PrimitiveType*>(stripQualifiers(firstMember.type.get()))) {
                                    if (primType->kind == AST::PrimitiveKind::Float ||
                                        primType->kind == AST::PrimitiveKind::Double ||
                                        primType->kind == AST::PrimitiveKind::LongDouble) {
                                        result += std::to_string(val) + ".0";
                                    } else {
                                        result += std::to_string(val);
                                    }
                                } else {
                                    result += std::to_string(val);
                                }
                            }
                            result += " }";
                        }
                    } else {
                        
                        if (auto* primType = dynamic_cast<AST::PrimitiveType*>(memberType)) {
                            if (primType->kind == AST::PrimitiveKind::Float ||
                                primType->kind == AST::PrimitiveKind::Double ||
                                primType->kind == AST::PrimitiveKind::LongDouble) {
                                
                                result += std::to_string(val) + ".0";
                            } else {
                                result += std::to_string(val);
                            }
                        } else {
                            result += std::to_string(val);
                        }
                    }
                    idx++;
                } else {
                    
                    if (auto* initListExpr = dynamic_cast<AST::InitializerList*>(flatList->initializers[idx].get())) {
                        
                        
                        if (initListExpr->initializers.empty()) {
                            result += getDefaultValue(memberType);
                        } else {
                            
                            result += generateInitializerValue(memberType, initListExpr);
                        }
                        idx++;
                    } else {
                        result += getDefaultValue(memberType);
                        idx++;
                    }
                }
            } else {
                
                if (auto* structMemberType = dynamic_cast<AST::StructType*>(memberType)) {
                    
                    if (idx < flatList->initializers.size()) {
                        if (auto* initListExpr = dynamic_cast<AST::InitializerList*>(flatList->initializers[idx].get())) {
                            
                            result += generateInitializerValue(memberType, initListExpr);
                            idx++;
                        } else {
                            
                            result += generateStructInitializerFromFlatHelper(structMemberType, flatList, idx);
                        }
                    } else {
                        result += getDefaultValue(memberType);
                    }
                } else if (auto* arrayMemberType = dynamic_cast<AST::ArrayType*>(memberType)) {
                    
                    if (idx < flatList->initializers.size()) {
                        if (auto* initListExpr = dynamic_cast<AST::InitializerList*>(flatList->initializers[idx].get())) {
                            
                            result += generateInitializerValue(memberType, initListExpr);
                            idx++;
                        } else {
                            
                            size_t elemCount = (arrayMemberType->size > 0) ? arrayMemberType->size : 0;
                            if (elemCount > 0) {
                                result += "[";
                                std::string elemTypeStr = typeToLLVM(arrayMemberType->elementType.get());
                                for (size_t k = 0; k < elemCount; ++k) {
                                    if (k > 0) result += ", ";
                                    result += elemTypeStr + " ";
                                    if (idx < flatList->initializers.size()) {
                                        long long val;
                                        if (evaluateConstantExpr(flatList->initializers[idx].get(), val)) {
                                            result += std::to_string(val);
                                        } else {
                                            result += "0";
                                        }
                                        idx++;
                                    } else {
                                        result += "0";
                                    }
                                }
                                result += "]";
                            } else {
                                result += getDefaultValue(memberType);
                            }
                        }
                    } else {
                        result += getDefaultValue(memberType);
                    }
                } else {
                    result += getDefaultValue(memberType);
                }
            }
            i++;
        }
    }

    result += " }";
    return result;
}

} 
