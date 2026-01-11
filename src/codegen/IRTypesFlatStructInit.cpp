#include <codegen/IRGenerator.hpp>

namespace cc1 {

// Generate LLVM struct initializer from a flat initializer list
// Takes the flattened initializers and distributes them across struct members
// Uses a reference parameter to track position in the flat list
std::string IRGenerator::generateStructInitializerFromFlatHelper(AST::StructType* st, AST::InitializerList* flatList,
                                                                size_t& idx) {
    if (!st || !flatList) return "zeroinitializer";

    std::string result = "{ ";
    bool first = true;

    size_t i = 0;
    while (i < st->members.size()) {
        if (!first) result += ", ";
        first = false;

        const auto& member = st->members[i];
        AST::Type* memberType = stripQualifiers(member.type.get());
        std::string memberTypeStr = typeToLLVM(memberType);

        // Handle bitfield packing
        if (member.isBitfield()) {
            // Count consecutive bitfields
            int totalBits = member.bitWidth;
            size_t j = i + 1;
            while (j < st->members.size() && st->members[j].isBitfield()) {
                totalBits += st->members[j].bitWidth;
                j++;
            }

            // Pack all bitfield values into a single byte
            long long packedValue = 0;
            int bitOffset = 0;
            for (size_t k = i; k < j; ++k) {
                if (idx < flatList->initializers.size()) {
                    long long val;
                    if (evaluateConstantExpr(flatList->initializers[idx].get(), val)) {
                        // Mask the value to its bitwidth and shift into position
                        long long mask = (1LL << st->members[k].bitWidth) - 1;
                        packedValue |= (val & mask) << bitOffset;
                    }
                    idx++;
                }
                bitOffset += st->members[k].bitWidth;
            }
            result += memberTypeStr + " " + std::to_string(packedValue);

            // Add padding if needed
            int packingBytes = totalBits / 8;
            if (packingBytes == 1 && j < st->members.size()) {
                // Bitfield took 1 byte, add 3 bytes of padding
                result += ", [3 x i8] zeroinitializer";
            }

            i = j;
        } else {
            // Regular (non-bitfield) member
            // Determine how many initializers this member consumes
            size_t memberInitCount = countFlattedMembers(memberType);

            // Generate initializer value for this member
            result += memberTypeStr + " ";

            if (idx >= flatList->initializers.size()) {
                // No more initializers available
                result += getDefaultValue(memberType);
            } else if (memberInitCount == 1) {
                // Single initializer for this member - extract value
                long long val;
                if (evaluateConstantExpr(flatList->initializers[idx].get(), val)) {
                    // Check if this is a struct/union - need to wrap in braces
                    if (auto* structMemberType = dynamic_cast<AST::StructType*>(memberType)) {
                        // For struct/union with 1 initializer, wrap in braces
                        result += "{ ";
                        if (!structMemberType->members.empty()) {
                            const auto& firstMember = structMemberType->members[0];
                            std::string firstMemberType = typeToLLVM(stripQualifiers(firstMember.type.get()));
                            result += firstMemberType + " ";
                            // Format the value based on member type
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
                    } else {
                        // Primitive type
                        if (auto* primType = dynamic_cast<AST::PrimitiveType*>(memberType)) {
                            if (primType->kind == AST::PrimitiveKind::Float ||
                                primType->kind == AST::PrimitiveKind::Double ||
                                primType->kind == AST::PrimitiveKind::LongDouble) {
                                // Float/double - add decimal point
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
                    // Could be an InitializerList for nested struct
                    if (auto* initListExpr = dynamic_cast<AST::InitializerList*>(flatList->initializers[idx].get())) {
                        // It's a nested initializer list
                        // If the list is empty, zero-init the member
                        if (initListExpr->initializers.empty()) {
                            result += getDefaultValue(memberType);
                        } else {
                            // Use it directly for the member
                            result += generateInitializerValue(memberType, initListExpr);
                        }
                        idx++;
                    } else {
                        result += getDefaultValue(memberType);
                        idx++;
                    }
                }
            } else {
                // Multiple initializers - recursively distribute them
                if (auto* structMemberType = dynamic_cast<AST::StructType*>(memberType)) {
                    // For struct members, check if we have a braced initializer list
                    if (idx < flatList->initializers.size()) {
                        if (auto* initListExpr = dynamic_cast<AST::InitializerList*>(flatList->initializers[idx].get())) {
                            // Use the braced list directly
                            result += generateInitializerValue(memberType, initListExpr);
                            idx++;
                        } else {
                            // No braced list - use flat helper
                            result += generateStructInitializerFromFlatHelper(structMemberType, flatList, idx);
                        }
                    } else {
                        result += getDefaultValue(memberType);
                    }
                } else if (auto* arrayMemberType = dynamic_cast<AST::ArrayType*>(memberType)) {
                    // For arrays, check if we have a braced initializer list first
                    if (idx < flatList->initializers.size()) {
                        if (auto* initListExpr = dynamic_cast<AST::InitializerList*>(flatList->initializers[idx].get())) {
                            // Use the braced list directly
                            result += generateInitializerValue(memberType, initListExpr);
                            idx++;
                        } else {
                            // Scalar values for array elements - consume multiple initializers
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

} // namespace cc1
