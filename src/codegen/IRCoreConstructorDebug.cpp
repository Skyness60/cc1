#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Initializes the IR generator and target datalayout/triple.
// FR: Initialise le generateur IR et le datalayout/target.
IRGenerator::IRGenerator(bool is64bit) : is64bit_(is64bit) {
    
    enterScope();

    
    if (is64bit_) {
        headerBuffer_ << "; ModuleID = 'cc1'\n";
        headerBuffer_ << "source_filename = \"cc1\"\n";
        headerBuffer_ << "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n";
        headerBuffer_ << "target triple = \"x86_64-pc-linux-gnu\"\n\n";
    } else {
        headerBuffer_ << "; ModuleID = 'cc1'\n";
        headerBuffer_ << "source_filename = \"cc1\"\n";
        headerBuffer_ << "target datalayout = \"e-m:e-p:32:32-p270:32:32-p271:32:32-p272:64:64-f64:32:64-f80:32-n8:16:32-S128\"\n";
        headerBuffer_ << "target triple = \"i386-pc-linux-gnu\"\n\n";
    }
}

// EN: Enables/disables debug info and sets the primary filename.
// FR: Active/desactive les infos debug et fixe le fichier principal.
void IRGenerator::setDebugInfo(bool enabled, const std::string& primaryFilename) {
    debugInfo_ = enabled;
    debugFilename_ = primaryFilename;

    
    auto pos = debugFilename_.find_last_of('/');
    if (pos == std::string::npos) {
        debugDirectory_.clear();
    } else {
        debugDirectory_ = debugFilename_.substr(0, pos);
        debugFilename_ = debugFilename_.substr(pos + 1);
    }
}

// EN: Initializes debug metadata nodes if not already created.
// FR: Initialise les metadonnees debug si besoin.
void IRGenerator::initDebugMetadataIfNeeded() {
    if (!debugInfo_) return;
    if (diFileId_ != -1 && diCompileUnitId_ != -1) return;

    diFileId_ = newDebugMetaId();
    
    std::string dir = debugDirectory_.empty() ? "." : debugDirectory_;
    debugMetaBuffer_ << "!" << diFileId_ << " = !DIFile(filename: \"" << debugFilename_ << "\", directory: \"" << dir << "\")\n";

    diCompileUnitId_ = newDebugMetaId();
    debugMetaBuffer_ << "!" << diCompileUnitId_
                     << " = distinct !DICompileUnit(language: DW_LANG_C89, file: !" << diFileId_
                     << ", producer: \"cc1\", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug)\n";
}

// EN: Allocates a new unique debug metadata ID.
// FR: Alloue un nouvel ID de metadonnees debug.
int IRGenerator::newDebugMetaId() {
    return nextDebugMetaId_++;
}

// EN: Returns or creates a DILocation ID for line/column/scope.
// FR: Renvoie ou cree un ID DILocation pour ligne/colonne/scope.
int IRGenerator::getOrCreateDILocationId(int line, int col, int scopeId) {
    auto key = std::make_tuple(line, col, scopeId);
    auto it = diLocationIds_.find(key);
    if (it != diLocationIds_.end()) return it->second;

    int id = newDebugMetaId();
    diLocationIds_[key] = id;
    debugMetaBuffer_ << "!" << id << " = !DILocation(line: " << line << ", column: " << col
                     << ", scope: !" << scopeId << ")\n";
    return id;
}

// EN: Pushes a debug location onto the stack.
// FR: Empile une localisation debug.
void IRGenerator::pushDebugLoc(int line, int col) {
    DebugLoc loc;
    loc.line = line;
    loc.column = col;
    debugLocStack_.push_back(loc);
}

// EN: Pops the latest debug location.
// FR: Depile la derniere localisation debug.
void IRGenerator::popDebugLoc() {
    if (!debugLocStack_.empty()) debugLocStack_.pop_back();
}

// EN: Builds the IR debug suffix for the current location.
// FR: Construit le suffixe debug IR pour la position courante.
std::string IRGenerator::dbgSuffixForCurrentLoc() {
    if (!debugInfo_) return "";
    if (currentSubprogramId_ < 0) return "";
    if (debugLocStack_.empty()) return "";

    const DebugLoc& loc = debugLocStack_.back();
    int line = loc.line > 0 ? loc.line : 1;
    int col = loc.column > 0 ? loc.column : 1;
    int locId = getOrCreateDILocationId(line, col, currentSubprogramId_);
    return ", !dbg !" + std::to_string(locId);
}

} 
