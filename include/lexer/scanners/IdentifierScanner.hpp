#pragma once

#include <lexer/IScanner.hpp>
#include <lexer/CharClassifier.hpp>
#include <lexer/KeywordMatcher.hpp>




// EN: Scans identifiers and keywords.
// FR: Scanne les identifiants et mots-cles.
class IdentifierScanner final : public IScanner {
    public:
        // EN: Starts when the first char is alphabetic.
        // FR: Demarre si le premier caractere est alphabetique.
        bool canScan(char c) const override { return CharClassifier::isAlpha(c); }
        
        // EN: Consumes an identifier and returns a keyword or identifier token.
        // FR: Consomme un identifiant et renvoie un token mot-cle ou identifiant.
        Token scan(SourceReader& reader) override {
            int line = reader.line();
            int col = reader.column();
            std::string text;
            
            while (CharClassifier::isAlphaNumeric(reader.peek()))
                text += reader.advance();
            
            return Token(KeywordMatcher::match(text), text, line, col);
        }
};

// TODO(cc1) EN: Accept '_' as a starting character if needed.
// FR: Accepter '_' comme caractere de depart si besoin.
