#pragma once



#include <memory>
#include <vector>

namespace AST {


class Visitor;





// EN: Base AST node with source location and visitor dispatch.
// FR: Noeud AST de base avec position source et dispatch visiteur.
class Node {
public:
    // EN: Virtual destructor for polymorphic cleanup.
    // FR: Destructeur virtuel pour nettoyage polymorphique.
    virtual ~Node() = default;
    // EN: Accepts a visitor for node-specific processing.
    // FR: Accepte un visiteur pour traitement specifique.
    virtual void accept(Visitor& visitor) = 0;
    
    
    // EN: Source line for diagnostics.
    // FR: Ligne source pour diagnostics.
    int line = 0;
    // EN: Source column for diagnostics.
    // FR: Colonne source pour diagnostics.
    int column = 0;
    
protected:
    // EN: Protected default ctor for base class usage.
    // FR: Constructeur par defaut protege pour la base.
    Node() = default;
    // EN: Protected ctor initializing source location.
    // FR: Constructeur protege initialisant la position.
    Node(int l, int c) : line(l), column(c) {}
};





// EN: Unique pointer alias for AST nodes.
// FR: Alias de unique_ptr pour noeuds AST.
template<typename T>
using Ptr = std::unique_ptr<T>;

// EN: List alias for unique_ptr nodes.
// FR: Alias de liste pour noeuds en unique_ptr.
template<typename T>
using PtrList = std::vector<std::unique_ptr<T>>;


// EN: Helper to construct AST nodes with type inference.
// FR: Helper pour construire des noeuds AST avec inference de type.
template<typename T, typename... Args>
Ptr<T> make(Args&&... args) {
    return Ptr<T>(new T(std::forward<Args>(args)...));
}

// TODO(cc1) EN: Consider adding shared_ptr support for cross-linked nodes.
// FR: Envisager un support shared_ptr pour noeuds relies entre eux.
} 
