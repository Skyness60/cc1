#pragma once



#include <stdexcept>
#include <string>
#include <utils/SourceLocation.hpp>
#include <utils/Diagnostic.hpp>

// EN: Exception type for parse errors with source context.
// FR: Type d exception pour erreurs de parsing avec contexte source.
class ParseError : public std::runtime_error {
public:
    // EN: Builds a parse error with formatted diagnostics.
    // FR: Construit une erreur de parse avec diagnostic formate.
    ParseError(const std::string& message, const SourceLocation& loc,
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

// TODO(cc1) EN: Attach expected token info for better diagnostics.
// FR: Attacher les tokens attendus pour de meilleurs diagnostics.
