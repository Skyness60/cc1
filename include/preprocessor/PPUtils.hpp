#pragma once

#include <string>

namespace cc1 {
namespace pp {

// ============================================================================
// String Concatenator - Phase 6: concatenate adjacent string literals
// ============================================================================

class StringConcatenator {
public:
    // Concatenate adjacent string literals in source
    std::string concatenate(const std::string& source);
    
private:
    // Parse a single string literal (with quotes)
    std::string parseString(const std::string& source, size_t& pos);
};

// ============================================================================
// Comment Remover - Phases 1-3: remove comments, handle trigraphs
// ============================================================================

class CommentRemover {
public:
    // Remove C and C++ style comments
    std::string remove(const std::string& source);
};

// ============================================================================
// Line Continuation Handler
// ============================================================================

class LineContinuation {
public:
    // Join lines ending with backslash
    std::string process(const std::string& line, std::istream& stream, int& lineCount);
};

} // namespace pp
} // namespace cc1
