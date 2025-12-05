#pragma once

#include <memory>
#include <vector>

namespace AST {

// Forward declarations
class Visitor;

// ============================================================================
// Base AST Node
// ============================================================================

class Node {
public:
    virtual ~Node() = default;
    virtual void accept(Visitor& visitor) = 0;
    
    // Source location for error reporting
    int line = 0;
    int column = 0;
    
protected:
    Node() = default;
    Node(int l, int c) : line(l), column(c) {}
};

// ============================================================================
// Convenience Type Aliases and Helpers
// ============================================================================

template<typename T>
using Ptr = std::unique_ptr<T>;

template<typename T>
using PtrList = std::vector<std::unique_ptr<T>>;

/// Helper function to create AST nodes (like std::make_unique for C++11)
template<typename T, typename... Args>
Ptr<T> make(Args&&... args) {
    return Ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace AST
