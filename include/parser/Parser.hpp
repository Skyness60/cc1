#pragma once

#include <lexer/Token.hpp>
#include <parser/ast/AST.hpp>
#include <parser/ParseError.hpp>
#include <utils/SourceLocation.hpp>
#include <vector>


#include <string>
#include <set>
#include <map>

namespace cc1 {


// EN: Recursive-descent parser for C-like syntax, producing an AST.
// FR: Parseur recursive-descendant pour une syntaxe type C, produisant un AST.
class Parser {
public:
    
    // EN: Initializes the parser with tokens and source context for diagnostics.
    // FR: Initialise le parseur avec tokens et contexte source pour diagnostics.
    Parser(const std::vector<Token>& tokens, 
           const std::string& filename,
           const std::string& source);
    
    
    // EN: Parses a translation unit and returns the root AST node.
    // FR: Parse une unite de traduction et renvoie la racine de l AST.
    AST::Ptr<AST::TranslationUnit> parse();
    
    
    // EN: Reports whether a parse error occurred.
    // FR: Indique si une erreur de parsing est survenue.
    bool hadError() const { return hadError_; }

private:
    
    
    
    
    // EN: Returns the current token under the cursor.
    // FR: Renvoie le token courant sous le curseur.
    const Token& current() const;
    // EN: Peeks ahead by an offset without consuming tokens.
    // FR: Regarde en avant d un offset sans consommer.
    const Token& peek(int offset = 1) const;
    // EN: Returns the most recently consumed token.
    // FR: Renvoie le token consomme le plus recent.
    const Token& previous() const;
    
    // EN: Checks if the parser has reached end of file.
    // FR: Verifie si la fin de fichier est atteinte.
    bool isAtEnd() const;
    // EN: Checks whether the current token matches a type.
    // FR: Verifie si le token courant correspond au type.
    bool check(TokenType type) const;
    // EN: Checks whether the current token matches any given types.
    // FR: Verifie si le token courant correspond a un des types donnes.
    bool checkAny(std::initializer_list<TokenType> types) const;
    
    // EN: Consumes the current token and advances.
    // FR: Consomme le token courant et avance.
    const Token& advance();
    // EN: Consumes the token if it matches the given type.
    // FR: Consomme le token s il correspond au type donne.
    bool match(TokenType type);
    // EN: Consumes the token if it matches any given type.
    // FR: Consomme le token s il correspond a un des types donnes.
    bool matchAny(std::initializer_list<TokenType> types);
    
    // EN: Consumes a required token or raises a parse error.
    // FR: Consomme un token requis ou leve une erreur de parse.
    const Token& consume(TokenType type, const std::string& message);
    // EN: Synchronizes after an error to a safe parsing point.
    // FR: Synchronise apres erreur vers un point de reprise.
    void synchronize();
    
    
    
    
    
    // EN: Raises a parse error at the current token.
    // FR: Leve une erreur de parse au token courant.
    [[noreturn]] void error(const std::string& message);
    // EN: Raises a parse error at a specific token.
    // FR: Leve une erreur de parse a un token specifique.
    [[noreturn]] void errorAt(const Token& token, const std::string& message);
    // EN: Raises a parse error at an explicit line/column position.
    // FR: Leve une erreur de parse a une position ligne/colonne explicite.
    [[noreturn]] void errorAtPosition(int line, int column, const std::string& message);
    
    
    
    
    
    
    struct DeclSpecifiers {
        AST::StorageClass storageClass = AST::StorageClass::None;
        AST::Ptr<AST::Type> type;
        bool isTypedef = false;
        bool isConst = false;
        bool isVolatile = false;
        int storageClassLine = 0;
        int storageClassColumn = 0;
    };
    
    
    struct Declarator {
        std::string name;
        AST::Ptr<AST::Type> type;
        std::vector<AST::Ptr<AST::ParamDecl>> params;  
        bool isVariadic = false;
        int line = 0;
        int column = 0;
    };
    
