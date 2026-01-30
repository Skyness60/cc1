#pragma once

#include <parser/ast/Visitor.hpp>
#include <parser/ast/Type.hpp>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <tuple>
#include <vector>
#include <stack>
#include <fstream>

namespace cc1 {

// EN: Simple logger for codegen debugging, writing to a file when enabled.
// FR: Logger simple pour debug codegen, ecrit dans un fichier si active.
class DebugLogger {
public:
    // EN: Returns the singleton logger instance.
    // FR: Renvoie l instance singleton du logger.
    static DebugLogger& instance() {
        static DebugLogger logger;
        return logger;
    }
    
    // EN: Enables or disables logging.
    // FR: Active ou desactive le logging.
    void setEnabled(bool enabled) { enabled_ = enabled; }
    // EN: Sets the output file for log messages.
    // FR: Definit le fichier de sortie des logs.
    void setOutputFile(const std::string& filename) {
        output_.open(filename, std::ios::app);
    }
    
    // EN: Logs a raw message if logging is enabled.
    // FR: Log un message brut si le logging est actif.
    void log(const std::string& message) {
        if (!enabled_) return;
        output_ << message << "\n";
        output_.flush();
    }
    
    // EN: Logs expression info for debug traces.
    // FR: Log les infos d expression pour traces debug.
    void logExpr(const std::string& exprName, const std::string& type, 
                 const std::string& name, bool isPointer, bool isConstant) {
        if (!enabled_) return;
        output_ << "[EXPR] " << exprName << " -> type='" << type << "' name='" << name 
               << "' isPtr=" << isPointer << " isConst=" << isConstant << "\n";
        output_.flush();
    }

private:
    DebugLogger() = default;
    bool enabled_ = false;
    std::ofstream output_;
};

// EN: Represents a value in IR (name, type, pointer/constant metadata).
// FR: Represente une valeur IR (nom, type, pointeur/constante).
struct IRValue {
    std::string name;       
    std::string type;       
    bool isPointer = false; 
    bool isConstant = false;

    
    
    bool isBitfieldRef = false;
    std::string bitfieldStorageType; 
    int bitfieldOffset = 0;          
    int bitfieldWidth = 0;           
    bool bitfieldIsUnsigned = true;  
    
    // EN: Default constructor for an empty IR value.
    // FR: Constructeur par defaut pour une valeur IR vide.
    IRValue() = default;
    // EN: Builds an IR value with basic metadata.
    // FR: Construit une valeur IR avec les metadonnees de base.
    IRValue(const std::string& n, const std::string& t, bool ptr = false, bool cst = false)
        : name(n), type(t), isPointer(ptr), isConstant(cst) {}
    
    
    // EN: Returns the dereferenced type (strips one trailing '*').
    // FR: Renvoie le type dereference (retire un '*' final).
    std::string derefType() const {
        if (type.size() > 1 && type.back() == '*') {
            return type.substr(0, type.size() - 1);
        }
        return type;
    }
};

// EN: Tracks symbol metadata for IR generation.
// FR: Stocke les metadonnees de symboles pour l IR.
struct IRSymbol {
    std::string name;           
    std::string irName;         
    std::string type;           
    bool isGlobal = false;
    bool isFunction = false;
    bool isParameter = false;
    int paramIndex = -1;
};

// EN: AST visitor that emits LLVM IR for declarations, statements, expressions.
// FR: Visiteur AST qui genere l IR LLVM pour decls, statements, expressions.
class IRGenerator : public AST::DefaultVisitor {
public:
    // EN: Constructs a generator for the chosen target width.
    // FR: Construit un generateur pour la largeur cible.
    IRGenerator(bool is64bit = false);

