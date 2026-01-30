#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits an IR instruction into the current buffer.
// FR: Emet une instruction IR dans le buffer courant.
void IRGenerator::emit(const std::string& ir) {
    if (currentBuffer_) {
        *currentBuffer_ << "  " << ir;
        if (currentBuffer_ == &functionBuffer_) {
            *currentBuffer_ << dbgSuffixForCurrentLoc();
        }
        *currentBuffer_ << "\n";
    }

    
    
    
    if (ir.rfind("br ", 0) == 0 || ir.rfind("ret ", 0) == 0 || ir.rfind("switch ", 0) == 0 ||
        ir.rfind("unreachable", 0) == 0) {
        blockTerminated_ = true;
    }
}

// EN: Emits a label and resets block-termination tracking.
// FR: Emet un label et reinitialise le suivi de terminaison.
void IRGenerator::emitLabel(const std::string& label) {
    if (currentBuffer_) {
        *currentBuffer_ << label << ":\n";
    }

    
    blockTerminated_ = false;
}

// EN: Emits a comment line into the current buffer.
// FR: Emet une ligne de commentaire dans le buffer courant.
void IRGenerator::emitComment(const std::string& comment) {
    if (currentBuffer_) {
        *currentBuffer_ << "  ; " << comment << "\n";
    }
}

// EN: Emits a global-level IR line.
// FR: Emet une ligne IR au niveau global.
void IRGenerator::emitGlobal(const std::string& ir) {
    globalBuffer_ << ir << "\n";
}

} 