    // EN: Parses an external declaration (function/variable/typedef).
    // FR: Parse une declaration externe (fonction/variable/typedef).
    AST::Ptr<AST::Declaration> parseExternalDeclaration();
    // EN: Parses a declaration within a block or global scope.
    // FR: Parse une declaration dans un bloc ou scope global.
    AST::Ptr<AST::Declaration> parseDeclaration();
    // EN: Parses a list of declarations.
    // FR: Parse une liste de declarations.
    std::vector<AST::Ptr<AST::Declaration>> parseDeclarations();
    
    // EN: Parses declaration specifiers (storage class, type, qualifiers).
    // FR: Parse les specifiers (storage class, type, qualifiers).
    DeclSpecifiers parseDeclarationSpecifiers();
    // EN: Checks if the current token starts a declaration specifier.
    // FR: Verifie si le token courant demarre un specifier de declaration.
    bool isDeclarationSpecifier() const;
    // EN: Checks if the current token is a type specifier.
    // FR: Verifie si le token courant est un type specifier.
    bool isTypeSpecifier() const;
    // EN: Checks if the current token is a storage class specifier.
    // FR: Verifie si le token courant est un storage class specifier.
    bool isStorageClassSpecifier() const;
    // EN: Checks if the current token is a type qualifier.
    // FR: Verifie si le token courant est un type qualifier.
    bool isTypeQualifier() const;
    
    // EN: Parses a declarator with a given base type.
    // FR: Parse un declarator avec un type de base.
    Declarator parseDeclarator(const AST::Ptr<AST::Type>& baseType);
    // EN: Parses the direct declarator portion (names, params, arrays).
    // FR: Parse la partie declarator directe (noms, params, tableaux).
    Declarator parseDirectDeclarator(const AST::Ptr<AST::Type>& baseType);
    // EN: Parses pointer qualifiers and nests them into the type.
    // FR: Parse les qualifiers de pointeur et les imbrique dans le type.
    AST::Ptr<AST::Type> parsePointer(AST::Ptr<AST::Type> baseType);
    
    // EN: Parses a function definition from specifiers and declarator.
    // FR: Parse une definition de fonction depuis specifiers et declarator.
    AST::Ptr<AST::FunctionDecl> parseFunctionDefinition(
        const DeclSpecifiers& specs, const Declarator& decl);
    // EN: Parses a parameter list and detects variadic usage.
    // FR: Parse une liste de parametres et detecte le variadique.
    std::vector<AST::Ptr<AST::ParamDecl>> parseParameterList(bool& isVariadic);
    // EN: Parses a single parameter declaration.
    // FR: Parse une declaration de parametre.
    AST::Ptr<AST::ParamDecl> parseParameterDeclaration();
    
    // EN: Parses struct/union specifiers and their members.
    // FR: Parse les specifiers struct/union et leurs membres.
    AST::Ptr<AST::Type> parseStructOrUnionSpecifier();
    // EN: Parses an enum specifier and its enumerators.
    // FR: Parse un enum specifier et ses enumerateurs.
    AST::Ptr<AST::Type> parseEnumSpecifier();
    // EN: Parses a list of struct member declarations.
    // FR: Parse une liste de membres de struct.
    std::vector<AST::Ptr<AST::VarDecl>> parseStructMemberList();
    
    // EN: Parses an initializer for a declarator.
    // FR: Parse un initialiseur pour un declarator.
    AST::Ptr<AST::Expression> parseInitializer();
    // EN: Parses a brace-enclosed initializer list.
    // FR: Parse une liste d initialisation entre accolades.
    AST::Ptr<AST::InitializerList> parseInitializerList();
    
    
    
    
    