    // EN: Enables debug info emission and sets the main filename.
    // FR: Active l emission debug et fixe le fichier principal.
    void setDebugInfo(bool enabled, const std::string& primaryFilename);
    
    
    // EN: Generates IR for a translation unit.
    // FR: Genere l IR pour une unite de traduction.
    void generate(AST::TranslationUnit& unit);
    
    
    // EN: Returns the full IR module as a string.
    // FR: Renvoie le module IR complet en string.
    std::string getIR() const;
    
    
    // EN: Reports whether codegen has encountered errors.
    // FR: Indique si le codegen a rencontre des erreurs.
    bool hadError() const { return hadError_; }

    
    
    
    // EN: Visits a translation unit.
    // FR: Visite une unite de traduction.
    void visit(AST::TranslationUnit& node) override;
    // EN: Visits a variable declaration.
    // FR: Visite une declaration de variable.
    void visit(AST::VarDecl& node) override;
    // EN: Visits a function declaration/definition.
    // FR: Visite une declaration/definition de fonction.
    void visit(AST::FunctionDecl& node) override;
    // EN: Visits a parameter declaration (no-op for codegen).
    // FR: Visite une declaration de parametre (no-op codegen).
    void visit(AST::ParamDecl& node) override;
    // EN: Visits a struct declaration.
    // FR: Visite une declaration de struct.
    void visit(AST::StructDecl& node) override;
    // EN: Visits an enum declaration.
    // FR: Visite une declaration d enum.
    void visit(AST::EnumDecl& node) override;
    
    
    
    
    // EN: Visits a compound statement.
    // FR: Visite un bloc compose.
    void visit(AST::CompoundStmt& node) override;
    // EN: Visits an expression statement.
    // FR: Visite une instruction d expression.
    void visit(AST::ExpressionStmt& node) override;
    // EN: Visits an if statement.
    // FR: Visite une instruction if.
    void visit(AST::IfStmt& node) override;
    // EN: Visits a while statement.
    // FR: Visite une instruction while.
    void visit(AST::WhileStmt& node) override;
    // EN: Visits a do-while statement.
    // FR: Visite une instruction do-while.
    void visit(AST::DoWhileStmt& node) override;
    // EN: Visits a for statement.
    // FR: Visite une instruction for.
    void visit(AST::ForStmt& node) override;
    // EN: Visits a switch statement.
    // FR: Visite une instruction switch.
    void visit(AST::SwitchStmt& node) override;
    // EN: Visits a case statement.
    // FR: Visite une instruction case.
    void visit(AST::CaseStmt& node) override;
    // EN: Visits a default statement.
    // FR: Visite une instruction default.
    void visit(AST::DefaultStmt& node) override;
    // EN: Visits a break statement.
    // FR: Visite une instruction break.
    void visit(AST::BreakStmt& node) override;
    // EN: Visits a continue statement.
    // FR: Visite une instruction continue.
    void visit(AST::ContinueStmt& node) override;
    // EN: Visits a return statement.
    // FR: Visite une instruction return.
    void visit(AST::ReturnStmt& node) override;
    // EN: Visits a goto statement.
    // FR: Visite une instruction goto.
    void visit(AST::GotoStmt& node) override;
    // EN: Visits a label statement.
    // FR: Visite une instruction label.
    void visit(AST::LabelStmt& node) override;
    
    
    
    
    // EN: Visits an integer literal.
    // FR: Visite un litteral entier.
    void visit(AST::IntegerLiteral& node) override;
    // EN: Visits a float literal.
    // FR: Visite un litteral flottant.
    void visit(AST::FloatLiteral& node) override;
    // EN: Visits a char literal.
    // FR: Visite un litteral caractere.
    void visit(AST::CharLiteral& node) override;
    // EN: Visits a string literal.
    // FR: Visite un litteral chaine.
    void visit(AST::StringLiteral& node) override;
    // EN: Visits an identifier.
    // FR: Visite un identifiant.
    void visit(AST::Identifier& node) override;
    // EN: Visits a binary expression.
    // FR: Visite une expression binaire.
    void visit(AST::BinaryExpr& node) override;
    // EN: Visits a unary expression.
    // FR: Visite une expression unaire.
    void visit(AST::UnaryExpr& node) override;
    // EN: Visits a cast expression.
    // FR: Visite une expression de cast.
    void visit(AST::CastExpr& node) override;
    // EN: Visits a sizeof expression.
    // FR: Visite une expression sizeof.
    void visit(AST::SizeofExpr& node) override;
    // EN: Visits a call expression.
    // FR: Visite une expression d appel.
    void visit(AST::CallExpr& node) override;
    // EN: Visits a member access expression.
    // FR: Visite une expression d acces membre.
    void visit(AST::MemberExpr& node) override;
    // EN: Visits an index expression.
    // FR: Visite une expression d indexation.
    void visit(AST::IndexExpr& node) override;
    // EN: Visits a ternary expression.
    // FR: Visite une expression ternaire.
    void visit(AST::TernaryExpr& node) override;
    // EN: Visits an initializer list expression.
    // FR: Visite une liste d initialisation.
    void visit(AST::InitializerList& node) override;

    
    
    
    // EN: Converts an AST type to an LLVM type string.
    // FR: Convertit un type AST en type LLVM.
    std::string typeToLLVM(AST::Type* type);
    // EN: Converts a primitive kind to an LLVM type string.
    // FR: Convertit un type primitif en type LLVM.
    std::string primitiveToLLVM(AST::PrimitiveKind kind);
    // EN: Computes byte size of a type.
    // FR: Calcule la taille en octets d un type.
    int getTypeSize(AST::Type* type);
    // EN: Computes alignment of a type.
    // FR: Calcule l alignement d un type.
    int getTypeAlign(AST::Type* type);
    // EN: Returns byte size of a primitive kind.
    // FR: Renvoie la taille d un type primitif.
    int getPrimitiveSize(AST::PrimitiveKind kind);
    // EN: Returns a default zero initializer for a type.
    // FR: Renvoie un initialiseur zero par defaut pour un type.
    std::string getDefaultValue(AST::Type* type);
    // EN: Builds an initializer value from an initializer list.
    // FR: Construit une valeur d init depuis une liste d init.
    std::string generateInitializerValue(AST::Type* type, AST::InitializerList* initList);
    // EN: Counts flattened members for initializer expansion.
    // FR: Compte les membres flatten pour expansion d init.
    size_t countFlattedMembers(AST::Type* type);
    // EN: Builds a struct initializer from a flattened list.
    // FR: Construit un init de struct depuis une liste flat.
    std::string generateStructInitializerFromFlatHelper(AST::StructType* st, AST::InitializerList* flatList, size_t& idx);
    // EN: Strips qualifiers/typedefs to a base type.
    // FR: Retire qualifiers/typedefs vers un type de base.
    AST::Type* stripQualifiers(AST::Type* type);

    
    
    
    // EN: Allocates a new SSA temporary name.
    // FR: Alloue un nouveau nom de temporaire SSA.
    std::string newTemp();          
    // EN: Allocates a new label name.
    // FR: Alloue un nouveau nom de label.
    std::string newLabel(const std::string& prefix = "L");
    // EN: Emits a global string and returns its symbol name.
    // FR: Emet une string globale et renvoie son symbole.
    std::string newGlobalString(const std::string& str);
    
