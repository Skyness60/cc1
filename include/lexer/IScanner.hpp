#pragma once

#include <lexer/Token.hpp>
#include <lexer/SourceReader.hpp>




// EN: Interface for token scanners that recognize specific token kinds.
// FR: Interface pour les scanners de tokens qui reconnaissent des types specifiques.
class IScanner {
public:
    // EN: Virtual destructor for polymorphic cleanup.
    // FR: Destructeur virtuel pour nettoyage polymorphique.
    virtual ~IScanner() = default;

    
    // EN: Checks whether this scanner can handle a given starting character.
    // FR: Verifie si ce scanner peut gerer un caractere de depart.
    virtual bool canScan(char c) const = 0;

    
    // EN: Scans the next token from the reader.
    // FR: Scanne le prochain token depuis le reader.
    virtual Token scan(SourceReader& reader) = 0;
};

// TODO(cc1) EN: Add a method to report scanner name for diagnostics.
// FR: Ajouter une methode pour nommer le scanner dans les diagnostics.