    // EN: Parses a generic statement and dispatches by leading token.
    // FR: Parse une instruction generique et dispatch selon le token.
    AST::Ptr<AST::Statement> parseStatement();
    // EN: Parses a compound statement (block).
    // FR: Parse une instruction composee (bloc).
    AST::Ptr<AST::CompoundStmt> parseCompoundStatement();
    // EN: Parses an if/else statement.
    // FR: Parse une instruction if/else.
    AST::Ptr<AST::Statement> parseIfStatement();
    // EN: Parses a while loop.
    // FR: Parse une boucle while.
    AST::Ptr<AST::Statement> parseWhileStatement();
    // EN: Parses a do-while loop.
    // FR: Parse une boucle do-while.
    AST::Ptr<AST::Statement> parseDoWhileStatement();
    // EN: Parses a for loop.
    // FR: Parse une boucle for.
    AST::Ptr<AST::Statement> parseForStatement();
    // EN: Parses a switch statement.
    // FR: Parse une instruction switch.
    AST::Ptr<AST::Statement> parseSwitchStatement();
    // EN: Parses a case label and its statement.
    // FR: Parse un label case et son instruction.
    AST::Ptr<AST::Statement> parseCaseStatement();
    // EN: Parses a default label and its statement.
    // FR: Parse un label default et son instruction.
    AST::Ptr<AST::Statement> parseDefaultStatement();
    // EN: Parses a return statement.
    // FR: Parse une instruction return.
    AST::Ptr<AST::Statement> parseReturnStatement();
    // EN: Parses a goto statement.
    // FR: Parse une instruction goto.
    AST::Ptr<AST::Statement> parseGotoStatement();
    // EN: Parses a break statement.
    // FR: Parse une instruction break.
    AST::Ptr<AST::Statement> parseBreakStatement();
    // EN: Parses a continue statement.
    // FR: Parse une instruction continue.
    AST::Ptr<AST::Statement> parseContinueStatement();
    // EN: Parses a labeled statement (identifier:).
    // FR: Parse une instruction etiquetee (identifiant:).
    AST::Ptr<AST::Statement> parseLabeledStatement();
    // EN: Parses an expression statement or empty statement.
    // FR: Parse une instruction expression ou vide.
    AST::Ptr<AST::Statement> parseExpressionStatement();
    
    
    
    
    
    // EN: Parses a full expression (comma-separated).
    // FR: Parse une expression complete (separee par virgules).
    AST::Ptr<AST::Expression> parseExpression();
    // EN: Parses an assignment expression.
    // FR: Parse une expression d affectation.
    AST::Ptr<AST::Expression> parseAssignmentExpression();
    // EN: Parses a ternary conditional expression.
    // FR: Parse une expression conditionnelle ternaire.
    AST::Ptr<AST::Expression> parseConditionalExpression();
    // EN: Parses logical OR expressions.
    // FR: Parse les expressions OR logique.
    AST::Ptr<AST::Expression> parseLogicalOrExpression();
    // EN: Parses logical AND expressions.
    // FR: Parse les expressions AND logique.
    AST::Ptr<AST::Expression> parseLogicalAndExpression();
    // EN: Parses bitwise OR expressions.
    // FR: Parse les expressions OR bit a bit.
    AST::Ptr<AST::Expression> parseBitwiseOrExpression();
    // EN: Parses bitwise XOR expressions.
    // FR: Parse les expressions XOR bit a bit.
    AST::Ptr<AST::Expression> parseBitwiseXorExpression();
    // EN: Parses bitwise AND expressions.
    // FR: Parse les expressions AND bit a bit.
    AST::Ptr<AST::Expression> parseBitwiseAndExpression();
    // EN: Parses equality expressions (==, !=).
    // FR: Parse les expressions d egalite (==, !=).
    AST::Ptr<AST::Expression> parseEqualityExpression();
    // EN: Parses relational expressions (<, >, <=, >=).
    // FR: Parse les expressions relationnelles (<, >, <=, >=).
    AST::Ptr<AST::Expression> parseRelationalExpression();
    // EN: Parses shift expressions (<<, >>).
    // FR: Parse les expressions de decalage (<<, >>).
    AST::Ptr<AST::Expression> parseShiftExpression();
    // EN: Parses additive expressions (+, -).
    // FR: Parse les expressions additives (+, -).
    AST::Ptr<AST::Expression> parseAdditiveExpression();
    // EN: Parses multiplicative expressions (*, /, %).
    // FR: Parse les expressions multiplicatives (*, /, %).
    AST::Ptr<AST::Expression> parseMultiplicativeExpression();
    // EN: Parses cast expressions.
    // FR: Parse les expressions de cast.
    AST::Ptr<AST::Expression> parseCastExpression();
    // EN: Parses unary expressions.
    // FR: Parse les expressions unaires.
    AST::Ptr<AST::Expression> parseUnaryExpression();
    // EN: Parses postfix expressions (calls, indexing, etc.).
    // FR: Parse les expressions postfixees (appels, index, etc.).
    AST::Ptr<AST::Expression> parsePostfixExpression();
    // EN: Parses primary expressions.
    // FR: Parse les expressions primaires.
    AST::Ptr<AST::Expression> parsePrimaryExpression();
    // EN: Parses an integer literal primary.
    // FR: Parse un litteral entier primaire.
    AST::Ptr<AST::Expression> parsePrimaryIntegerLiteral();
    // EN: Parses a float literal primary.
    // FR: Parse un litteral flottant primaire.
    AST::Ptr<AST::Expression> parsePrimaryFloatLiteral();
    // EN: Parses a string literal primary.
    // FR: Parse un litteral chaine primaire.
    AST::Ptr<AST::Expression> parsePrimaryStringLiteral();
    // EN: Parses a char literal primary.
    // FR: Parse un litteral caractere primaire.
    AST::Ptr<AST::Expression> parsePrimaryCharLiteral();
    // EN: Parses a sizeof expression.
    // FR: Parse une expression sizeof.
    AST::Ptr<AST::Expression> parseSizeofExpression();
    // EN: Parses a constant expression (for case/enum).
    // FR: Parse une expression constante (case/enum).
    AST::Ptr<AST::Expression> parseConstantExpression();
    
    
    
    
    