    // EN: Emits an instruction to the current buffer.
    // FR: Emet une instruction dans le buffer courant.
    void emit(const std::string& ir);
    // EN: Emits a label and resets block termination.
    // FR: Emet un label et reinitialise la terminaison.
    void emitLabel(const std::string& label);
    // EN: Emits a comment line.
    // FR: Emet une ligne de commentaire.
    void emitComment(const std::string& comment);
    // EN: Emits a global definition line.
    // FR: Emet une ligne de definition globale.
    void emitGlobal(const std::string& ir);
    
    
    
    
    // EN: Loads a value from memory if needed.
    // FR: Charge une valeur depuis la memoire si besoin.
    IRValue loadValue(const IRValue& val);  
    // EN: Stores a value into a pointer location.
    // FR: Stocke une valeur dans un pointeur.
    IRValue storeValue(const IRValue& val, const IRValue& ptr);
    
    
    
    
    // EN: Enters a new symbol scope.
    // FR: Entre dans un nouveau scope de symboles.
    void enterScope();
    // EN: Exits the current symbol scope.
    // FR: Sort du scope courant.
    void exitScope();
    // EN: Looks up a symbol by name.
    // FR: Cherche un symbole par nom.
    IRSymbol* lookupSymbol(const std::string& name);
    // EN: Defines a symbol in the current scope.
    // FR: Definit un symbole dans le scope courant.
    void defineSymbol(const std::string& name, const IRSymbol& sym);
    
    
    
    
    // EN: Resolves a typedef name to a concrete type.
    // FR: Resout un typedef vers un type concret.
    AST::Type* resolveTypedef(const std::string& name);
    
    
    
    
    struct StructLayout {
        std::string llvmType;
        std::map<std::string, int> memberIndices;  
        std::map<std::string, std::string> memberTypes;  
        std::map<std::string, int> bitfieldOffsets;  
        std::map<std::string, int> bitfieldWidths;   
        std::map<std::string, bool> bitfieldIsUnsigned;  
        int totalSize = 0;
        int alignment = 1;
    };
    // EN: Computes layout for a struct declaration.
    // FR: Calcule le layout pour une declaration de struct.
    StructLayout computeStructLayout(AST::StructDecl* decl);
    // EN: Computes layout for a struct/union type.
    // FR: Calcule le layout pour un type struct/union.
    StructLayout computeStructLayout(AST::StructType* type);
    // EN: Looks up a named struct layout if cached.
    // FR: Cherche un layout de struct nommee en cache.
    const StructLayout* getStructLayout(const std::string& name);
    
    
    // EN: Registers named structs for type emission.
    // FR: Enregistre les structs nommees pour emission.
    void collectNamedStruct(AST::StructType* structType);
    
    
    // EN: Generates the LLVM definition for a struct type.
    // FR: Genere la definition LLVM pour un type struct.
    std::string generateStructTypeDefinition(AST::StructType* structType);
    
    
    
