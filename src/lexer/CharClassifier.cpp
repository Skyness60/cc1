#include <lexer/CharClassifier.hpp>

// EN: Returns true for ASCII letters or underscore.
// FR: Renvoie vrai pour lettres ASCII ou underscore.
bool CharClassifier::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

// EN: Returns true for ASCII digits.
// FR: Renvoie vrai pour chiffres ASCII.
bool CharClassifier::isDigit(char c) {
    return c >= '0' && c <= '9';
}

// EN: Returns true for hexadecimal digits.
// FR: Renvoie vrai pour digits hexadecimaux.
bool CharClassifier::isHexDigit(char c) {
    return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

// EN: Returns true for octal digits.
// FR: Renvoie vrai pour digits octaux.
bool CharClassifier::isOctalDigit(char c) {
    return c >= '0' && c <= '7';
}

// EN: Returns true for letters or digits.
// FR: Renvoie vrai pour lettres ou chiffres.
bool CharClassifier::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

// EN: Returns true for whitespace characters.
// FR: Renvoie vrai pour caracteres d espace.
bool CharClassifier::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

// TODO(cc1) EN: Decide whether to treat vertical tab/form feed as whitespace.
// FR: Decider si VT/FF doivent etre consideres comme espaces.
