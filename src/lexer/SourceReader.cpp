#include <lexer/SourceReader.hpp>

// EN: Initializes the reader with a source buffer.
// FR: Initialise le reader avec un buffer source.
SourceReader::SourceReader(const std::string& source)
    : source_(source) {}

// EN: Peeks at the current character or '\0' at end.
// FR: Regarde le caractere courant ou '\0' a la fin.
char SourceReader::peek() const {
    return isAtEnd() ? '\0' : source_[pos_];
}

// EN: Peeks at the next character or '\0' at end.
// FR: Regarde le caractere suivant ou '\0' a la fin.
char SourceReader::peekNext() const {
    return (pos_ + 1 >= source_.length()) ? '\0' : source_[pos_ + 1];
}

// EN: Consumes the current character and updates line/column.
// FR: Consomme le caractere courant et met a jour ligne/colonne.
char SourceReader::advance() {
    char c = source_[pos_++];
    if (c == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    return c;
}

// EN: Consumes the expected character if it matches.
// FR: Consomme le caractere attendu s il correspond.
bool SourceReader::match(char expected) {
    if (isAtEnd() || source_[pos_] != expected) return false;
    advance();
    return true;
}

// EN: Checks whether the cursor reached the end.
// FR: Verifie si le curseur est a la fin.
bool SourceReader::isAtEnd() const {
    return pos_ >= source_.length();
}

// EN: Moves back one character, adjusting line/column.
// FR: Recule d un caractere en ajustant ligne/colonne.
void SourceReader::retreat() {
    if (pos_ > 0) {
        pos_--;
        char c = source_[pos_];
        if (c == '\n') {
            line_--;
            
            column_ = 1;
            for (size_t i = pos_; i > 0 && source_[i - 1] != '\n'; --i)
                column_++;
        } else {
            column_--;
        }
    }
}

// TODO(cc1) EN: Optimize retreat() for large lines by caching line starts.
// FR: Optimiser retreat() pour grandes lignes en cachant debuts de ligne.
