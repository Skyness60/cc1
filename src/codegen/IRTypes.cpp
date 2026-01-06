#include <codegen/IRGenerator.hpp>
#include <functional>

namespace cc1 {

// ============================================================================
// Type to LLVM IR Mapping
// ============================================================================

std::string IRGenerator::typeToLLVM(AST::Type* type) {
    if (!type) return "void";
    
    type = stripQualifiers(type);
    
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        return primitiveToLLVM(prim->kind);
    }
    
    if (auto* ptr = dynamic_cast<AST::PointerType*>(type)) {
        std::string pointeeType = typeToLLVM(ptr->pointee.get());
        // In LLVM, void* is not valid - use i8* instead
        if (pointeeType == "void") {
            return "i8*";
        }
        return pointeeType + "*";
    }
    
    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        std::string elemType = typeToLLVM(arr->elementType.get());
        long long size = arr->size;
        if (size < 0 && arr->sizeExpr) {
            evaluateConstantExpr(arr->sizeExpr.get(), size);
        }
        if (size > 0) {
            return "[" + std::to_string(size) + " x " + elemType + "]";
        }
        // Unsized array - treat as pointer
        return elemType + "*";
    }
    
    if (auto* func = dynamic_cast<AST::FunctionType*>(type)) {
        std::string result = typeToLLVM(func->returnType.get()) + " (";
        for (size_t i = 0; i < func->parameterTypes.size(); ++i) {
            if (i > 0) result += ", ";
            result += typeToLLVM(func->parameterTypes[i].get());
        }
        if (func->isVariadic) {
            if (!func->parameterTypes.empty()) result += ", ";
            result += "...";
        }
        result += ")*";
        return result;
    }
    
    if (auto* typedefType = dynamic_cast<AST::TypedefType*>(type)) {
        // Resolve typedef to its underlying type
        AST::Type* resolved = resolveTypedef(typedefType->name);
        if (resolved) {
            return typeToLLVM(resolved);
        }
        // Fallback if not resolved
        return "i32";
    }
    
    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        if (!structType->name.empty()) {
            // Named struct - register for module-level definition
            collectNamedStruct(structType);
            return "%struct." + structType->name;
        }
        // Anonymous struct - compute inline
        std::string result = "{ ";
        if (structType->isUnion && !structType->members.empty()) {
            // Union: use largest member
            int maxSize = 0;
            std::string maxType;
            for (const auto& member : structType->members) {
                int size = getTypeSize(member.type.get());
                if (size > maxSize) {
                    maxSize = size;
                    maxType = typeToLLVM(member.type.get());
                }
            }
            result += maxType;
        } else {
            // Struct: all members, packing consecutive bitfields
            size_t i = 0;
            while (i < structType->members.size()) {
                if (i > 0) result += ", ";
                
                const auto& member = structType->members[i];
                
                // If this is a bitfield, pack consecutive bitfields into one type
                if (member.isBitfield()) {
                    int totalBits = member.bitWidth;
                    size_t j = i + 1;
                    // Count consecutive bitfields
                    while (j < structType->members.size() && structType->members[j].isBitfield()) {
                        totalBits += structType->members[j].bitWidth;
                        j++;
                    }
                    
                    // Determine packing type
                    if (totalBits <= 8) {
                        result += "i8";
                    } else if (totalBits <= 16) {
                        result += "i16";
                    } else if (totalBits <= 32) {
                        result += "i32";
                    } else {
                        result += "i64";
                    }
                    
                    // Skip the packed bitfields
                    i = j;
                } else {
                    result += typeToLLVM(member.type.get());
                    i++;
                }
            }
        }
        result += " }";
        
        // Register the inline struct member info for later lookup
        registerInlineStructMembers(result, structType);
        
        return result;
    }
    
    if (auto* enumType = dynamic_cast<AST::EnumType*>(type)) {
        // Enums are i32
        return is64bit_ ? "i32" : "i32";
    }
    
    return "i32";  // Default fallback
}

