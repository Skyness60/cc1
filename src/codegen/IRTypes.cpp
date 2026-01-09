#include <codegen/IRGenerator.hpp>
#include <functional>

namespace cc1 {

static std::string nextAnonStructName() {
    static int anonCounter = 0;
    return "anon." + std::to_string(anonCounter++);
}

static int alignTo(int offset, int alignment) {
    if (alignment <= 0) return offset;
    int rem = offset % alignment;
    return rem == 0 ? offset : (offset + (alignment - rem));
}

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
        if (structType->name.empty()) {
            // Anonymous struct/union types are distinct in C.
            // If we inline them, multiple different anonymous structs can collapse to the
            // same LLVM type string (e.g., "{ i8 }") which breaks member/bitfield access.
            // Give them a unique internal name and emit them as named %struct.*.
            structType->name = nextAnonStructName();
        }
        // Named struct (including auto-named anonymous): register for module-level definition.
        collectNamedStruct(structType);
        return "%struct." + structType->name;
    }
    
    if (auto* enumType = dynamic_cast<AST::EnumType*>(type)) {
        (void)enumType;
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

        // Anonymous/inline structs: compute full layout (including padding)
        StructLayout layout = computeStructLayout(structType);
        return layout.totalSize;
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

        StructLayout layout = computeStructLayout(structType);
        return layout.alignment;
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
    
    if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
        // For structs, generate proper initializer with all members defaulted
        // For unions, only initialize the first member
        std::string result = "{ ";
        bool first = true;
        
        size_t memberCount = structType->isUnion ? 1 : structType->members.size();
        for (size_t i = 0; i < memberCount; ++i) {
            if (!first) result += ", ";
            first = false;
            const auto& member = structType->members[i];
            std::string memberType = typeToLLVM(stripQualifiers(member.type.get()));
            result += memberType + " " + getDefaultValue(stripQualifiers(member.type.get()));
        }
        result += " }";
        return result;

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
        
        // Handle struct array initialization
        if (structType) {
            // Determine if this is per-element or flat initialization
            // Key indicator: if the first initializer is a braced InitList, it's per-element
            // Otherwise, all scalars means flat initialization
            
            bool isPerElement = false;
            if (!initList->initializers.empty()) {
                isPerElement = dynamic_cast<AST::InitializerList*>(initList->initializers[0].get()) != nullptr;
            }
            
            if (isPerElement) {
                // Per-element initialization
                // Process each array element: some may have InitializerLists, others are individual scalars
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
                                        initIdx++;
                                    } else {
                                        result += "0";
                                    }
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

    // Prefer the preserved StructType if available (keeps bitfield widths).
    if (decl && decl->declaredType) {
        layout = computeStructLayout(decl->declaredType.get());
        layout.llvmType = "%struct." + decl->name + " = type " + layout.llvmType;
        return layout;
    }
    
    if (decl->isUnion) {
        // Union: all members overlap at offset 0; alignment is max member alignment.
        layout.llvmType = "%struct." + decl->name + " = type { ";
        int maxSize = 1;
        int maxAlign = 1;
        std::string chosenType = "i8";
        int chosenSize = 1;

        if (!decl->members.empty() && decl->members[0] && decl->members[0]->type) {
            AST::Type* firstType = stripQualifiers(decl->members[0]->type.get());
            chosenType = typeToLLVM(firstType);
            chosenSize = std::max(1, getTypeSize(firstType));
            maxAlign = std::max(1, getTypeAlign(firstType));
            maxSize = chosenSize;
        }

        for (size_t i = 0; i < decl->members.size(); ++i) {
            auto& member = decl->members[i];
            AST::Type* memberTypeAst = stripQualifiers(member->type.get());
            std::string memberType = typeToLLVM(memberTypeAst);
            int memberSize = getTypeSize(memberTypeAst);
            int memberAlign = getTypeAlign(memberTypeAst);

            layout.memberIndices[member->name] = 0;
            layout.memberTypes[member->name] = memberType;

            maxSize = std::max(maxSize, memberSize);
            if (memberAlign > maxAlign) {
                maxAlign = memberAlign;
                chosenType = memberType;
                chosenSize = memberSize;
            } else if (memberAlign == maxAlign && memberSize > chosenSize) {
                chosenType = memberType;
                chosenSize = memberSize;
            }
        }

        int desiredSize = alignTo(maxSize > 0 ? maxSize : 1, maxAlign);
        layout.llvmType += chosenType;
        int pad = desiredSize - chosenSize;
        if (pad > 0) {
            layout.llvmType += ", [" + std::to_string(pad) + " x i8]";
        }
        layout.llvmType += " }";
        layout.totalSize = desiredSize;
        layout.alignment = maxAlign;
    } else {
        // Struct: add explicit padding fields to match ABI alignment.
        layout.llvmType = "%struct." + decl->name + " = type { ";
        int offset = 0;
        int maxAlign = 1;
        int fieldIndex = 0;
        bool firstField = true;

        for (size_t i = 0; i < decl->members.size(); ++i) {
            auto& member = decl->members[i];
            std::string memberType = typeToLLVM(member->type.get());
            int memberSize = getTypeSize(member->type.get());
            int memberAlign = getTypeAlign(member->type.get());

            int aligned = alignTo(offset, memberAlign);
            int pad = aligned - offset;
            if (pad > 0) {
                if (!firstField) layout.llvmType += ", ";
                firstField = false;
                layout.llvmType += "[" + std::to_string(pad) + " x i8]";
                offset += pad;
                fieldIndex++;
            }

            if (!firstField) layout.llvmType += ", ";
            firstField = false;
            layout.llvmType += memberType;

            layout.memberIndices[member->name] = fieldIndex;
            layout.memberTypes[member->name] = memberType;

            offset += memberSize;
            maxAlign = std::max(maxAlign, memberAlign);
            fieldIndex++;
        }

        int finalSize = alignTo(offset, maxAlign);
        int tailPad = finalSize - offset;
        if (tailPad > 0) {
            if (!firstField) layout.llvmType += ", ";
            layout.llvmType += "[" + std::to_string(tailPad) + " x i8]";
            fieldIndex++;
        }

        layout.llvmType += " }";
        layout.totalSize = finalSize > 0 ? finalSize : 1;
        layout.alignment = maxAlign;
    }
    
    return layout;
}

IRGenerator::StructLayout IRGenerator::computeStructLayout(AST::StructType* type) {
    StructLayout layout;
    
    if (type->isUnion) {
        layout.llvmType = "{ ";
        int maxSize = 1;
        int maxAlign = 1;
        std::string chosenType = "i8";
        int chosenSize = 1;

        if (!type->members.empty() && type->members[0].type) {
            AST::Type* firstType = stripQualifiers(type->members[0].type.get());
            chosenType = typeToLLVM(firstType);
            chosenSize = std::max(1, getTypeSize(firstType));
            maxAlign = std::max(1, getTypeAlign(firstType));
            maxSize = chosenSize;
        }

        for (size_t i = 0; i < type->members.size(); ++i) {
            auto& member = type->members[i];
            AST::Type* memberTypeAst = stripQualifiers(member.type.get());
            std::string memberType = typeToLLVM(memberTypeAst);
            int memberSize = getTypeSize(memberTypeAst);
            int memberAlign = getTypeAlign(memberTypeAst);

            layout.memberIndices[member.name] = 0;
            layout.memberTypes[member.name] = memberType;

            maxSize = std::max(maxSize, memberSize);
            if (memberAlign > maxAlign) {
                maxAlign = memberAlign;
                chosenType = memberType;
                chosenSize = memberSize;
            } else if (memberAlign == maxAlign && memberSize > chosenSize) {
                chosenType = memberType;
                chosenSize = memberSize;
            }
        }

        int desiredSize = alignTo(maxSize > 0 ? maxSize : 1, maxAlign);
        layout.llvmType += chosenType;
        int pad = desiredSize - chosenSize;
        if (pad > 0) {
            layout.llvmType += ", [" + std::to_string(pad) + " x i8]";
        }
        layout.llvmType += " }";
        layout.totalSize = desiredSize;
        layout.alignment = maxAlign;
    } else {
        layout.llvmType = "{ ";
        int offset = 0;
        int maxAlign = 1;
        int fieldIndex = 0;
        bool firstField = true;

        // Bitfield packing: storage-unit based (declared base type).
        int currentUnitSize = 0;
        int currentUnitAlign = 1;
        int currentUnitBitsTotal = 0;
        int currentUnitBitsUsed = 0;
        int currentUnitFieldIndex = -1;
        std::string currentUnitLLVMType;

        auto flushUnit = [&]() {
            if (currentUnitBitsUsed > 0 && currentUnitSize > 0) {
                offset += currentUnitSize;
            }
            currentUnitSize = 0;
            currentUnitAlign = 1;
            currentUnitBitsTotal = 0;
            currentUnitBitsUsed = 0;
            currentUnitFieldIndex = -1;
            currentUnitLLVMType.clear();
        };

        auto ensurePaddingTo = [&](int alignment) {
            int aligned = alignTo(offset, alignment);
            int pad = aligned - offset;
            if (pad > 0) {
                if (!firstField) layout.llvmType += ", ";
                firstField = false;
                layout.llvmType += "[" + std::to_string(pad) + " x i8]";
                offset += pad;
                fieldIndex++;
            }
        };

        auto openStorageUnitIfNeeded = [&](int unitSize, int unitAlign, const std::string& unitLLVMType) {
            if (currentUnitBitsTotal == 0 || currentUnitSize != unitSize || currentUnitAlign != unitAlign) {
                flushUnit();
                ensurePaddingTo(unitAlign);

                if (!firstField) layout.llvmType += ", ";
                firstField = false;
                layout.llvmType += unitLLVMType;
                currentUnitFieldIndex = fieldIndex;
                fieldIndex++;

                currentUnitSize = unitSize;
                currentUnitAlign = unitAlign;
                currentUnitBitsTotal = unitSize * 8;
                currentUnitBitsUsed = 0;
                currentUnitLLVMType = unitLLVMType;
                maxAlign = std::max(maxAlign, unitAlign);
            }
        };

        for (size_t i = 0; i < type->members.size(); ++i) {
            const auto& member = type->members[i];

            if (member.isBitfield()) {
                int unitSize = getTypeSize(member.type.get());
                int unitAlign = getTypeAlign(member.type.get());
                if (unitSize <= 0) unitSize = 1;
                if (unitAlign <= 0) unitAlign = 1;
                std::string unitLLVMType;
                if (unitSize == 1) unitLLVMType = "i8";
                else if (unitSize == 2) unitLLVMType = "i16";
                else if (unitSize == 4) unitLLVMType = "i32";
                else unitLLVMType = "i64";

                // Zero-width bitfield forces alignment to next storage unit boundary.
                if (member.bitWidth == 0) {
                    flushUnit();
                    ensurePaddingTo(unitAlign);
                    maxAlign = std::max(maxAlign, unitAlign);
                    continue;
                }

                // Open or switch storage unit.
                openStorageUnitIfNeeded(unitSize, unitAlign, unitLLVMType);

                // If it doesn't fit, start a new unit.
                if (currentUnitBitsUsed + member.bitWidth > currentUnitBitsTotal) {
                    flushUnit();
                    openStorageUnitIfNeeded(unitSize, unitAlign, unitLLVMType);
                }

                if (!member.name.empty()) {
                    layout.memberIndices[member.name] = currentUnitFieldIndex;
                    layout.memberTypes[member.name] = currentUnitLLVMType;
                    layout.bitfieldOffsets[member.name] = currentUnitBitsUsed;
                    layout.bitfieldWidths[member.name] = member.bitWidth;

                    bool isUnsigned = false;
                    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(stripQualifiers(member.type.get()))) {
                        switch (prim->kind) {
                            case AST::PrimitiveKind::UnsignedChar:
                            case AST::PrimitiveKind::UnsignedShort:
                            case AST::PrimitiveKind::UnsignedInt:
                            case AST::PrimitiveKind::UnsignedLong:
                            case AST::PrimitiveKind::UnsignedLongLong:
                                isUnsigned = true;
                                break;
                            default:
                                isUnsigned = false;
                                break;
                        }
                    }
                    layout.bitfieldIsUnsigned[member.name] = isUnsigned;
                }

                currentUnitBitsUsed += member.bitWidth;
                if (currentUnitBitsUsed == currentUnitBitsTotal) {
                    flushUnit();
                }
                continue;
            }

            // Non-bitfield member closes any active storage unit.
            flushUnit();

            std::string memberType = typeToLLVM(member.type.get());
            int memberSize = getTypeSize(member.type.get());
            int memberAlign = getTypeAlign(member.type.get());

            int aligned = alignTo(offset, memberAlign);
            int pad = aligned - offset;
            if (pad > 0) {
                if (!firstField) layout.llvmType += ", ";
                firstField = false;
                layout.llvmType += "[" + std::to_string(pad) + " x i8]";
                offset += pad;
                fieldIndex++;
            }

            if (!firstField) layout.llvmType += ", ";
            firstField = false;
            layout.llvmType += memberType;

            layout.memberIndices[member.name] = fieldIndex;
            layout.memberTypes[member.name] = memberType;

            offset += memberSize;
            maxAlign = std::max(maxAlign, memberAlign);
            fieldIndex++;
        }

        // Flush trailing partial storage unit.
        flushUnit();

        int finalSize = alignTo(offset, maxAlign);
        int tailPad = finalSize - offset;
        if (tailPad > 0) {
            if (!firstField) layout.llvmType += ", ";
            layout.llvmType += "[" + std::to_string(tailPad) + " x i8]";
            fieldIndex++;
        }

        layout.llvmType += " }";
        layout.totalSize = finalSize > 0 ? finalSize : 1;
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
    if (structType->members.size() > 0) {
        for (const auto& member : structType->members) {
            if (member.type) {
                AST::Type* nestedType = stripQualifiers(member.type.get());
                if (auto* nestedStruct = dynamic_cast<AST::StructType*>(nestedType)) {
                    // Give anonymous nested structs a generated name
                    if (nestedStruct->name.empty()) {
                        nestedStruct->name = nextAnonStructName();
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
    
    // Also compute and register the layout for member access
    if (structLayouts_.find(structType->name) == structLayouts_.end()) {
        StructLayout layout = computeStructLayout(structType);
        structLayouts_[structType->name] = layout;
    }
}

std::string IRGenerator::generateStructTypeDefinition(AST::StructType* structType) {
    if (!structType) {
        return "%struct.unknown = type { i32 }";
    }
    
    StructLayout layout = computeStructLayout(structType);
    return "%struct." + structType->name + " = type " + layout.llvmType;
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
        
        StructLayout layout = computeStructLayout(structType);
        for (const auto& member : structType->members) {
            if (!member.name.empty()) {
                auto it = layout.memberIndices.find(member.name);
                if (it != layout.memberIndices.end()) {
                    memberMap[member.name] = it->second;
                }
            }
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
    // For unions, only count 1 since they initialize only the first member
    if (auto* st = dynamic_cast<AST::StructType*>(type)) {
        if (st->isUnion) {
            return 1;  // Union consumes only one initializer
        }
        size_t total = 0;
        size_t i = 0;
        while (i < st->members.size()) {
            const auto& member = st->members[i];
            
            // Handle consecutive bitfields as a single unit
            if (member.isBitfield()) {
                total += 1;  // Packed bitfields count as 1 initializer
                // Skip all consecutive bitfields
                while (i < st->members.size() && st->members[i].isBitfield()) {
                    i++;
                }
            } else {
                total += countFlattedMembers(member.type.get());
                i++;
            }
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
                            if (auto* primType = dynamic_cast<AST::PrimitiveType*>(stripQualifiers(firstMember.type.get()))) {
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

