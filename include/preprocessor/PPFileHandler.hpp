#pragma once



#include <string>
#include <vector>
#include <unordered_set>

namespace cc1 {
namespace pp {





// EN: Handles include paths, file lookup, and include-once tracking.
// FR: Gere les chemins d include, la recherche de fichiers et include-once.
class FileHandler {
public:
    
    // EN: Adds a search path for includes.
    // FR: Ajoute un chemin de recherche pour includes.
    void addIncludePath(const std::string& path);
    
    
    // EN: Sets the current file for relative include resolution.
    // FR: Definit le fichier courant pour les includes relatifs.
    void setCurrentFile(const std::string& file);
    // EN: Returns the current file path.
    // FR: Renvoie le chemin du fichier courant.
    const std::string& getCurrentFile() const;
    
    
    // EN: Reads a file into a string for preprocessing; returns true on open success.
    // FR: Lit un fichier en chaine pour le pretraitement; renvoie true si l'ouverture reussit.
    bool readFile(const std::string& filename, std::string& out);
    
    // EN: Finds an include file using local/system search rules.
    // FR: Trouve un include selon les regles local/systeme.
    std::string findInclude(const std::string& filename, bool isSystemInclude);
    
    
    // EN: Checks if a file was included with pragma once/guard logic.
    // FR: Verifie si un fichier a deja ete inclus (pragma once).
    bool hasIncludedOnce(const std::string& path) const;
    // EN: Marks a file as included-once.
    // FR: Marque un fichier comme inclus une fois.
    void markIncludedOnce(const std::string& path);
    
private:
    std::vector<std::string> includePaths_;
    std::string currentFile_;
    std::unordered_set<std::string> includedOnce_;
    
    
    // EN: Checks if a path exists on disk.
    // FR: Verifie si un chemin existe sur disque.
    bool fileExists(const std::string& path) const;
    
    
    // EN: Extracts the directory portion of a path.
    // FR: Extrait le dossier d un chemin.
    std::string getDirectory(const std::string& path) const;
    
    
    // EN: Joins directory and file into a normalized path.
    // FR: Concatene dossier et fichier en un chemin normalise.
    std::string joinPath(const std::string& dir, const std::string& file) const;
};

// TODO(cc1) EN: Support include search caching to reduce repeated filesystem IO.
// FR: Ajouter un cache de recherche d include pour limiter les IO.
} 
} 
