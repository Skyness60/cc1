#include <preprocessor/PPFileHandler.hpp>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <libgen.h>
#include <cstring>
#include <cstdlib>
#include <climits>

namespace cc1 {
namespace pp {

// EN: Adds an include search path for future resolution.
// FR: Ajoute un chemin de recherche pour les includes.
void FileHandler::addIncludePath(const std::string& path) {
    includePaths_.push_back(path);
}

// EN: Sets the current file used for relative include lookup.
// FR: Definit le fichier courant pour les includes relatifs.
void FileHandler::setCurrentFile(const std::string& file) {
    currentFile_ = file;
}

// EN: Returns the current file path.
// FR: Renvoie le chemin du fichier courant.
const std::string& FileHandler::getCurrentFile() const {
    return currentFile_;
}

// EN: Reads a file into a string, honoring include-once suppression.
// FR: Lit un fichier en chaine en respectant include-once.
bool FileHandler::readFile(const std::string& filename, std::string& out) {
    
    if (hasIncludedOnce(filename)) {
        out.clear();
        return true;
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        out.clear();
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    out = buffer.str();
    return true;
}

// EN: Resolves an include filename using local paths, include paths, and
// system paths. FR: Resout un include via chemins locaux, includePaths et
// chemins systeme.
std::string FileHandler::findInclude(const std::string& filename, bool isSystemInclude) {
    
    if (!isSystemInclude && !currentFile_.empty()) {
        std::string dir = getDirectory(currentFile_);
        std::string path = joinPath(dir, filename);
        if (fileExists(path)) {
            return path;
        }
    }
    
    
    for (size_t i = 0; i < includePaths_.size(); i++) {
        std::string path = joinPath(includePaths_[i], filename);
        if (fileExists(path)) {
            return path;
        }
    }
    
    
    if (isSystemInclude) {
        static const char* systemPaths[] = {
            "/usr/include",
            "/usr/local/include",
            "/usr/include/x86_64-linux-gnu"
        };
        
        for (size_t i = 0; i < sizeof(systemPaths) / sizeof(systemPaths[0]); i++) {
            std::string path = joinPath(systemPaths[i], filename);
            if (fileExists(path)) {
                return path;
            }
        }
    }
    
    
    if (fileExists(filename)) {
        return filename;
    }
    
    return "";
}

// EN: Checks whether a file is marked as included once.
// FR: Verifie si un fichier est marque comme inclus une fois.
bool FileHandler::hasIncludedOnce(const std::string& path) const {
    return includedOnce_.find(path) != includedOnce_.end();
}

// EN: Marks a file as included once to prevent reinclusion.
// FR: Marque un fichier comme inclus une fois pour eviter la re-inclusion.
void FileHandler::markIncludedOnce(const std::string& path) {
    includedOnce_.insert(path);
}

// EN: Tests filesystem existence for a path.
// FR: Teste l existence d un chemin.
bool FileHandler::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

// EN: Extracts the directory from a full path.
// FR: Extrait le dossier depuis un chemin complet.
std::string FileHandler::getDirectory(const std::string& path) const {
    if (path.empty()) return ".";
    
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        return ".";
    }
    
    return path.substr(0, pos);
}

// EN: Joins directory and file into a single path.
// FR: Concatene un dossier et un fichier en un chemin.
std::string FileHandler::joinPath(const std::string& dir, const std::string& file) const {
    if (dir.empty() || dir == ".") {
        return file;
    }
    
    if (dir.back() == '/') {
        return dir + file;
    }
    
    return dir + "/" + file;
}

} 
} 

// TODO(cc1) EN: Normalize paths to avoid duplicate include-once entries.
// FR: Normaliser les chemins pour eviter les doublons include-once.
