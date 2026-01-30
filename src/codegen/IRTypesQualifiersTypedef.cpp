#include <codegen/IRGenerator.hpp>

#include <cctype>

namespace cc1 {

// EN: Strips qualifiers and resolves typedef chains to a base type.
// FR: Retire qualifiers et resout les typedefs vers le type de base.
AST::Type* IRGenerator::stripQualifiers(AST::Type* type) {
    while (auto* qual = dynamic_cast<AST::QualifiedType*>(type)) {
        type = qual->baseType.get();
    }
    
    while (auto* typedefType = dynamic_cast<AST::TypedefType*>(type)) {
        AST::Type* resolved = resolveTypedef(typedefType->name);
        if (resolved) {
            type = resolved;
        } else {
            break;  
        }
    }
    return type;
}

// EN: Resolves a typedef name to its underlying type if known.
// FR: Resout un typedef vers son type sous-jacent si connu.
AST::Type* IRGenerator::resolveTypedef(const std::string& name) {
    auto it = typedefMap_.find(name);
    if (it != typedefMap_.end() && it->second) {
        return it->second;
    }
    return nullptr;
}

// EN: Extracts a field type from an inline LLVM struct type string.
// FR: Extrait le type d un champ depuis un type struct LLVM inline.
std::string IRGenerator::extractFieldTypeFromInlineStruct(const std::string& inlineStructType, int fieldIndex) {
    
    

    if (inlineStructType.empty() || inlineStructType[0] != '{') {
        return "i32";  
    }

    
    size_t pos = 1;
    int currentField = 0;
    int braceDepth = 0;
    size_t fieldStart = pos;

    while (pos < inlineStructType.size()) {
        char c = inlineStructType[pos];

        
        if (c == '{' || c == '[') {
            braceDepth++;
        } else if (c == '}' || c == ']') {
            braceDepth--;
            if (c == '}' && braceDepth == -1) {
                
                break;
            }
        }
        
        else if (c == ',' && braceDepth == 0) {
            if (currentField == fieldIndex) {
                
                std::string fieldType = inlineStructType.substr(fieldStart, pos - fieldStart);
                
                while (!fieldType.empty() && std::isspace(static_cast<unsigned char>(fieldType.front()))) {
                    fieldType = fieldType.substr(1);
                }
                while (!fieldType.empty() && std::isspace(static_cast<unsigned char>(fieldType.back()))) {
                    fieldType = fieldType.substr(0, fieldType.size() - 1);
                }
                return fieldType;
            }
            currentField++;
            fieldStart = pos + 1;  
        }

        pos++;
    }

    
    if (currentField == fieldIndex) {
        std::string fieldType = inlineStructType.substr(fieldStart, pos - fieldStart);
        
        while (!fieldType.empty() && std::isspace(static_cast<unsigned char>(fieldType.front()))) {
            fieldType = fieldType.substr(1);
        }
        while (!fieldType.empty() && std::isspace(static_cast<unsigned char>(fieldType.back()))) {
            fieldType = fieldType.substr(0, fieldType.size() - 1);
        }
        return fieldType;
    }

    return "i32";  
}

// EN: Registers member indices for inline struct types.
// FR: Enregistre les indices des membres pour structs inline.
void IRGenerator::registerInlineStructMembers(const std::string& llvmType, AST::StructType* structType) {
    if (!structType || llvmType.empty()) return;

    try {
        
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

        
        if (!memberMap.empty()) {
            inlineStructMembers_[llvmType] = memberMap;
        }
    } catch (...) {
        
    }
}

// EN: Finds a member index in a cached inline struct mapping.
// FR: Cherche l index d un membre dans une struct inline cachee.
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

    
    DebugLogger::instance().log("[InlineStruct] Member '" + memberName + "' not found in " + llvmType);
    return -1;
}

} 
