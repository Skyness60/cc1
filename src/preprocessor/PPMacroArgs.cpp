#include <preprocessor/PPMacro.hpp>

namespace cc1 {
namespace pp {

std::vector<std::string> MacroExpander::parseArguments(const std::string& text, size_t& pos) {
    std::vector<std::string> args;

    if (pos >= text.size() || text[pos] != '(') {
        return args;
    }
    pos++;

    int parenDepth = 1;
    std::string currentArg;

    while (pos < text.size() && parenDepth > 0) {
        char c = text[pos];

        if (c == '(') {
            parenDepth++;
            currentArg += c;
        } else if (c == ')') {
            parenDepth--;
            if (parenDepth > 0) {
                currentArg += c;
            }
        } else if (c == ',' && parenDepth == 1) {
            args.push_back(currentArg);
            currentArg.clear();
        } else if (c == '"') {
            currentArg += c;
            pos++;
            while (pos < text.size() && text[pos] != '"') {
                if (text[pos] == '\\' && pos + 1 < text.size()) {
                    currentArg += text[pos++];
                }
                currentArg += text[pos++];
            }
            if (pos < text.size()) {
                currentArg += text[pos];
            }
        } else if (c == '\'') {
            currentArg += c;
            pos++;
            while (pos < text.size() && text[pos] != '\'') {
                if (text[pos] == '\\' && pos + 1 < text.size()) {
                    currentArg += text[pos++];
                }
                currentArg += text[pos++];
            }
            if (pos < text.size()) {
                currentArg += text[pos];
            }
        } else {
            currentArg += c;
        }
        pos++;
    }

    if (!currentArg.empty() || !args.empty()) {
        args.push_back(currentArg);
    }

    for (size_t j = 0; j < args.size(); j++) {
        std::string& arg = args[j];
        size_t start = 0;
        while (start < arg.size() && (arg[start] == ' ' || arg[start] == '\t')) {
            start++;
        }
        size_t end = arg.size();
        while (end > start && (arg[end - 1] == ' ' || arg[end - 1] == '\t')) {
            end--;
        }
        arg = arg.substr(start, end - start);
    }

    return args;
}

std::string MacroExpander::stringify(const std::string& arg) {
    std::string result = "\"";
    for (size_t i = 0; i < arg.size(); i++) {
        if (arg[i] == '"' || arg[i] == '\\') {
            result += '\\';
        }
        result += arg[i];
    }
    result += "\"";
    return result;
}

} // namespace pp
} // namespace cc1
