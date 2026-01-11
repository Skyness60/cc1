#include <codegen/IRGenerator.hpp>

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace cc1 {

std::string IRGenerator::formatLLVMFloatConstant(float value) {
    // LLVM textual IR accepts float constants in a 64-bit hex form (same style clang emits).
    // We encode the rounded float value as a double bit-pattern.
    double asDouble = static_cast<double>(value);
    uint64_t bits = 0;
    static_assert(sizeof(bits) == sizeof(asDouble), "unexpected double size");
    std::memcpy(&bits, &asDouble, sizeof(bits));

    std::ostringstream oss;
    oss << "0x" << std::hex << std::nouppercase << std::setw(16) << std::setfill('0') << bits;
    return oss.str();
}

} // namespace cc1