    // EN: Extracts field type from an inline LLVM struct type string.
    // FR: Extrait un type de champ d une struct LLVM inline.
    std::string extractFieldTypeFromInlineStruct(const std::string& inlineStructType, int fieldIndex);
    
    
    // EN: Registers inline struct members for later lookup.
    // FR: Enregistre les membres de struct inline pour lookup.
    void registerInlineStructMembers(const std::string& llvmType, AST::StructType* structType);
    
    
    // EN: Finds the member index for an inline struct type.
    // FR: Trouve l index d un membre dans une struct inline.
    int findMemberIndexInInlineStruct(const std::string& llvmType, const std::string& memberName);
    
    
    
    // EN: Builds flattened initializers for arrays of structs.
    // FR: Construit des init flatten pour tableaux de structs.
    std::string generateFlattenedInitializer(AST::ArrayType* arrayType, AST::InitializerList* initList);

    
    
    
    
    // EN: Evaluates integer constant expressions for init.
    // FR: Evalue les expressions constantes entieres pour init.
    bool evaluateConstantExpr(AST::Expression* expr, long long& result);

    
    
    // EN: Evaluates floating constant expressions for init.
    // FR: Evalue les expressions constantes flottantes pour init.
    bool evaluateConstantFloatExpr(AST::Expression* expr, double& result);

private:
    
    
    
    // EN: Generates unique names for anonymous structs.
    // FR: Genere des noms uniques pour structs anonymes.
    std::string nextAnonStructName();

