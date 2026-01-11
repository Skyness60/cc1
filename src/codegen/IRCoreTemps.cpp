#include <codegen/IRGenerator.hpp>

#include <cstdio>

namespace cc1 {

// ============================================================================
// Temporary and Label Generation
// ============================================================================

std::string IRGenerator::newTemp() {
    return "%" + std::to_string(tempCounter_++);
}

std::string IRGenerator::newLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter_++);
}

std::string IRGenerator::newGlobalString(const std::string& str) {
    std::string name = "@.str." + std::to_string(stringCounter_++);

    // Escape the string for LLVM IR
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
            // Non-printable: emit as hex
            char hex[5];
            snprintf(hex, sizeof(hex), "\\%02X", static_cast<unsigned char>(c));
            escaped += hex;
        } else {
            escaped += c;
        }
    }

    // Add null terminator
    escaped += "\\00";

    // Emit global string constant to string buffer
    stringBuffer_ << name << " = private unnamed_addr constant [" << (str.length() + 1)
                  << " x i8] c\"" << escaped << "\"\n";

    return name;
}

} // namespace cc1
