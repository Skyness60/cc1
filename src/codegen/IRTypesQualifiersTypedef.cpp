#include <codegen/IRGenerator.hpp>

#include <cctype>

namespace cc1 {

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
                while (!fieldType.empty() && std::isspace(static_cast<unsigned char>(fieldType.front()))) {
                    fieldType = fieldType.substr(1);
                }
                while (!fieldType.empty() && std::isspace(static_cast<unsigned char>(fieldType.back()))) {
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
        while (!fieldType.empty() && std::isspace(static_cast<unsigned char>(fieldType.front()))) {
            fieldType = fieldType.substr(1);
        }
        while (!fieldType.empty() && std::isspace(static_cast<unsigned char>(fieldType.back()))) {
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

} // namespace cc1