    // EN: Generates an array initializer from a list.
    // FR: Genere un initialiseur de tableau depuis une liste.
    std::string generateArrayInitializerValue(AST::ArrayType* arrayType, AST::InitializerList* initList);
    // EN: Generates a struct initializer from a list.
    // FR: Genere un initialiseur de struct depuis une liste.
    std::string generateStructInitializerValue(AST::StructType* structType, AST::InitializerList* initList);

    // EN: Generates array initializer for struct element types.
    // FR: Genere un init de tableau pour elements struct.
    std::string generateArrayInitializerStruct(AST::ArrayType* arrayType, AST::StructType* structType, AST::InitializerList* initList,
                                               size_t arraySize, const std::string& elemType);
    // EN: Generates array initializer for non-struct element types.
    // FR: Genere un init de tableau pour elements non-struct.
    std::string generateArrayInitializerNonStruct(AST::ArrayType* arrayType, AST::StructType* structType, AST::InitializerList* initList,
                                                  size_t arraySize, const std::string& elemType);

    // EN: Generates a flattened struct initializer from nested lists.
    // FR: Genere un init de struct flatten depuis listes imbriquees.
    std::string generateStructFromInitForFlattened(AST::StructType* stype, AST::InitializerList* typeInitList);

    
    
    
    // EN: Formats float constants for LLVM IR.
    // FR: Formate les constantes float pour LLVM IR.
    std::string formatLLVMFloatConstant(float value);
    // EN: Emits a function prototype for function-typed vars.
    // FR: Emet un prototype pour variables typees fonction.
    void emitVarDeclFunctionPrototype(AST::VarDecl& node, AST::FunctionType& funcType);
    // EN: Emits a global variable declaration.
    // FR: Emet une declaration de variable globale.
    void emitGlobalVarDecl(AST::VarDecl& node, const std::string& llvmType);
    // EN: Emits a local variable declaration.
    // FR: Emet une declaration de variable locale.
    void emitLocalVarDecl(AST::VarDecl& node, const std::string& llvmType);

    // EN: Emits IR for assignment expressions.
    // FR: Emet l IR pour les affectations.
    bool emitBinaryAssign(AST::BinaryExpr& node);
    // EN: Emits IR for compound assignments.
    // FR: Emet l IR pour assignations composees.
    bool emitBinaryCompoundAssign(AST::BinaryExpr& node);
    // EN: Emits IR for logical short-circuiting.
    // FR: Emet l IR pour les logiques court-circuit.
    bool emitBinaryLogicalShortCircuit(AST::BinaryExpr& node);
    // EN: Emits IR for regular binary operators.
    // FR: Emet l IR pour les binaires classiques.
    void emitBinaryRegular(AST::BinaryExpr& node);

    // EN: Emits arithmetic/bitwise binary ops.
    // FR: Emet les ops binaires arith/bit.
    bool emitBinaryArithmetic(AST::BinaryExpr& node,
                             const std::string& lhsReg,
                             const std::string& lhsType,
                             const std::string& rhsReg,
                             const std::string& rhsType,
                             std::string& outResult,
                             std::string& outResultType);
    // EN: Emits comparison binary ops.
    // FR: Emet les ops binaires de comparaison.
    bool emitBinaryComparison(AST::BinaryExpr& node,
                             const IRValue& rhsVal,
                             const std::string& lhsReg,
                             const std::string& lhsType,
                             const std::string& rhsReg,
                             std::string& outResult,
                             std::string& outResultType);

    // EN: Computes union layout for a struct type.
    // FR: Calcule le layout union pour un type struct.
    StructLayout computeStructLayoutUnion(AST::StructType* type);
    // EN: Computes struct layout for a struct type.
    // FR: Calcule le layout struct pour un type struct.
    StructLayout computeStructLayoutStruct(AST::StructType* type);

    
    struct DebugLoc {
        int line = 0;
        int column = 0;
    };

    bool debugInfo_ = false;
    std::string debugFilename_;
    std::string debugDirectory_;

