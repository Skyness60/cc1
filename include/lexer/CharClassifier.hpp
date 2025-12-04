#pragma once

class CharClassifier {
public:
    static bool isAlpha(char c);
    static bool isDigit(char c);
    static bool isHexDigit(char c);
    static bool isOctalDigit(char c);
    static bool isAlphaNumeric(char c);
    static bool isWhitespace(char c);
};
