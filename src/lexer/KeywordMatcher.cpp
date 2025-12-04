#include <lexer/KeywordMatcher.hpp>

const std::unordered_map<std::string, TokenType> KeywordMatcher::keywords_ = {
    {"auto", TokenType::Auto},         {"break", TokenType::Break},
    {"case", TokenType::Case},         {"char", TokenType::Char},
    {"const", TokenType::Const},       {"continue", TokenType::Continue},
    {"default", TokenType::Default},   {"do", TokenType::Do},
    {"double", TokenType::Double},     {"else", TokenType::Else},
    {"enum", TokenType::Enum},         {"extern", TokenType::Extern},
    {"float", TokenType::Float},       {"for", TokenType::For},
    {"goto", TokenType::Goto},         {"if", TokenType::If},
    {"int", TokenType::Int},           {"long", TokenType::Long},
    {"register", TokenType::Register}, {"return", TokenType::Return},
    {"short", TokenType::Short},       {"signed", TokenType::Signed},
    {"sizeof", TokenType::Sizeof},     {"static", TokenType::Static},
    {"struct", TokenType::Struct},     {"switch", TokenType::Switch},
    {"typedef", TokenType::Typedef},   {"union", TokenType::Union},
    {"unsigned", TokenType::Unsigned}, {"void", TokenType::Void},
    {"volatile", TokenType::Volatile}, {"while", TokenType::While}
};

TokenType KeywordMatcher::match(const std::string& identifier) {
    auto it = keywords_.find(identifier);
    return (it != keywords_.end()) ? it->second : TokenType::Identifier;
}

bool KeywordMatcher::isKeyword(const std::string& identifier) {
    return keywords_.find(identifier) != keywords_.end();
}
