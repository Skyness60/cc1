#pragma once

#include <string>
#include <unordered_map>
#include <lexer/Token.hpp>

class KeywordMatcher {
public:
    static TokenType match(const std::string& identifier);
    static bool isKeyword(const std::string& identifier);

private:
    static const std::unordered_map<std::string, TokenType> keywords_;
};
