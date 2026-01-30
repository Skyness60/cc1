#pragma once



#include <string>
#include <utility>

namespace cc1 {
namespace pp {





// EN: Joins adjacent string literals per C rules, simplifying the parser.
// FR: Concatene les chaines adjacentes selon les regles C pour simplifier le parseur.
class StringConcatenator {
public:
    
    // EN: Concatenates adjacent string tokens in a source buffer so later passes
    // see merged literals. FR: Concatene les tokens chaine du buffer pour que
    // les passes suivantes voient des chaines fusionnees.
    std::string concatenate(const std::string& source);
    
private:
    
    // EN: Parses one string literal and returns both text and termination status.
    // FR: Parse une chaine et renvoie le texte ainsi que l etat de terminaison.
    std::pair<std::string, bool> parseString(const std::string& source, size_t& pos);
};





// EN: Removes // and /* */ comments while preserving newlines for diagnostics.
// FR: Supprime les commentaires en preservant les sauts de ligne.
class CommentRemover {
public:
    
    // EN: Strips comments from a full source buffer, keeping newlines intact.
    // FR: Retire les commentaires d un buffer en gardant les lignes.
    std::string remove(const std::string& source);
};





// EN: Handles backslash-newline continuations so directives/macros parse as one line.
// FR: Gere les continuations pour parser une ligne logique unique.
class LineContinuation {
public:
    
    // EN: Extends a line with continued lines from the stream and updates
    // line counters. FR: Etend une ligne avec les continuations et met a jour
    // le compteur de lignes.
    std::string process(const std::string& line, std::istream& stream, int& lineCount);
};

// TODO(cc1) EN: Consider preserving comment text for optional debug output while
// still removing it from preprocessing results. FR: Envisager de conserver le
// texte des commentaires pour debug tout en le retirant du resultat.
} 
} 
