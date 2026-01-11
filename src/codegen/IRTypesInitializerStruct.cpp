#include <codegen/IRGenerator.hpp>

namespace cc1 {

std::string IRGenerator::generateStructInitializerValue(AST::StructType* structType, AST::InitializerList* initList) {
    if (!structType || !initList) return "zeroinitializer";

    std::string result = "{ ";
    bool first = true;

    size_t initIdx = 0;  // Track position in initializers list

    // Process members, being aware of bitfield packing and padding
    size_t i = 0;
    while (i < structType->members.size()) {
        if (!first) result += ", ";
        first = false;

        const auto& member = structType->members[i];
        std::string memberType = typeToLLVM(member.type.get());

        // Check if this is a bitfield
        if (member.isBitfield()) {
            // Pack consecutive bitfields
            int totalBits = member.bitWidth;
            size_t j = i + 1;
            while (j < structType->members.size() && structType->members[j].isBitfield()) {
                totalBits += structType->members[j].bitWidth;
                j++;
            }

            // Pack all bitfield values into a single byte
            long long packedValue = 0;
            int bitOffset = 0;
            for (size_t k = i; k < j; ++k) {
                if (initIdx < initList->initializers.size()) {
                    long long val;
                    if (evaluateConstantExpr(initList->initializers[initIdx].get(), val)) {
                        // Mask the value to its bitwidth and shift into position
                        long long mask = (1LL << structType->members[k].bitWidth) - 1;
                        packedValue |= (val & mask) << bitOffset;
                    }
                    initIdx++;
                }
                bitOffset += structType->members[k].bitWidth;
            }
            result += memberType + " " + std::to_string(packedValue);

            // After bitfield packing, add padding if needed
            int packingBytes = totalBits / 8;
            if (packingBytes == 1) {
                result += ", [3 x i8] zeroinitializer";
            }

            i = j;
        } else {
            // Regular (non-bitfield) member
            if (initIdx < initList->initializers.size()) {
                if (auto* elemInitList = dynamic_cast<AST::InitializerList*>(initList->initializers[initIdx].get())) {
                    result += memberType + " " + generateInitializerValue(member.type.get(), elemInitList);
                } else {
                    // Scalar value - check member type
                    auto* memberStructType = dynamic_cast<AST::StructType*>(stripQualifiers(member.type.get()));
                    auto* memberArrayType = dynamic_cast<AST::ArrayType*>(stripQualifiers(member.type.get()));

                    if (memberStructType) {
                        // Create minimal initializer list for the struct member
                        long long val;
                        if (evaluateConstantExpr(initList->initializers[initIdx].get(), val)) {
                            // Generate struct with first field initialized
                            result += memberType + " { ";
                            if (!memberStructType->members.empty()) {
                                result += typeToLLVM(memberStructType->members[0].type.get()) + " ";
                                // Format value based on first member type
                                if (auto* primType = dynamic_cast<AST::PrimitiveType*>(
                                        stripQualifiers(memberStructType->members[0].type.get()))) {
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
                        } else {
                            result += memberType + " " + getDefaultValue(member.type.get());
                        }
                    } else if (memberArrayType) {
                        // Array member - consume multiple initializers, one per element
                        size_t elemCount = (memberArrayType->size > 0) ? memberArrayType->size : 0;
                        if (elemCount > 0) {
                            result += memberType + " [";
                            std::string elemTypeStr = typeToLLVM(memberArrayType->elementType.get());
                            for (size_t k = 0; k < elemCount; ++k) {
                                if (k > 0) result += ", ";
                                result += elemTypeStr + " ";
                                if (initIdx < initList->initializers.size()) {
                                    long long val;
                                    if (evaluateConstantExpr(initList->initializers[initIdx].get(), val)) {
                                        result += std::to_string(val);
                                    } else {
                                        result += "0";
                                    }
                                } else {
                                    result += "0";
                                }
                                initIdx++;
                            }
                            result += "]";
                        } else {
                            result += memberType + " " + getDefaultValue(member.type.get());
                        }
                    } else {
                        // Scalar primitive type
                        long long val;
                        if (evaluateConstantExpr(initList->initializers[initIdx].get(), val)) {
                            // Format value based on member type
                            std::string valStr = std::to_string(val);
                            if (auto* primType = dynamic_cast<AST::PrimitiveType*>(stripQualifiers(member.type.get()))) {
                                if (primType->kind == AST::PrimitiveKind::Float ||
                                    primType->kind == AST::PrimitiveKind::Double ||
                                    primType->kind == AST::PrimitiveKind::LongDouble) {
                                    valStr += ".0";
                                }
                            }
                            result += memberType + " " + valStr;
                        } else {
                            result += memberType + " " + getDefaultValue(member.type.get());
                        }
                    }
                }
                initIdx++;
            } else {
                // No more initializers - use default
                result += memberType + " " + getDefaultValue(member.type.get());
            }
            i++;
        }
    }

    result += " }";
    return result;
}

} // namespace cc1
