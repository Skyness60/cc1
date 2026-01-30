#pragma once

#include <string>




// EN: Extracts full lines from source text for diagnostics.
// FR: Extrait des lignes du texte source pour les diagnostics.
class SourceExtractor {
public:
    
    // EN: Initializes the extractor with the source buffer.
    // FR: Initialise l extracteur avec le buffer source.
    SourceExtractor(const std::string& source);
    
    
    // EN: Returns a specific line by number (1-based).
    // FR: Renvoie une ligne par numero (base 1).
    std::string getLine(int lineNumber) const;

    
    // EN: Returns the current line given a current line number.
    // FR: Renvoie la ligne courante selon un numero de ligne.
    std::string getCurrentLine(int currentLine) const;

private:
    const std::string& source_;
};

// TODO(cc1) EN: Cache line offsets for faster repeated lookups.
// FR: Cacher les offsets de lignes pour accelerer les recherches repetées.
