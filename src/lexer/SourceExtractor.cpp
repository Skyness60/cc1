#include <lexer/SourceExtractor.hpp>

// EN: Stores a reference to the source for line extraction.
// FR: Stocke une reference a la source pour extraire des lignes.
SourceExtractor::SourceExtractor(const std::string& source)
    : source_(source) {}

// EN: Returns the content of a specific line by number.
// FR: Renvoie le contenu d une ligne par numero.
std::string SourceExtractor::getLine(int targetLine) const {
    int currentLine = 1;
    size_t lineStart = 0;
    
    for (size_t i = 0; i < source_.length(); ++i) {
        if (currentLine == targetLine) {
            lineStart = i;
            break;
        }
        if (source_[i] == '\n') {
            currentLine++;
        }
    }
    
    size_t lineEnd = lineStart;
    while (lineEnd < source_.length() && source_[lineEnd] != '\n') {
        lineEnd++;
    }
    
    return source_.substr(lineStart, lineEnd - lineStart);
}

// EN: Returns the content of the current line.
// FR: Renvoie le contenu de la ligne courante.
std::string SourceExtractor::getCurrentLine(int currentLine) const {
    return getLine(currentLine);
}

// TODO(cc1) EN: Cache line offsets to avoid repeated scanning.
// FR: Cacher les offsets de lignes pour eviter les scans repetes.