std::string IRGenerator::primitiveToLLVM(AST::PrimitiveKind kind) {
    switch (kind) {
        case AST::PrimitiveKind::Void:
            return "void";
        case AST::PrimitiveKind::Char:
        case AST::PrimitiveKind::SignedChar:
        case AST::PrimitiveKind::UnsignedChar:
            return "i8";
        case AST::PrimitiveKind::Short:
        case AST::PrimitiveKind::UnsignedShort:
            return "i16";
        case AST::PrimitiveKind::Int:
        case AST::PrimitiveKind::UnsignedInt:
            return "i32";
        case AST::PrimitiveKind::Long:
        case AST::PrimitiveKind::UnsignedLong:
            return is64bit_ ? "i64" : "i32";
        case AST::PrimitiveKind::LongLong:
        case AST::PrimitiveKind::UnsignedLongLong:
            return "i64";
        case AST::PrimitiveKind::Float:
            return "float";
        case AST::PrimitiveKind::Double:
            return "double";
        case AST::PrimitiveKind::LongDouble:
            return is64bit_ ? "x86_fp80" : "x86_fp80";
    }
    return "i32";
}

int IRGenerator::getTypeSize(AST::Type* type) {
    if (!type) return 0;
    
    type = stripQualifiers(type);
    
    // Handle typedef
    if (auto* typedefType = dynamic_cast<AST::TypedefType*>(type)) {
        AST::Type* resolved = resolveTypedef(typedefType->name);
        if (resolved) {
            return getTypeSize(resolved);
        }
        return 4;  // Fallback
    }
    
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        return getPrimitiveSize(prim->kind);
    }
    
    if (dynamic_cast<AST::PointerType*>(type)) {
        return is64bit_ ? 8 : 4;
    }
    
    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        long long size = arr->size;
        if (size < 0 && arr->sizeExpr) {
            evaluateConstantExpr(arr->sizeExpr.get(), size);
        }
        if (size > 0) {
            return static_cast<int>(size * getTypeSize(arr->elementType.get()));
        }
        return 0;
    }
    
    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        if (!structType->name.empty()) {
            auto* layout = getStructLayout(structType->name);
            if (layout) return layout->totalSize;
        }
        
        if (structType->isUnion) {
            int maxSize = 0;
            for (const auto& member : structType->members) {
                maxSize = std::max(maxSize, getTypeSize(member.type.get()));
            }
            return maxSize > 0 ? maxSize : 1;
        } else {
            int totalSize = 0;
            for (const auto& member : structType->members) {
                totalSize += getTypeSize(member.type.get());
            }
            return totalSize > 0 ? totalSize : 1;
        }
    }
    
    if (dynamic_cast<AST::EnumType*>(type)) {
        return 4;  // Enums are int
    }
    
    return 4;  // Default
}

int IRGenerator::getTypeAlign(AST::Type* type) {
    if (!type) return 1;
    
    type = stripQualifiers(type);
    
    // Handle typedef
    if (auto* typedefType = dynamic_cast<AST::TypedefType*>(type)) {
        AST::Type* resolved = resolveTypedef(typedefType->name);
        if (resolved) {
            return getTypeAlign(resolved);
        }
        return 4;  // Fallback
    }
    
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        int size = getPrimitiveSize(prim->kind);
        // Alignment is min(size, pointer_size)
        int ptrSize = is64bit_ ? 8 : 4;
        return std::min(size, ptrSize);
    }
    
    if (dynamic_cast<AST::PointerType*>(type)) {
        return is64bit_ ? 8 : 4;
    }
    
    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        return getTypeAlign(arr->elementType.get());
    }
    
    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        if (!structType->name.empty()) {
            auto* layout = getStructLayout(structType->name);
            if (layout) return layout->alignment;
        }
        
        int maxAlign = 1;
        for (const auto& member : structType->members) {
            maxAlign = std::max(maxAlign, getTypeAlign(member.type.get()));
        }
        return maxAlign;
    }
    
    return 4;  // Default
}

int IRGenerator::getPrimitiveSize(AST::PrimitiveKind kind) {
    switch (kind) {
        case AST::PrimitiveKind::Void:
            return 0;
        case AST::PrimitiveKind::Char:
        case AST::PrimitiveKind::SignedChar:
        case AST::PrimitiveKind::UnsignedChar:
            return 1;
        case AST::PrimitiveKind::Short:
        case AST::PrimitiveKind::UnsignedShort:
            return 2;
        case AST::PrimitiveKind::Int:
        case AST::PrimitiveKind::UnsignedInt:
            return 4;
        case AST::PrimitiveKind::Long:
        case AST::PrimitiveKind::UnsignedLong:
            return is64bit_ ? 8 : 4;
        case AST::PrimitiveKind::LongLong:
        case AST::PrimitiveKind::UnsignedLongLong:
            return 8;
        case AST::PrimitiveKind::Float:
            return 4;
        case AST::PrimitiveKind::Double:
            return 8;
        case AST::PrimitiveKind::LongDouble:
            return is64bit_ ? 16 : 12;  // x86_fp80
    }
    return 4;
}

