#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Main Generation Entry Point
// ============================================================================

void IRGenerator::generate(AST::TranslationUnit& unit) {
    currentBuffer_ = &globalBuffer_;
    unit.accept(*this);
}

std::string IRGenerator::getIR() const {
    // Build struct type definitions
    std::stringstream structs;
    for (const auto& pair : namedStructDefs_) {
        if (!pair.second.first.empty()) {
            structs << pair.second.first << "\n";
        }
    }

    // Build metadata for PIE (Position Independent Executable) and (optionally) debug info.
    std::stringstream meta;
    meta << "!llvm.module.flags = !{";
    if (debugInfo_) {
        meta << "!0, !1, !2, !3";
    } else {
        meta << "!0, !1";
    }
    meta << "}\n";
    meta << "!0 = !{i32 8, !\"PIC Level\", i32 2}\n";
    meta << "!1 = !{i32 7, !\"PIE Level\", i32 2}\n";
    if (debugInfo_) {
        meta << "!2 = !{i32 2, !\"Dwarf Version\", i32 4}\n";
        meta << "!3 = !{i32 2, !\"Debug Info Version\", i32 3}\n";
        if (diCompileUnitId_ >= 0) {
            meta << "!llvm.dbg.cu = !{!" << diCompileUnitId_ << "}\n";
        }
    }

    // Build function declarations (only for functions not defined)
    std::stringstream decls;
    for (const auto& pair : functionDeclarations_) {
        if (!definedFunctions_.count(pair.first)) {
            decls << pair.second;
        }
    }

    // Order: header, struct type definitions, globals, string literals,
    // function declarations, function definitions, debug metadata, module metadata
    return headerBuffer_.str() + structs.str() + globalBuffer_.str() + stringBuffer_.str() +
           decls.str() + funcDefBuffer_.str() + debugMetaBuffer_.str() + meta.str();
}

} // namespace cc1
