#pragma once



#include <string>
#include <vector>
#include <memory>
#include <lexer/Token.hpp>
#include <lexer/SourceReader.hpp>
#include <lexer/SourceExtractor.hpp>
#include <lexer/IScanner.hpp>
#include <utils/SourceLocation.hpp>


// EN: Converts raw source text into a stream of tokens.
// FR: Convertit le texte source en une suite de tokens.
class Lexer {
public:
    
    // EN: Initializes the lexer with source text and optional filename.
    // FR: Initialise le lexer avec le texte source et un nom de fichier optionnel.
    Lexer(const std::string& source, const std::string& filename = "");
    
    
    // EN: Tokenizes the entire input source.
    // FR: Tokenise toute l entree source.
    std::vector<Token> tokenize();

    // EN: Overrides the source line buffer used for diagnostics.
    // FR: Surcharge le buffer source utilise pour les diagnostics.
    void setDisplaySource(const std::string& src) { display_source_override_ = src; }

    // EN: Returns the buffer used for diagnostics (original if no override).
    // FR: Renvoie le buffer utilise pour les diagnostics (original si pas d override).
    const std::string& diagnosticSource() const { return display_source_override_.empty() ? extractor_source_ : display_source_override_; }

private:
    enum ScannerType {
        IDENTIFIER = 0,
        NUMBER,
        STRING,
        CHAR,
        SCANNER_COUNT
    };
    
    std::string filename_;
    std::string extractor_source_;
    std::string display_source_override_;
    std::unique_ptr<SourceReader> reader_;
    std::unique_ptr<SourceExtractor> extractor_;
    std::unique_ptr<IScanner> scanners_[SCANNER_COUNT];
    
    // EN: Skips whitespace and comments to align to the next token.
    // FR: Saute les espaces et commentaires pour aligner au prochain token.
    void skipWhitespaceAndComments();
    // EN: Skips a /* */ comment block.
    // FR: Saute un bloc de commentaire /* */.
    void skipBlockComment();
    // EN: Scans the next token using the appropriate scanner.
    // FR: Scanne le prochain token avec le scanner approprie.
    Token scanToken();
    
    // EN: Reports a fatal lexing error without location.
    // FR: Signale une erreur fatale de lexing sans position.
    [[noreturn]] void error(const std::string& msg);
    // EN: Reports a fatal lexing error at a specific location.
    // FR: Signale une erreur fatale de lexing a une position precise.
    [[noreturn]] void errorAt(const std::string& msg, const SourceLocation& loc);
};

// TODO(cc1) EN: Add support for line directives to remap filename/line numbers.
// FR: Ajouter le support des directives de ligne pour remapper fichier/ligne.
