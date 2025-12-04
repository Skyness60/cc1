#pragma once

#include <lexer/Token.hpp>
#include <lexer/SourceReader.hpp>

class IScanner {
public:
    virtual ~IScanner() = default;
    virtual bool canScan(char c) const = 0;
    virtual Token scan(SourceReader& reader) = 0;
};
