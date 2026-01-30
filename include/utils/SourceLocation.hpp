#pragma once



#include <string>

// EN: Holds a filename and 1-based line/column for diagnostics.
// FR: Contient fichier et ligne/colonne (base 1) pour diagnostics.
struct SourceLocation {
    std::string filename;
    int line;
    int column;
    
    // EN: Constructs a location with defaults for unknown positions.
    // FR: Construit une position avec valeurs par defaut si inconnue.
    SourceLocation(const std::string& file = "", int l = 1, int c = 1)
        : filename(file), line(l), column(c) {}
    
    // EN: Formats the location as "file:line:column".
    // FR: Formate la position en "fichier:ligne:colonne".
    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};
