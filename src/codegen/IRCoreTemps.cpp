#include <codegen/IRGenerator.hpp>

#include <cstdio>

namespace cc1 {

// EN: Returns a fresh SSA temporary register name.
// FR: Renvoie un nouveau nom de registre temporaire SSA.
std::string IRGenerator::newTemp() {
    return "%t" + std::to_string(tempCounter_++);
}

// EN: Returns a fresh label name with the given prefix.
// FR: Renvoie un nouveau label avec le prefixe donne.
std::string IRGenerator::newLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter_++);
}

// EN: Emits a global string constant and returns its symbol name.
// FR: Emet une constante string globale et renvoie son symbole.
std::string IRGenerator::newGlobalString(const std::string& str) {
    std::string name = "@.str." + std::to_string(stringCounter_++);

    
    std::string escaped;
    for (char c : str) {
        if (c == '\\') {
            escaped += "\\5C";
        } else if (c == '"') {
            escaped += "\\22";
        } else if (c == '\n') {
            escaped += "\\0A";
        } else if (c == '\t') {
            escaped += "\\09";
        } else if (c == '\r') {
            escaped += "\\0D";
        } else if (c == '\0') {
            escaped += "\\00";
        } else if (c < 32 || c > 126) {
            
            char hex[5];
            snprintf(hex, sizeof(hex), "\\%02X", static_cast<unsigned char>(c));
            escaped += hex;
        } else {
            escaped += c;
        }
    }

    
    escaped += "\\00";

    
    stringBuffer_ << name << " = private unnamed_addr constant [" << (str.length() + 1)
                  << " x i8] c\"" << escaped << "\"\n";

    return name;
}

} 
