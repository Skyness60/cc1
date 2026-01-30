#include <codegen/IRGenerator.hpp>

#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace cc1 {

// EN: Formats a float constant as an LLVM hex literal.
// FR: Formate une constante float en literal hex LLVM.
std::string IRGenerator::formatLLVMFloatConstant(float value) {
    
    
    double asDouble = static_cast<double>(value);
    uint64_t bits = 0;
    static_assert(sizeof(bits) == sizeof(asDouble), "unexpected double size");
    std::memcpy(&bits, &asDouble, sizeof(bits));

    std::ostringstream oss;
    oss << "0x" << std::hex << std::nouppercase << std::setw(16) << std::setfill('0') << bits;
    return oss.str();
}

} 
