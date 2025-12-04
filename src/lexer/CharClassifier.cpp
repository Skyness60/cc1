#include <lexer/CharClassifier.hpp>

bool CharClassifier::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool CharClassifier::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool CharClassifier::isHexDigit(char c) {
    return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

bool CharClassifier::isOctalDigit(char c) {
    return c >= '0' && c <= '7';
}

bool CharClassifier::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

bool CharClassifier::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}
