#pragma once



#include <string>
#include <memory>
#include <vector>
#include <preprocessor/PPDirective.hpp>

namespace cc1 {
namespace pp {





// EN: Parses a preprocessor directive line into a typed Directive object.
// FR: Parse une ligne de directive en un objet Directive type.
class DirectiveParser {
public:
    
    // EN: Parses a directive line and returns the corresponding directive node.
    // FR: Parse une ligne de directive et renvoie le noeud correspondant.
    std::unique_ptr<Directive> parse(const std::string& line);
    
private:
    
    // EN: Parses a #define directive starting at position.
    // FR: Parse une directive #define a partir d une position.
    std::unique_ptr<DefineDirective> parseDefine(const std::string& line, size_t pos);
    // EN: Parses a #undef directive starting at position.
    // FR: Parse une directive #undef a partir d une position.
    std::unique_ptr<UndefDirective> parseUndef(const std::string& line, size_t pos);
    // EN: Parses a #include directive starting at position.
    // FR: Parse une directive #include a partir d une position.
    std::unique_ptr<IncludeDirective> parseInclude(const std::string& line, size_t pos);
    // EN: Parses a #if directive starting at position.
    // FR: Parse une directive #if a partir d une position.
    std::unique_ptr<IfDirective> parseIf(const std::string& line, size_t pos);
    // EN: Parses #ifdef/#ifndef, controlled by isIfndef.
    // FR: Parse #ifdef/#ifndef, controle par isIfndef.
    std::unique_ptr<IfdefDirective> parseIfdef(const std::string& line, size_t pos, bool isIfndef);
    // EN: Parses a #elif directive starting at position.
    // FR: Parse une directive #elif a partir d une position.
    std::unique_ptr<ElifDirective> parseElif(const std::string& line, size_t pos);
    // EN: Parses a #error directive starting at position.
    // FR: Parse une directive #error a partir d une position.
    std::unique_ptr<ErrorDirective> parseError(const std::string& line, size_t pos);
    // EN: Parses a #pragma directive starting at position.
    // FR: Parse une directive #pragma a partir d une position.
    std::unique_ptr<PragmaDirective> parsePragma(const std::string& line, size_t pos);
    // EN: Parses a #line directive starting at position.
    // FR: Parse une directive #line a partir d une position.
    std::unique_ptr<LineDirective> parseLine(const std::string& line, size_t pos);
    
    
    // EN: Skips whitespace to align parsing positions.
    // FR: Saute les espaces pour aligner le parsing.
    void skipWhitespace(const std::string& line, size_t& pos);
    // EN: Parses an identifier token from the line.
    // FR: Parse un identifiant dans la ligne.
    std::string parseIdentifier(const std::string& line, size_t& pos);
    // EN: Parses an include filename and marks system includes.
    // FR: Parse un nom de fichier include et detecte un include systeme.
    std::string parseFilename(const std::string& line, size_t& pos, bool& isSystemInclude);
    // EN: Parses macro parameter list and notes variadic use.
    // FR: Parse la liste des parametres de macro et note le variadique.
    std::vector<std::string> parseMacroParams(const std::string& line, size_t& pos, bool& isVariadic);
    // EN: Parses the macro body text after the name/params.
    // FR: Parse le corps de macro apres le nom/params.
    std::string parseMacroBody(const std::string& line, size_t pos);
};

// TODO(cc1) EN: Add more granular error reporting for malformed directives.
// FR: Ajouter des erreurs plus fines pour les directives mal formees.
} 
} 
