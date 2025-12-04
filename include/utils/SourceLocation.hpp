#pragma once

#include <string>

struct SourceLocation {
    std::string filename;
    int line;
    int column;
    
    SourceLocation(const std::string& file = "", int l = 1, int c = 1)
        : filename(file), line(l), column(c) {}
    
    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};