    int nextDebugMetaId_ = 4; 
    int diFileId_ = -1;
    int diCompileUnitId_ = -1;
    int currentSubprogramId_ = -1;
    std::vector<DebugLoc> debugLocStack_;
    std::map<std::tuple<int,int,int>, int> diLocationIds_; 
    std::stringstream debugMetaBuffer_;

    // EN: Initializes debug metadata nodes if needed.
    // FR: Initialise les metadonnees debug si besoin.
    void initDebugMetadataIfNeeded();
    // EN: Allocates a new debug metadata ID.
    // FR: Alloue un nouvel ID de metadonnees debug.
    int newDebugMetaId();
    // EN: Gets or creates a DILocation ID for a position.
    // FR: Recupere ou cree un ID DILocation pour une position.
    int getOrCreateDILocationId(int line, int col, int scopeId);
    // EN: Builds the debug suffix for current location.
    // FR: Construit le suffixe debug pour la position courante.
    std::string dbgSuffixForCurrentLoc();

    // EN: Pushes a debug location onto the stack.
    // FR: Empile une position debug.
    void pushDebugLoc(int line, int col);
    // EN: Pops the last debug location.
    // FR: Depile la derniere position debug.
    void popDebugLoc();

    struct DebugLocGuard {
        IRGenerator& gen;
        bool active;
        // EN: Pushes debug location on construction when enabled.
        // FR: Empile la position debug a la construction si active.
        DebugLocGuard(IRGenerator& g, int line, int col) : gen(g), active(g.debugInfo_) {
            if (active) gen.pushDebugLoc(line, col);
        }
        // EN: Pops debug location on destruction when enabled.
        // FR: Depile la position debug a la destruction si active.
        ~DebugLocGuard() {
            if (active) gen.popDebugLoc();
        }
    };

    
    
    
    std::stringstream headerBuffer_;    
    std::stringstream globalBuffer_;    
    std::stringstream stringBuffer_;    
    std::stringstream functionBuffer_;  
    std::stringstream declBuffer_;      
    std::stringstream funcDefBuffer_;   
    std::stringstream metadataBuffer_;  
    std::stringstream* currentBuffer_ = nullptr;
    
    int tempCounter_ = 0;
    int labelCounter_ = 0;
    int stringCounter_ = 0;
    int staticVarCounter_ = 0;  
    
    bool hadError_ = false;
    bool is64bit_ = false;
    
    
    AST::FunctionDecl* currentFunction_ = nullptr;
    std::string currentFunctionReturnType_;
    std::string returnLabel_;
    std::string returnValuePtr_;
    bool inGlobalScope_ = true;
    
    
    std::stack<std::string> breakLabels_;
    std::stack<std::string> continueLabels_;
    
    
    struct SwitchContext {
        std::string switchVar;
        std::string switchType;
        std::string defaultLabel;
        std::string endLabel;
        std::vector<std::pair<long long, std::string>> cases;
        
        std::vector<AST::Statement*> labelOrder;               
        std::map<AST::Statement*, std::string> labelForNode;   
    };
    std::stack<SwitchContext> switchStack_;

    
    
    bool blockTerminated_ = false;
    
    
    std::vector<std::map<std::string, IRSymbol>> scopes_;
    
    
    std::map<std::string, StructLayout> structLayouts_;
    
    
    std::map<std::string, std::map<std::string, int>> inlineStructMembers_;
    
    
    std::map<std::string, std::pair<std::string, AST::StructType*>> namedStructDefs_;
    
    
    std::map<std::string, AST::Type*> typedefMap_;
    
    
    std::map<std::string, long long> enumValues_;
    
    
    IRValue lastValue_;
    
    
    std::map<std::string, std::string> gotoLabels_;
    std::vector<std::string> pendingGotos_;  
    
    
    std::set<std::string> declaredFunctions_;
    std::set<std::string> definedFunctions_;  
    std::set<std::string> declaredGlobals_;
    std::map<std::string, std::string> functionDeclarations_;  
};

} 
