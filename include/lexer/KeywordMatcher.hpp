#pragma once

#include <string>
#include <unordered_map>
#include <lexer/Token.hpp>




// EN: Maps identifier strings to keyword token types.
// FR: Mappe les identifiants vers les types de tokens mot-cle.
class KeywordMatcher {
public:
    
    // EN: Returns the keyword token type or Identifier if not a keyword.
    // FR: Renvoie le type de mot-cle ou Identifier si ce n est pas un mot-cle.
    static TokenType match(const std::string& identifier);

    
    // EN: Checks if a string is a keyword.
    // FR: Verifie si une chaine est un mot-cle.
    static bool isKeyword(const std::string& identifier);

private:
    static const std::unordered_map<std::string, TokenType> keywords_;
};

// TODO(cc1) EN: Keep keyword list in sync with parser language mode selection.
// FR: Synchroniser la liste des mots-cles avec le mode de langage du parseur.
