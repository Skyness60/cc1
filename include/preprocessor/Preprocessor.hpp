#pragma once

#include <string>
#include <vector>
#include <stack>
#include <memory>

#include <preprocessor/PPDirective.hpp>
#include <preprocessor/PPMacro.hpp>
#include <preprocessor/PPExprEval.hpp>
#include <preprocessor/PPParser.hpp>
#include <preprocessor/PPFileHandler.hpp>
#include <preprocessor/PPUtils.hpp>

namespace cc1 {







// EN: Tracks nested conditional state to decide line emission and avoid multiple
// "else" activations. FR: Suit les etats conditionnels imbriques pour savoir
// si une ligne est emise et pour bloquer plusieurs "else".
struct ConditionalState {
    bool active;      
    bool seenTrue;    
    bool isElse;      
};





// EN: Coordinates the full preprocessing pipeline (files, comments, macros,
// directives) to preserve ordering and consistent diagnostics. FR: Orchestre
// tout le pretraitement (fichiers, commentaires, macros, directives) pour
// garder l ordre et produire des diagnostics coherents.
class Preprocessor : public pp::DirectiveVisitor {
public:
    // EN: Builds the preprocessor with fresh helpers so an instance can be
    // reused safely. FR: Construit le preprocesseur avec un etat propre pour
    // reutiliser l instance sans melanger les traitements.
    Preprocessor();
    // EN: Explicit destructor clarifies ownership for unique_ptr and keeps
    // forward declarations in the .cpp. FR: Destructeur explicite pour clarifier
    // l ownership des unique_ptr et permettre des declarations avancees.
    ~Preprocessor();
    
    
    // EN: Preprocesses a file and returns fully expanded text, preserving
    // filename-based diagnostics. FR: Pretraite un fichier et renvoie le texte
    // expanse en gardant des diagnostics bases sur le nom de fichier.
    std::string preprocess(const std::string& filename);
    
    
    // EN: Preprocesses a source string while tracking a logical filename, useful
    // for tests/REPL. FR: Pretraite une chaine source avec un nom logique,
    // utile pour tests/REPL.
    std::string preprocessString(const std::string& source, const std::string& filename = "<string>");
    
    
    // EN: Adds an include search path so projects control header resolution.
    // FR: Ajoute un chemin de recherche pour controler la resolution des
    // en-tetes.
    void addIncludePath(const std::string& path);
    
    
    // EN: Defines a macro up front, mirroring -D flags; default value eases
    // boolean toggles. FR: Definit une macro au demarrage, comme -D ; la valeur
    // par defaut facilite les bascules booleennes.
    void defineMacro(const std::string& name, const std::string& value = "1");
    
    
    // EN: Undefines a macro, matching -U behavior for fine-grained control.
    // FR: Annule une macro, comme -U, pour un controle fin.
    void undefineMacro(const std::string& name);
    
    
    // EN: Reports whether an error occurred so the driver can stop early.
    // FR: Indique si une erreur est survenue pour arreter tot.
    bool hadError() const { return hadError_; }
    
    
    // EN: Handles #define so new macros are visible to later lines.
    // FR: Traite #define pour rendre les macros visibles ensuite.
    void visit(pp::DefineDirective& dir) override;
    // EN: Handles #undef to remove macros. FR: Traite #undef pour supprimer.
    void visit(pp::UndefDirective& dir) override;
    // EN: Handles #include to splice in external content.
    // FR: Traite #include pour inserer un fichier.
    void visit(pp::IncludeDirective& dir) override;
    // EN: Handles #if by evaluating an expression for activation.
    // FR: Traite #if en evaluant l expression.
    void visit(pp::IfDirective& dir) override;
    // EN: Handles #ifdef by checking macro existence.
    // FR: Traite #ifdef en verifiant l existence d une macro.
    void visit(pp::IfdefDirective& dir) override;
    // EN: Handles #elif, ensuring only one branch becomes active.
    // FR: Traite #elif pour n activer qu une branche.
    void visit(pp::ElifDirective& dir) override;
    // EN: Handles #else, honoring prior true branches.
    // FR: Traite #else en respectant les branches vraies precedentes.
    void visit(pp::ElseDirective& dir) override;
    // EN: Handles #endif by unwinding the conditional stack.
    // FR: Traite #endif en depilant les conditions.
    void visit(pp::EndifDirective& dir) override;
    // EN: Emits an #error with the current location for precision.
    // FR: Emet un #error avec l emplacement courant.
    void visit(pp::ErrorDirective& dir) override;
    // EN: Handles #pragma; often ignored or logged by design.
    // FR: Traite #pragma ; souvent ignore ou journalise.
    void visit(pp::PragmaDirective& dir) override;
    // EN: Handles #line to remap logical filename and line numbers.
    // FR: Traite #line pour remapper fichier logique et numeros de ligne.
    void visit(pp::LineDirective& dir) override;
    
private:
    
    pp::MacroTable macroTable_;
    std::unique_ptr<pp::MacroExpander> macroExpander_;
    std::unique_ptr<pp::ExpressionEvaluator> exprEvaluator_;
    std::unique_ptr<pp::DirectiveParser> directiveParser_;
    std::unique_ptr<pp::FileHandler> fileHandler_;
    pp::CommentRemover commentRemover_;
    pp::StringConcatenator stringConcatenator_;
    
    
    std::string currentFile_;
    int currentLine_ = 1;
    bool hadError_ = false;
    std::stack<ConditionalState> conditionalStack_;
    std::string* currentOutput_ = nullptr;  
    
    
    // EN: Loads and preprocesses a file, centralizing IO and error reporting.
    // FR: Charge et pretraite un fichier, centralise IO et erreurs.
    std::string processFile(const std::string& filename);
    // EN: Processes a source buffer with a logical filename for reuse in-memory.
    // FR: Traite un buffer avec un nom logique pour reuse en memoire.
    std::string processSource(const std::string& source, const std::string& filename);
    // EN: Processes one line, applying directives or macro expansion as needed.
    // FR: Traite une ligne, applique directives ou expansion.
    std::string processLine(const std::string& line);
    
    
    // EN: Returns whether current conditional stack allows output.
    // FR: Indique si la pile conditionnelle autorise la sortie.
    bool isActive() const;
    
    
    // EN: Initializes builtin macros like __FILE__/__LINE__ to match compilers.
    // FR: Initialise les macros internes pour coller aux compilateurs.
    void setupBuiltinMacros();
    
    
    // EN: Emits an error at the current logical location and marks failure.
    // FR: Emet une erreur a l emplacement logique et marque l echec.
    void error(const std::string& message);
    // EN: Emits a warning at the current logical location without stopping.
    // FR: Emet un avertissement a l emplacement logique sans arreter.
    void warning(const std::string& message);
};

// TODO(cc1) EN: Decide if diagnostics should also cover inactive branches to
// match vendor preprocessors. FR: Decider si les diagnostics doivent couvrir
// les branches inactives pour coller aux preprocessseurs existants.
} 