    // EN: Parses a typename (used in casts and sizeof).
    // FR: Parse un typename (utilise dans cast et sizeof).
    AST::Ptr<AST::Type> parseTypeName();
    // EN: Parses an abstract declarator for type-only constructs.
    // FR: Parse un declarator abstrait pour types seuls.
    AST::Ptr<AST::Type> parseAbstractDeclarator(AST::Ptr<AST::Type> baseType);
    
    
    
    
    
    // EN: Checks if the current token begins a type name.
    // FR: Verifie si le token courant commence un type name.
    bool isTypeName() const;
    // EN: Checks if the current token begins a declaration.
    // FR: Verifie si le token courant commence une declaration.
    bool isDeclarationStart() const;
    
    // EN: Maps token types to AST binary operators.
    // FR: Mappe les tokens vers operateurs binaires AST.
    AST::BinaryOp tokenToBinaryOp(TokenType type) const;
    // EN: Maps token types to AST unary operators.
    // FR: Mappe les tokens vers operateurs unaires AST.
    AST::UnaryOp tokenToUnaryOp(TokenType type) const;
    // EN: Maps token types to AST postfix operators.
    // FR: Mappe les tokens vers operateurs postfixes AST.
    AST::UnaryOp tokenToPostfixOp(TokenType type) const;
    
    // EN: Returns a source line for diagnostics.
    // FR: Renvoie une ligne source pour diagnostics.
    std::string getSourceLine(int line) const;
    // EN: Builds a SourceLocation from a token.
    // FR: Construit une SourceLocation depuis un token.
    SourceLocation makeLocation(const Token& tok) const;
    
    
    
    
    
    const std::vector<Token>& tokens_;
    std::string filename_;
    std::string source_;
    size_t currentIndex_ = 0;
    bool hadError_ = false;
    
    
    std::set<std::string> typedefNames_;
    
    
    std::map<std::string, std::string> globalIdentifiers_;
    
    
    std::set<std::string> definedFunctions_;
    
    
    std::vector<AST::Ptr<AST::Declaration>> additionalDeclarations_;
    
    
    int switchDepth_ = 0;     
    int functionDepth_ = 0;   
};

// TODO(cc1) EN: Add incremental parsing or error recovery improvements.
// FR: Ajouter du parsing incrementiel ou ameliorer la recuperation d erreurs.
} 
