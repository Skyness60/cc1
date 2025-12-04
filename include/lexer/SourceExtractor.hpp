#pragma once

#include <string>

class SourceExtractor {
public:
    SourceExtractor(const std::string& source);
    
    std::string getLine(int lineNumber) const;
    std::string getCurrentLine(int currentLine) const;

private:
    const std::string& source_;
};
