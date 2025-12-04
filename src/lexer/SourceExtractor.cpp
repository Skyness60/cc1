#include <lexer/SourceExtractor.hpp>

SourceExtractor::SourceExtractor(const std::string& source)
    : source_(source) {}

std::string SourceExtractor::getLine(int targetLine) const {
    int currentLine = 1;
    size_t lineStart = 0;
    
    for (size_t i = 0; i < source_.length(); ++i) {
        if (currentLine == targetLine) {
            lineStart = i;
            break;
        }
        if (source_[i] == '\n') {
            currentLine++;
        }
    }
    
    size_t lineEnd = lineStart;
    while (lineEnd < source_.length() && source_[lineEnd] != '\n') {
        lineEnd++;
    }
    
    return source_.substr(lineStart, lineEnd - lineStart);
}

std::string SourceExtractor::getCurrentLine(int currentLine) const {
    return getLine(currentLine);
}
