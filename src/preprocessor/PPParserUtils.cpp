#include <preprocessor/PPParser.hpp>

#include <cctype>

namespace cc1 {
namespace pp {

void DirectiveParser::skipWhitespace(const std::string& line, size_t& pos) {
    while (pos < line.size() && std::isspace(line[pos])) {
        pos++;
    }
}

std::string DirectiveParser::parseIdentifier(const std::string& line, size_t& pos) {
    std::string result;
    skipWhitespace(line, pos);

    if (pos < line.size() && (std::isalpha(line[pos]) || line[pos] == '_')) {
        result += line[pos++];
        while (pos < line.size() && (std::isalnum(line[pos]) || line[pos] == '_')) {
            result += line[pos++];
        }
    }

    return result;
}

std::string DirectiveParser::parseFilename(const std::string& line, size_t& pos, bool& isSystemInclude) {
    skipWhitespace(line, pos);

    if (pos >= line.size()) {
        return "";
    }

    char delim;
    if (line[pos] == '<') {
        delim = '>';
        isSystemInclude = true;
        pos++;
    } else if (line[pos] == '"') {
        delim = '"';
        isSystemInclude = false;
        pos++;
    } else {
        return "";
    }

    std::string filename;
    while (pos < line.size() && line[pos] != delim) {
        filename += line[pos++];
    }

    if (pos < line.size()) {
        pos++;
    }

    return filename;
}

std::vector<std::string> DirectiveParser::parseMacroParams(const std::string& line, size_t& pos, bool& isVariadic) {
    std::vector<std::string> params;
    isVariadic = false;

    skipWhitespace(line, pos);

    if (pos < line.size() && line[pos] == '(') {
        pos++;
    }

    while (pos < line.size() && line[pos] != ')') {
        skipWhitespace(line, pos);

        if (pos + 2 < line.size() && line[pos] == '.' && line[pos + 1] == '.' && line[pos + 2] == '.') {
            isVariadic = true;
            pos += 3;
            skipWhitespace(line, pos);
            continue;
        }

        std::string param = parseIdentifier(line, pos);
        if (!param.empty()) {
            params.push_back(param);
        }

        skipWhitespace(line, pos);

        if (pos < line.size() && line[pos] == ',') {
            pos++;
        }
    }

    if (pos < line.size() && line[pos] == ')') {
        pos++;
    }

    return params;
}

std::string DirectiveParser::parseMacroBody(const std::string& line, size_t pos) {
    skipWhitespace(line, pos);

    std::string body;
    size_t end = line.size();

    while (end > pos && std::isspace(line[end - 1])) {
        end--;
    }

    if (pos < end) {
        body = line.substr(pos, end - pos);
    }

    return body;
}

} // namespace pp
} // namespace cc1
