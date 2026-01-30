#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Generates IR by visiting the translation unit.
// FR: Genere l IR en visitant l unite de traduction.
void IRGenerator::generate(AST::TranslationUnit& unit) {
    currentBuffer_ = &globalBuffer_;
    unit.accept(*this);
}

// EN: Assembles and returns the final IR module text.
// FR: Assemble et renvoie le texte IR final du module.
std::string IRGenerator::getIR() const {
    
    std::stringstream structs;
    for (const auto& pair : namedStructDefs_) {
        if (!pair.second.first.empty()) {
            structs << pair.second.first << "\n";
        }
    }

    
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

    
    std::stringstream decls;
    for (const auto& pair : functionDeclarations_) {
        if (!definedFunctions_.count(pair.first)) {
            decls << pair.second;
        }
    }

    
    
    return headerBuffer_.str() + structs.str() + globalBuffer_.str() + stringBuffer_.str() +
           decls.str() + funcDefBuffer_.str() + debugMetaBuffer_.str() + meta.str();
}

} 
