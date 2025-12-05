#pragma once

#include <stdexcept>
#include <string>
#include <utils/SourceLocation.hpp>
#include <utils/Diagnostic.hpp>

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message, const SourceLocation& loc,
               const std::string& sourceLine = "")
        : std::runtime_error(DiagnosticPrinter::format(DiagnosticKind::Error, loc, message, sourceLine)),
          location_(loc),
          message_(message) {}

    const SourceLocation& getLocation() const { return location_; }
    const std::string& getMessage() const { return message_; }

private:
    SourceLocation location_;
    std::string message_;
};
