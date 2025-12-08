#pragma once

#include <string>
#include <memory>
#include <vector>
#include <preprocessor/PPDirective.hpp>

namespace cc1 {
namespace pp {

// ============================================================================
// Directive Parser - parses preprocessor directives from lines
// ============================================================================

class DirectiveParser {
public:
    // Parse a directive line (starting with #)
    std::unique_ptr<Directive> parse(const std::string& line);
    
private:
    // Parse specific directive types
    std::unique_ptr<DefineDirective> parseDefine(const std::string& line, size_t pos);
    std::unique_ptr<UndefDirective> parseUndef(const std::string& line, size_t pos);
    std::unique_ptr<IncludeDirective> parseInclude(const std::string& line, size_t pos);
    std::unique_ptr<IfDirective> parseIf(const std::string& line, size_t pos);
    std::unique_ptr<IfdefDirective> parseIfdef(const std::string& line, size_t pos, bool isIfndef);
    std::unique_ptr<ElifDirective> parseElif(const std::string& line, size_t pos);
    std::unique_ptr<ErrorDirective> parseError(const std::string& line, size_t pos);
    std::unique_ptr<PragmaDirective> parsePragma(const std::string& line, size_t pos);
    std::unique_ptr<LineDirective> parseLine(const std::string& line, size_t pos);
    
    // Utilities
    void skipWhitespace(const std::string& line, size_t& pos);
    std::string parseIdentifier(const std::string& line, size_t& pos);
    std::string parseFilename(const std::string& line, size_t& pos, bool& isSystemInclude);
    std::vector<std::string> parseMacroParams(const std::string& line, size_t& pos, bool& isVariadic);
    std::string parseMacroBody(const std::string& line, size_t pos);
};

} // namespace pp
} // namespace cc1
