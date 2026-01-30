#pragma once




// EN: Utility class for character classification in the lexer.
// FR: Classe utilitaire de classification de caracteres pour le lexer.
class CharClassifier {
public:
    // EN: Checks if a character is alphabetic.
    // FR: Verifie si un caractere est alphabetique.
    static bool isAlpha(char c);
    // EN: Checks if a character is a digit.
    // FR: Verifie si un caractere est un chiffre.
    static bool isDigit(char c);
    // EN: Checks if a character is a hexadecimal digit.
    // FR: Verifie si un caractere est un chiffre hexadecimal.
    static bool isHexDigit(char c);
    // EN: Checks if a character is an octal digit.
    // FR: Verifie si un caractere est un chiffre octal.
    static bool isOctalDigit(char c);
    // EN: Checks if a character is alphanumeric.
    // FR: Verifie si un caractere est alphanumerique.
    static bool isAlphaNumeric(char c);
    // EN: Checks if a character is whitespace.
    // FR: Verifie si un caractere est un espace.
    static bool isWhitespace(char c);
};

// TODO(cc1) EN: Consider locale-aware classification if needed.
// FR: Envisager une classification dependante de la locale si besoin.
