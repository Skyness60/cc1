#include <preprocessor/PPParser.hpp>

#include <cctype>

namespace cc1 {
namespace pp {

std::unique_ptr<Directive> DirectiveParser::parse(const std::string& line) {
    size_t pos = 0;

    skipWhitespace(line, pos);

    if (pos >= line.size() || line[pos] != '#') {
        return std::unique_ptr<Directive>();
    }
    pos++;

    std::string directive = parseIdentifier(line, pos);

    if (directive == "define") {
        return parseDefine(line, pos);
    } else if (directive == "undef") {
        return parseUndef(line, pos);
    } else if (directive == "include") {
        return parseInclude(line, pos);
    } else if (directive == "if") {
        return parseIf(line, pos);
    } else if (directive == "ifdef") {
        return parseIfdef(line, pos, false);
    } else if (directive == "ifndef") {
        return parseIfdef(line, pos, true);
    } else if (directive == "elif") {
        return parseElif(line, pos);
    } else if (directive == "else") {
        return std::unique_ptr<Directive>(new ElseDirective());
    } else if (directive == "endif") {
        return std::unique_ptr<Directive>(new EndifDirective());
    } else if (directive == "error") {
        return parseError(line, pos);
    } else if (directive == "pragma") {
        return parsePragma(line, pos);
    } else if (directive == "line") {
        return parseLine(line, pos);
    }

    return std::unique_ptr<Directive>();
}

std::unique_ptr<DefineDirective> DirectiveParser::parseDefine(const std::string& line, size_t pos) {
    std::string name = parseIdentifier(line, pos);

    if (name.empty()) {
        return std::unique_ptr<DefineDirective>();
    }

    bool isFunctionLike = false;
    bool isVariadic = false;
    std::vector<std::string> params;

    if (pos < line.size() && line[pos] == '(') {
        isFunctionLike = true;
        params = parseMacroParams(line, pos, isVariadic);
    }

    std::string body = parseMacroBody(line, pos);

    return std::unique_ptr<DefineDirective>(new DefineDirective(name, params, body, isFunctionLike, isVariadic));
}

std::unique_ptr<UndefDirective> DirectiveParser::parseUndef(const std::string& line, size_t pos) {
    std::string name = parseIdentifier(line, pos);

    if (name.empty()) {
        return std::unique_ptr<UndefDirective>();
    }

    return std::unique_ptr<UndefDirective>(new UndefDirective(name));
}

std::unique_ptr<IncludeDirective> DirectiveParser::parseInclude(const std::string& line, size_t pos) {
    bool isSystemInclude = false;
    std::string filename = parseFilename(line, pos, isSystemInclude);

    if (filename.empty()) {
        return std::unique_ptr<IncludeDirective>();
    }

    return std::unique_ptr<IncludeDirective>(new IncludeDirective(filename, isSystemInclude));
}

std::unique_ptr<IfDirective> DirectiveParser::parseIf(const std::string& line, size_t pos) {
    skipWhitespace(line, pos);

    std::string condition;
    if (pos < line.size()) {
        condition = line.substr(pos);
        while (!condition.empty() && std::isspace(condition.back())) {
            condition.pop_back();
        }
    }

    return std::unique_ptr<IfDirective>(new IfDirective(condition));
}

std::unique_ptr<IfdefDirective> DirectiveParser::parseIfdef(const std::string& line, size_t pos, bool isIfndef) {
    std::string name = parseIdentifier(line, pos);

    if (name.empty()) {
        return std::unique_ptr<IfdefDirective>();
    }

    return std::unique_ptr<IfdefDirective>(new IfdefDirective(name, isIfndef));
}

std::unique_ptr<ElifDirective> DirectiveParser::parseElif(const std::string& line, size_t pos) {
    skipWhitespace(line, pos);

    std::string condition;
    if (pos < line.size()) {
        condition = line.substr(pos);
        while (!condition.empty() && std::isspace(condition.back())) {
            condition.pop_back();
        }
    }

    return std::unique_ptr<ElifDirective>(new ElifDirective(condition));
}

std::unique_ptr<ErrorDirective> DirectiveParser::parseError(const std::string& line, size_t pos) {
    skipWhitespace(line, pos);

    std::string message;
    if (pos < line.size()) {
        message = line.substr(pos);
        while (!message.empty() && std::isspace(message.back())) {
            message.pop_back();
        }
    }

    return std::unique_ptr<ErrorDirective>(new ErrorDirective(message));
}

std::unique_ptr<PragmaDirective> DirectiveParser::parsePragma(const std::string& line, size_t pos) {
    skipWhitespace(line, pos);

    std::string content;
    if (pos < line.size()) {
        content = line.substr(pos);
        while (!content.empty() && std::isspace(content.back())) {
            content.pop_back();
        }
    }

    return std::unique_ptr<PragmaDirective>(new PragmaDirective(content));
}

std::unique_ptr<LineDirective> DirectiveParser::parseLine(const std::string& line, size_t pos) {
    skipWhitespace(line, pos);

    int lineNum = 0;
    while (pos < line.size() && std::isdigit(line[pos])) {
        lineNum = lineNum * 10 + (line[pos] - '0');
        pos++;
    }

    skipWhitespace(line, pos);
    std::string filename;

    if (pos < line.size() && line[pos] == '"') {
        pos++;
        while (pos < line.size() && line[pos] != '"') {
            filename += line[pos++];
        }
    }

    return std::unique_ptr<LineDirective>(new LineDirective(lineNum, filename));
}

} // namespace pp
} // namespace cc1
