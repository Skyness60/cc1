#pragma once



#include <string>
#include <preprocessor/PPMacro.hpp>

namespace cc1 {
namespace pp {





// EN: Evaluates preprocessor #if expressions with macro expansion and C-like
// operator precedence. FR: Evalue les expressions #if avec expansion de macros
// et priorite d operateurs type C.
class ExpressionEvaluator {
public:
    // EN: Binds the evaluator to the macro table for defined() and expansions.
    // FR: Lie l evaluateur a la table de macros pour defined() et expansions.
    explicit ExpressionEvaluator(MacroTable& table);
    
    
    // EN: Evaluates an expression string and returns its integer result.
    // FR: Evalue une expression et renvoie un entier.
    long long evaluate(const std::string& expression);
    
    
    // EN: Reports whether evaluation failed.
    // FR: Indique si l evaluation a echoue.
    bool hadError() const { return hadError_; }
    // EN: Returns the last error message, if any.
    // FR: Renvoie le dernier message d erreur.
    std::string getError() const { return errorMessage_; }
    
private:
    MacroTable& macroTable_;
    bool hadError_ = false;
    std::string errorMessage_;
    
    
    // EN: Parses ternary operators ?: at the current position.
    // FR: Parse l operateur ternaire ?: a la position courante.
    long long evalTernary(const std::string& expr, size_t& pos);
    // EN: Parses logical OR (||).
    // FR: Parse l OR logique (||).
    long long evalLogicalOr(const std::string& expr, size_t& pos);
    // EN: Parses logical AND (&&).
    // FR: Parse l AND logique (&&).
    long long evalLogicalAnd(const std::string& expr, size_t& pos);
    // EN: Parses bitwise OR (|).
    // FR: Parse l OR bit a bit (|).
    long long evalBitwiseOr(const std::string& expr, size_t& pos);
    // EN: Parses bitwise XOR (^).
    // FR: Parse le XOR bit a bit (^).
    long long evalBitwiseXor(const std::string& expr, size_t& pos);
    // EN: Parses bitwise AND (&).
    // FR: Parse le AND bit a bit (&).
    long long evalBitwiseAnd(const std::string& expr, size_t& pos);
    // EN: Parses equality operators (==, !=).
    // FR: Parse les egalites (==, !=).
    long long evalEquality(const std::string& expr, size_t& pos);
    // EN: Parses relational operators (<, >, <=, >=).
    // FR: Parse les relations (<, >, <=, >=).
    long long evalRelational(const std::string& expr, size_t& pos);
    // EN: Parses shift operators (<<, >>).
    // FR: Parse les decalages (<<, >>).
    long long evalShift(const std::string& expr, size_t& pos);
    // EN: Parses additive operators (+, -).
    // FR: Parse les additions (+, -).
    long long evalAdditive(const std::string& expr, size_t& pos);
    // EN: Parses multiplicative operators (*, /, %).
    // FR: Parse les multiplications (*, /, %).
    long long evalMultiplicative(const std::string& expr, size_t& pos);
    // EN: Parses unary operators (!, ~, +, -).
    // FR: Parse les unaires (!, ~, +, -).
    long long evalUnary(const std::string& expr, size_t& pos);
    // EN: Parses primary expressions (numbers, identifiers, parentheses).
    // FR: Parse les expressions primaires (nombres, identifiants, parentheses).
    long long evalPrimary(const std::string& expr, size_t& pos);
    
    
    // EN: Skips whitespace in the expression.
    // FR: Saute les espaces dans l expression.
    void skipWhitespace(const std::string& expr, size_t& pos);
    // EN: Tests identifier start characters.
    // FR: Teste le debut d identifiant.
    bool isIdentifierStart(char c) const;
    // EN: Tests identifier continuation characters.
    // FR: Teste la continuation d identifiant.
    bool isIdentifierChar(char c) const;
    // EN: Parses an identifier token.
    // FR: Parse un identifiant.
    std::string parseIdentifier(const std::string& expr, size_t& pos);
    // EN: Parses an integer literal.
    // FR: Parse un litteral entier.
    long long parseNumber(const std::string& expr, size_t& pos);
    
    // EN: Records an error message and marks evaluation failure.
    // FR: Enregistre une erreur et marque l echec.
    void error(const std::string& message);
};

// TODO(cc1) EN: Add full C integer literal suffix support in expression parsing.
// FR: Ajouter le support complet des suffixes d entiers C.
} 
} 
