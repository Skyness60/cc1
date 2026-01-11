#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// IR Emission
// ============================================================================

void IRGenerator::emit(const std::string& ir) {
    if (currentBuffer_) {
        *currentBuffer_ << "  " << ir;
        if (currentBuffer_ == &functionBuffer_) {
            *currentBuffer_ << dbgSuffixForCurrentLoc();
        }
        *currentBuffer_ << "\n";
    }

    // Best-effort tracking of terminator instructions.
    // This generator emits labels explicitly after terminators for dead code paths;
    // tracking helps avoid producing invalid IR when introducing new labels.
    if (ir.rfind("br ", 0) == 0 || ir.rfind("ret ", 0) == 0 || ir.rfind("switch ", 0) == 0 ||
        ir.rfind("unreachable", 0) == 0) {
        blockTerminated_ = true;
    }
}

void IRGenerator::emitLabel(const std::string& label) {
    if (currentBuffer_) {
        *currentBuffer_ << label << ":\n";
    }

    // A label starts a new basic block.
    blockTerminated_ = false;
}

void IRGenerator::emitComment(const std::string& comment) {
    if (currentBuffer_) {
        *currentBuffer_ << "  ; " << comment << "\n";
    }
}

void IRGenerator::emitGlobal(const std::string& ir) {
    globalBuffer_ << ir << "\n";
}

} // namespace cc1
