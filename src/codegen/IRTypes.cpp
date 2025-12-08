#include <codegen/IRGenerator.hpp>

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
            // Named struct - use opaque type reference
            return "%struct." + structType->name;
        }
        // Anonymous struct - compute inline
        std::string result = structType->isUnion ? "{ " : "{ ";
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
            // Struct: all members
            for (size_t i = 0; i < structType->members.size(); ++i) {
                if (i > 0) result += ", ";
                result += typeToLLVM(structType->members[i].type.get());
            }
        }
        result += " }";
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

} // namespace cc1