std::string IRGenerator::getDefaultValue(AST::Type* type) {
    if (!type) return "0";
    
    type = stripQualifiers(type);
    
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        switch (prim->kind) {
            case AST::PrimitiveKind::Float:
                return "0.0";
            case AST::PrimitiveKind::Double:
            case AST::PrimitiveKind::LongDouble:
                return "0.0";
            default:
                return "0";
        }
    }
    
    if (dynamic_cast<AST::PointerType*>(type)) {
        return "null";
    }
    
    if (dynamic_cast<AST::ArrayType*>(type)) {
        return "zeroinitializer";
    }
    
    if (dynamic_cast<AST::StructType*>(type)) {
        return "zeroinitializer";
    }
    
    return "0";
}

std::string IRGenerator::generateInitializerValue(AST::Type* type, AST::InitializerList* initList) {
    if (!type || !initList) return "zeroinitializer";
    
    type = stripQualifiers(type);
    
    // Handle array types
    if (auto* arrayType = dynamic_cast<AST::ArrayType*>(type)) {
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
        
        std::string result = "[";
        std::string elemType = typeToLLVM(arrayType->elementType.get());
        
        // Check if array element is a struct and if initializers are properly nested (all are InitializerLists)
        auto* structType = dynamic_cast<AST::StructType*>(stripQualifiers(arrayType->elementType.get()));
        
        // Count how many of the initializers are actually InitializerLists
        size_t nestedCount = 0;
        for (const auto& init : initList->initializers) {
            if (dynamic_cast<AST::InitializerList*>(init.get())) {
                nestedCount++;
            }
        }
        
        // If we have struct array with nested lists (possibly mixed with scalars),
        // process them by grouping initializers into array elements
        if (structType && nestedCount > 0) {
            // Mixed braced and unbraced initializers - need special handling
            // Process elements, using flattened path for scalar portions
            
            // Calculate members per struct
            auto countStructMembers = [this](AST::StructType* st) -> size_t {
                size_t count = 0;
                for (const auto& member : st->members) {
                    count += countFlattedMembers(member.type.get());
                }
                return count;
            };
            
            size_t membersPerStruct = countStructMembers(structType);
            size_t initIdx = 0;
            
            for (size_t i = 0; i < arraySize; ++i) {
                if (i > 0) result += ", ";
                
                if (initIdx < initList->initializers.size()) {
                    if (auto* elemInitList = dynamic_cast<AST::InitializerList*>(initList->initializers[initIdx].get())) {
                        // Element has proper braced initializer
                        result += elemType + " " + generateInitializerValue(arrayType->elementType.get(), elemInitList);
                        initIdx++;
                    } else {
                        // Element uses unbraced (flattened) initializers
                        result += elemType + " " + generateStructInitializerFromFlatHelper(structType, initList, initIdx);
                    }
                } else {
                    result += elemType + " " + "zeroinitializer";
                }
            }
        } else if (structType && initList->initializers.size() > arraySize) {
            // Likely a flattened initializer list - need to group elements back into structs
            // Calculate members per struct including nested structs and arrays
            auto countStructMembers = [this](AST::StructType* st) -> size_t {
                size_t count = 0;
                for (const auto& member : st->members) {
                    count += countFlattedMembers(member.type.get());
                }
                return count;
            };
            
            size_t membersPerStruct = countStructMembers(structType);
            if (membersPerStruct > 0 && membersPerStruct < initList->initializers.size()) {
                // Process flattened list, grouping into structs
                size_t initIdx = 0;
                for (size_t i = 0; i < arraySize; ++i) {
                    if (i > 0) result += ", ";
                    
                    // Generate struct initializer from flat list
                    result += elemType + " " + generateStructInitializerFromFlatHelper(structType, initList, initIdx);
                }
            } else {
                // Not a flattened list, process normally
                for (size_t i = 0; i < arraySize; ++i) {
                    if (i > 0) result += ", ";
                    
                    if (i < initList->initializers.size()) {
                        if (auto* elemInitList = dynamic_cast<AST::InitializerList*>(initList->initializers[i].get())) {
                            result += elemType + " " + generateInitializerValue(arrayType->elementType.get(), elemInitList);
                        } else {
                            result += elemType + " " + "zeroinitializer";
                        }
                    } else {
                        result += elemType + " " + "zeroinitializer";
                    }
                }
            }
        } else {
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
                        } else if (auto* arrayElemType = dynamic_cast<AST::ArrayType*>(stripQualifiers(arrayType->elementType.get()))) {
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
        }
        
        result += "]";
        return result;
    }
    
    // Handle struct types
    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
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
                
                // Extract value for the first bitfield (or combined value for packed bitfields)
                if (initIdx < initList->initializers.size()) {
                    long long val;
                    if (evaluateConstantExpr(initList->initializers[initIdx].get(), val)) {
                        result += memberType + " " + std::to_string(val);
                    } else {
                        result += memberType + " " + getDefaultValue(member.type.get());
                    }
                    initIdx++;
                    
                    // For additional bitfields in the pack, consume more initializers
                    for (size_t k = i + 1; k < j; ++k) {
                        if (initIdx < initList->initializers.size()) {
                            initIdx++;  // Consume but don't emit separately (already packed)
                        }
                    }
                } else {
                    result += memberType + " " + getDefaultValue(member.type.get());
                }
                
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
                                    if (auto* primType = dynamic_cast<AST::PrimitiveType*>(stripQualifiers(memberStructType->members[0].type.get()))) {
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
                            // Array member with scalar initializer - fill first element and rest with zeros
                            long long val;
                            if (evaluateConstantExpr(initList->initializers[initIdx].get(), val)) {
                                size_t elemCount = (memberArrayType->size > 0) ? memberArrayType->size : 0;
                                if (elemCount > 0) {
                                    result += memberType + " [";
                                    std::string elemTypeStr = typeToLLVM(memberArrayType->elementType.get());
                                    for (size_t k = 0; k < elemCount; ++k) {
                                        if (k > 0) result += ", ";
                                        result += elemTypeStr + " ";
                                        result += (k == 0) ? std::to_string(val) : "0";
                                    }
                                    result += "]";
                                } else {
                                    result += memberType + " " + getDefaultValue(member.type.get());
                                }
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
    
    // For other types, return zeroinitializer
    return "zeroinitializer";
}

AST::Type* IRGenerator::stripQualifiers(AST::Type* type) {
    while (auto* qual = dynamic_cast<AST::QualifiedType*>(type)) {
        type = qual->baseType.get();
    }
    // Also strip typedefs by looking them up
    while (auto* typedefType = dynamic_cast<AST::TypedefType*>(type)) {
        AST::Type* resolved = resolveTypedef(typedefType->name);
        if (resolved) {
            type = resolved;
        } else {
            break;  // Unresolved typedef
        }
    }
    return type;
}

// ============================================================================
// Struct Layout Computation
// ============================================================================

IRGenerator::StructLayout IRGenerator::computeStructLayout(AST::StructDecl* decl) {
    StructLayout layout;
    
    if (decl->isUnion) {
        // Union: all members at offset 0, size is max member size
        layout.llvmType = "%struct." + decl->name + " = type { ";
        int maxSize = 0;
        std::string maxType;
        int maxAlign = 1;
        
        for (size_t i = 0; i < decl->members.size(); ++i) {
            auto& member = decl->members[i];
            std::string memberType = typeToLLVM(member->type.get());
            int memberSize = getTypeSize(member->type.get());
            int memberAlign = getTypeAlign(member->type.get());
            
            layout.memberIndices[member->name] = 0;  // All at index 0
            layout.memberTypes[member->name] = memberType;
            
            if (memberSize > maxSize) {
                maxSize = memberSize;
                maxType = memberType;
            }
            maxAlign = std::max(maxAlign, memberAlign);
        }
        
        layout.llvmType += maxType + " }";
        layout.totalSize = maxSize > 0 ? maxSize : 1;
        layout.alignment = maxAlign;
    } else {
        // Struct: sequential layout
        layout.llvmType = "%struct." + decl->name + " = type { ";
        int totalSize = 0;
        int maxAlign = 1;
        
        for (size_t i = 0; i < decl->members.size(); ++i) {
            auto& member = decl->members[i];
            std::string memberType = typeToLLVM(member->type.get());
            int memberSize = getTypeSize(member->type.get());
            int memberAlign = getTypeAlign(member->type.get());
            
            if (i > 0) layout.llvmType += ", ";
            layout.llvmType += memberType;
            
            layout.memberIndices[member->name] = static_cast<int>(i);
            layout.memberTypes[member->name] = memberType;
            
            totalSize += memberSize;
            maxAlign = std::max(maxAlign, memberAlign);
        }
        
        layout.llvmType += " }";
        layout.totalSize = totalSize > 0 ? totalSize : 1;
        layout.alignment = maxAlign;
    }
    
    return layout;
}

IRGenerator::StructLayout IRGenerator::computeStructLayout(AST::StructType* type) {
    StructLayout layout;
    
    if (type->isUnion) {
        layout.llvmType = "{ ";
        int maxSize = 0;
        std::string maxType = "i8";
        int maxAlign = 1;
        
        for (size_t i = 0; i < type->members.size(); ++i) {
            auto& member = type->members[i];
            std::string memberType = typeToLLVM(member.type.get());
            int memberSize = getTypeSize(member.type.get());
            int memberAlign = getTypeAlign(member.type.get());
            
            layout.memberIndices[member.name] = 0;
            layout.memberTypes[member.name] = memberType;
            
            if (memberSize > maxSize) {
                maxSize = memberSize;
                maxType = memberType;
            }
            maxAlign = std::max(maxAlign, memberAlign);
        }
        
        layout.llvmType += maxType + " }";
        layout.totalSize = maxSize > 0 ? maxSize : 1;
        layout.alignment = maxAlign;
    } else {
        layout.llvmType = "{ ";
        int totalSize = 0;
        int maxAlign = 1;
        
        for (size_t i = 0; i < type->members.size(); ++i) {
            auto& member = type->members[i];
            std::string memberType = typeToLLVM(member.type.get());
            int memberSize = getTypeSize(member.type.get());
            int memberAlign = getTypeAlign(member.type.get());
            
            if (i > 0) layout.llvmType += ", ";
            layout.llvmType += memberType;
            
            layout.memberIndices[member.name] = static_cast<int>(i);
            layout.memberTypes[member.name] = memberType;
            
            totalSize += memberSize;
            maxAlign = std::max(maxAlign, memberAlign);
        }
        
        layout.llvmType += " }";
        layout.totalSize = totalSize > 0 ? totalSize : 1;
        layout.alignment = maxAlign;
    }
    
    return layout;
}

const IRGenerator::StructLayout* IRGenerator::getStructLayout(const std::string& name) {
    auto it = structLayouts_.find(name);
    if (it != structLayouts_.end()) {
        return &it->second;
    }
    return nullptr;
}

void IRGenerator::collectNamedStruct(AST::StructType* structType) {
    if (!structType) {
        return;  // Null struct type
    }
    
    if (structType->name.empty()) {
        return;  // Not a named struct
    }
    
    // Check if already collected
    if (namedStructDefs_.count(structType->name)) {
        return;
    }
    
    // First, ensure all nested structs have names and are collected
    static int anonCounter = 0;
    if (structType->members.size() > 0) {
        for (const auto& member : structType->members) {
            if (member.type) {
                if (auto* nestedStruct = dynamic_cast<AST::StructType*>(member.type.get())) {
                    // Give anonymous nested structs a generated name
                    if (nestedStruct->name.empty()) {
                        nestedStruct->name = "anon." + std::to_string(anonCounter++);
                    }
                    // Recursively collect nested structs FIRST
                    collectNamedStruct(nestedStruct);
                }
            }
        }
    }
    
    // Now generate definition for this struct (nested structs already defined)
    std::string definition = generateStructTypeDefinition(structType);
    namedStructDefs_[structType->name] = {definition, structType};
}

std::string IRGenerator::generateStructTypeDefinition(AST::StructType* structType) {
    if (!structType) {
        return "%struct.unknown = type { i32 }";
    }
    
    std::string result = "%struct." + structType->name + " = type { ";
    
    if (structType->isUnion && !structType->members.empty()) {
        // Union: use largest member
        int maxSize = 0;
        std::string maxType;
        for (const auto& member : structType->members) {
            if (member.type) {
                int size = getTypeSize(member.type.get());
                if (size > maxSize) {
                    maxSize = size;
                    maxType = typeToLLVM(member.type.get());
                }
            }
        }
        result += maxType;
    } else {
        // Struct: all members with bitfield packing
        size_t i = 0;
        while (i < structType->members.size()) {
            if (i > 0) result += ", ";
            
            const auto& member = structType->members[i];
            
            // If this is a bitfield, pack consecutive bitfields into one type
            if (member.isBitfield()) {
                int totalBits = member.bitWidth;
                size_t j = i + 1;
                // Count consecutive bitfields
                while (j < structType->members.size() && structType->members[j].isBitfield()) {
                    totalBits += structType->members[j].bitWidth;
                    j++;
                }
                
                // Determine packing type
                if (totalBits <= 8) {
                    result += "i8";
                } else if (totalBits <= 16) {
                    result += "i16";
                } else if (totalBits <= 32) {
                    result += "i32";
                } else {
                    result += "i64";
                }
                
                // Add padding after bitfields (3 bytes for single byte bitfields on 32-bit)
                // This matches clang's alignment padding behavior
                int packingBytes = totalBits / 8;
                if (packingBytes == 1) {
                    // One byte of bitfields -> add 3 bytes padding for 4-byte alignment
                    result += ", [3 x i8]";
                }
                
                // Skip the packed bitfields
                i = j;
            } else {
                if (member.type) {
                    result += typeToLLVM(member.type.get());
                } else {
                    result += "i32";  // Fallback for null types
                }
                i++;
            }
        }
    }
    
    result += " }";
    return result;
}

AST::Type* IRGenerator::resolveTypedef(const std::string& name) {
    auto it = typedefMap_.find(name);
    if (it != typedefMap_.end() && it->second) {
        return it->second;
    }
    return nullptr;
}

std::string IRGenerator::extractFieldTypeFromInlineStruct(const std::string& inlineStructType, int fieldIndex) {
    // Parse inline struct type like "{ float, [2 x i32], { i32 } }"
    // and extract the Nth field type
    
    if (inlineStructType.empty() || inlineStructType[0] != '{') {
        return "i32";  // Not an inline struct
    }
    
    // Skip the opening brace
    size_t pos = 1;
    int currentField = 0;
    int braceDepth = 0;
    size_t fieldStart = pos;
    
    while (pos < inlineStructType.size()) {
        char c = inlineStructType[pos];
        
        // Track nested braces
        if (c == '{' || c == '[') {
            braceDepth++;
        } else if (c == '}' || c == ']') {
            braceDepth--;
            if (c == '}' && braceDepth == -1) {
                // End of outer struct
                break;
            }
        }
        // Comma separates fields (only at depth 0)
        else if (c == ',' && braceDepth == 0) {
            if (currentField == fieldIndex) {
                // Extract the field type
                std::string fieldType = inlineStructType.substr(fieldStart, pos - fieldStart);
                // Trim whitespace
                while (!fieldType.empty() && std::isspace(fieldType.front())) {
                    fieldType = fieldType.substr(1);
                }
                while (!fieldType.empty() && std::isspace(fieldType.back())) {
                    fieldType = fieldType.substr(0, fieldType.size() - 1);
                }
                return fieldType;
            }
            currentField++;
            fieldStart = pos + 1;  // Skip the comma
        }
        
        pos++;
    }
    
    // Last field or single field
    if (currentField == fieldIndex) {
        std::string fieldType = inlineStructType.substr(fieldStart, pos - fieldStart);
        // Trim whitespace
        while (!fieldType.empty() && std::isspace(fieldType.front())) {
            fieldType = fieldType.substr(1);
        }
        while (!fieldType.empty() && std::isspace(fieldType.back())) {
            fieldType = fieldType.substr(0, fieldType.size() - 1);
        }
        return fieldType;
    }
    
    return "i32";  // Field not found, return default
}

void IRGenerator::registerInlineStructMembers(const std::string& llvmType, AST::StructType* structType) {
    if (!structType || llvmType.empty()) return;
    
    try {
        // Create a map of member names to indices for this struct
        std::map<std::string, int> memberMap;
        
        int index = 0;
        for (const auto& member : structType->members) {
            if (!member.name.empty()) {
                memberMap[member.name] = index;
            }
            index++;
        }
        
        // Store in the global map
        if (!memberMap.empty()) {
            inlineStructMembers_[llvmType] = memberMap;
        }
    } catch (...) {
        // Silently ignore any errors during registration
    }
}

int IRGenerator::findMemberIndexInInlineStruct(const std::string& llvmType, const std::string& memberName) {
    auto it = inlineStructMembers_.find(llvmType);
    if (it != inlineStructMembers_.end()) {
        auto memberIt = it->second.find(memberName);
        if (memberIt != it->second.end()) {
            DebugLogger::instance().log("[InlineStruct] Found member '" + memberName + 
                                       "' at index " + std::to_string(memberIt->second));
            return memberIt->second;
        }
    }
    
    // Not found - return -1 to indicate lookup failed
    DebugLogger::instance().log("[InlineStruct] Member '" + memberName + "' not found in " + llvmType);
    return -1;
}

std::string IRGenerator::generateFlattenedInitializer(AST::ArrayType* arrayType, AST::InitializerList* initList) {
    if (!arrayType || !initList) return "zeroinitializer";
    
    auto* elemType = stripQualifiers(arrayType->elementType.get());
    auto* structType = dynamic_cast<AST::StructType*>(elemType);
    
    if (!structType) {
        // Not an array of struct - use regular initializer
        return generateInitializerValue(arrayType, initList);
    }
    
    // Get array size
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
    
    // Helper function to generate initializer for a struct type given its initializer list
    std::function<std::string(AST::StructType*, AST::InitializerList*)> generateStructFromInit = 
        [&](AST::StructType* stype, AST::InitializerList* typeInitList) -> std::string {
        std::string res = "{ ";
        bool first = true;
        
        // For unions, only initialize the first member
        size_t memberCount = stype->isUnion ? 1 : stype->members.size();
        
        size_t initIdx = 0;  // Index into typeInitList->initializers
        size_t j = 0;        // Index into struct members
        
        while (j < memberCount && (!typeInitList || initIdx < typeInitList->initializers.size())) {
            if (!first) res += ", ";
            first = false;
            
            const auto& member = stype->members[j];
            
            // Check if this is a bitfield and collect consecutive bitfields
            if (member.isBitfield()) {
                // Count consecutive bitfields and pack them
                int totalBits = member.bitWidth;
                size_t k = j + 1;
                while (k < memberCount && stype->members[k].isBitfield()) {
                    totalBits += stype->members[k].bitWidth;
                    k++;
                }
                
                // Determine packing type
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
                
                // Consume all the values for the bitfields
                long long packedValue = 0;
                int bitOffset = 0;
                for (size_t m = j; m < k; ++m) {
                    if (typeInitList && initIdx < typeInitList->initializers.size()) {
                        long long val;
                        if (evaluateConstantExpr(typeInitList->initializers[initIdx++].get(), val)) {
                            // Mask to bitWidth and shift into position
                            long long mask = (1LL << stype->members[m].bitWidth) - 1;
                            packedValue |= ((val & mask) << bitOffset);
                        }
                    }
                    bitOffset += stype->members[m].bitWidth;
                }
                res += std::to_string(packedValue);
                
                // Add padding after bitfields (3 bytes for single byte bitfields on 32-bit)
                int packingBytes = totalBits / 8;
                if (packingBytes == 1) {
                    // One byte of bitfields -> add 3 bytes padding for 4-byte alignment
                    res += ", [3 x i8] [i8 0, i8 0, i8 0]";
                }
                
                j = k;
            } else {
                auto* memberType = stripQualifiers(member.type.get());
                std::string memberTypeLLVM = typeToLLVM(memberType);
                res += memberTypeLLVM + " ";
                
                // Check if this member is a struct (nested struct)
                if (auto* nestedStruct = dynamic_cast<AST::StructType*>(memberType)) {
                    if (typeInitList && initIdx < typeInitList->initializers.size()) {
                        if (auto* subList = dynamic_cast<AST::InitializerList*>(typeInitList->initializers[initIdx].get())) {
                            // Initializer with braces or flattened - both are InitializerLists
                            res += generateStructFromInit(nestedStruct, subList);
                        } else {
                            // Single non-list value - treat as empty for nested struct (will fill with zeros)
                            res += generateStructFromInit(nestedStruct, nullptr);
                        }
                        initIdx++;
                    } else {
                        res += generateStructFromInit(nestedStruct, nullptr);
                    }
                }
                // Check if this member is an array
                else if (auto* arrType = dynamic_cast<AST::ArrayType*>(memberType)) {
                    size_t elemCount = 0;
                    if (arrType->size >= 0) {
                        elemCount = static_cast<size_t>(arrType->size);
                    }
                    res += "[";
                    
                    // The initializer for this array member
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
                // Scalar member
                else {
                    if (typeInitList && initIdx < typeInitList->initializers.size()) {
                        long long val;
                        if (evaluateConstantExpr(typeInitList->initializers[initIdx++].get(), val)) {
                            // Format based on target type
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
        
        // Fill remaining members with zero
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
    };
    
    // Process each array initializer as a separate struct element
    std::string result = "[";
    std::string elemTypeLLVM = typeToLLVM(arrayType->elementType.get());
    
    // Iterate over each array element (could be up to arraySize)
    for (size_t i = 0; i < arraySize && i < initList->initializers.size(); ++i) {
        if (i > 0) result += ", ";
        result += elemTypeLLVM + " ";
        
        // Get this array element's initializer
        const auto& init = initList->initializers[i];
        
        if (auto* subList = dynamic_cast<AST::InitializerList*>(init.get())) {
            // This is a struct initializer with braces
            result += generateStructFromInit(structType, subList);
        } else {
            // Single expression - treat as initialization of first member
            // Create a temporary list with just this one item
            AST::InitializerList tempList(0, 0);
            tempList.initializers.push_back(std::unique_ptr<AST::Expression>(const_cast<AST::Expression*>(init.get())));
            result += generateStructFromInit(structType, &tempList);
        }
    }
    
    // Fill remaining array elements with zeros
    for (size_t i = initList->initializers.size(); i < arraySize; ++i) {
        if (i > 0) result += ", ";
        result += elemTypeLLVM + " ";
        
        // Create empty initializer (all zeros)
        result += generateStructFromInit(structType, nullptr);
    }
    
    result += "]";
    return result;
}

// Count how many flat initializers are needed to fill a struct type
// This accounts for nested structs and arrays
size_t IRGenerator::countFlattedMembers(AST::Type* type) {
    if (!type) return 0;
    
    type = stripQualifiers(type);
    
    // Primitive types: 1 initializer
    if (dynamic_cast<AST::PrimitiveType*>(type)) {
        return 1;
    }
    if (dynamic_cast<AST::PointerType*>(type)) {
        return 1;
    }
    
    // Arrays: count elements (not total flattened)
    // In C flat initialization, arrays take as many initializers as they have elements
    if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
        if (arr->size > 0) {
            return arr->size;
        }
        return 1;  // Unknown size
    }
    
    // Structs: sum of all member counts (recursively flattened)
    if (auto* st = dynamic_cast<AST::StructType*>(type)) {
        size_t total = 0;
        for (const auto& member : st->members) {
            total += countFlattedMembers(member.type.get());
        }
        return total;
    }
    
    return 1;
}

// Generate LLVM struct initializer from a flat initializer list
// Takes the flattened initializers and distributes them across struct members
// Uses a reference parameter to track position in the flat list
std::string IRGenerator::generateStructInitializerFromFlatHelper(AST::StructType* st, AST::InitializerList* flatList, size_t& idx) {
    if (!st || !flatList) return "zeroinitializer";
    
    std::string result = "{ ";
    bool first = true;
    
    for (const auto& member : st->members) {
        if (!first) result += ", ";
        first = false;
        
        AST::Type* memberType = stripQualifiers(member.type.get());
        std::string memberTypeStr = typeToLLVM(memberType);
        
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
                // Format the value based on member type
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
                idx++;
            } else {
                // Could be an InitializerList for nested struct
                if (auto* initListExpr = dynamic_cast<AST::InitializerList*>(flatList->initializers[idx].get())) {
                    // It's a nested initializer list - use it directly for the next element
                    result += generateInitializerValue(memberType, initListExpr);
                    idx++;
                } else {
                    result += getDefaultValue(memberType);
                    idx++;
                }
            }
        } else {
            // Multiple initializers - recursively distribute them
            if (auto* structMemberType = dynamic_cast<AST::StructType*>(memberType)) {
                result += generateStructInitializerFromFlatHelper(structMemberType, flatList, idx);
            } else if (auto* arrayMemberType = dynamic_cast<AST::ArrayType*>(memberType)) {
                // For arrays, consume the required number of initializers
                size_t elemCount = (arrayMemberType->size > 0) ? arrayMemberType->size : 0;
                result += "[";
                std::string elemTypeStr = typeToLLVM(arrayMemberType->elementType.get());
                for (size_t i = 0; i < elemCount; ++i) {
                    if (i > 0) result += ", ";
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
    }
    
    result += " }";
    return result;
}

} // namespace cc1

