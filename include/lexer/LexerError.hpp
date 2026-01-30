#pragma once

#include <stdexcept>
#include <string>
#include <utils/SourceLocation.hpp>
#include <utils/Diagnostic.hpp>




// EN: Exception type for lexer errors with source context.
// FR: Type d exception pour les erreurs du lexer avec contexte source.
class LexerError : public std::runtime_error {
public:
    
    // EN: Builds a lexer error with formatted diagnostic details.
    // FR: Construit une erreur de lexer avec diagnostic formate.
    LexerError(const std::string& message, const SourceLocation& loc,
               const std::string& sourceLine = "")
        : std::runtime_error(DiagnosticPrinter::format(DiagnosticKind::Error, loc, message, sourceLine)),
          location_(loc),
          message_(message) {}

    // EN: Returns the source location of the error.
    // FR: Renvoie la position source de l erreur.
    const SourceLocation& getLocation() const { return location_; }
    // EN: Returns the raw error message.
    // FR: Renvoie le message d erreur brut.
    const std::string& getMessage() const { return message_; }

private:
    SourceLocation location_;
    std::string message_;
};

// TODO(cc1) EN: Include token context or suggestions in lexer errors.
// FR: Inclure le contexte de token ou des suggestions dans les erreurs.
