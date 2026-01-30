#pragma once

#include <string>




// EN: Lightweight cursor over source text with line/column tracking.
// FR: Curseur leger sur le texte source avec suivi ligne/colonne.
class SourceReader {
public:
    
    // EN: Initializes a reader with the source buffer.
    // FR: Initialise un reader avec le buffer source.
    SourceReader(const std::string& source);
    
    
    // EN: Returns the current character without consuming it.
    // FR: Renvoie le caractere courant sans l avancer.
    char peek() const;
    // EN: Returns the next character without consuming it.
    // FR: Renvoie le caractere suivant sans l avancer.
    char peekNext() const;
    // EN: Consumes and returns the current character.
    // FR: Consomme et renvoie le caractere courant.
    char advance();
    // EN: Advances if the next character matches the expected one.
    // FR: Avance si le prochain caractere correspond a l attendu.
    bool match(char expected);
    // EN: Checks if the cursor reached the end of the source.
    // FR: Verifie si le curseur a atteint la fin.
    bool isAtEnd() const;
    
    
    // EN: Returns current line number.
    // FR: Renvoie le numero de ligne courant.
    int line() const { return line_; }
    // EN: Returns current column number.
    // FR: Renvoie la colonne courante.
    int column() const { return column_; }
    // EN: Returns current absolute position in the buffer.
    // FR: Renvoie la position absolue dans le buffer.
    size_t position() const { return pos_; }
    
    
    // EN: Steps back one character, adjusting line/column.
    // FR: Recule d un caractere en ajustant ligne/colonne.
    void retreat();
    
private:
    std::string source_;
    size_t pos_ = 0;
    int line_ = 1;
    int column_ = 1;
};

// TODO(cc1) EN: Track UTF-8 column widths if non-ASCII input is supported.
// FR: Gerer la largeur de colonne UTF-8 si le non-ASCII est supporte.
