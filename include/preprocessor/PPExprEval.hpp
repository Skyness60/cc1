#pragma once

#include <string>
#include <preprocessor/PPMacro.hpp>

namespace cc1 {
namespace pp {

// ============================================================================
// Expression Evaluator - evaluates #if expressions
// ============================================================================

class ExpressionEvaluator {
public:
    explicit ExpressionEvaluator(MacroTable& table);
    
    // Evaluate a preprocessor expression (returns 0 or non-zero)
    long long evaluate(const std::string& expression);
    
    // Check for errors
    bool hadError() const { return hadError_; }
    std::string getError() const { return errorMessage_; }
    
private:
    MacroTable& macroTable_;
    bool hadError_ = false;
    std::string errorMessage_;
    
    // Recursive descent parser for expressions
    long long evalTernary(const std::string& expr, size_t& pos);
    long long evalLogicalOr(const std::string& expr, size_t& pos);
    long long evalLogicalAnd(const std::string& expr, size_t& pos);
    long long evalBitwiseOr(const std::string& expr, size_t& pos);
    long long evalBitwiseXor(const std::string& expr, size_t& pos);
    long long evalBitwiseAnd(const std::string& expr, size_t& pos);
    long long evalEquality(const std::string& expr, size_t& pos);
    long long evalRelational(const std::string& expr, size_t& pos);
    long long evalShift(const std::string& expr, size_t& pos);
    long long evalAdditive(const std::string& expr, size_t& pos);
    long long evalMultiplicative(const std::string& expr, size_t& pos);
    long long evalUnary(const std::string& expr, size_t& pos);
    long long evalPrimary(const std::string& expr, size_t& pos);
    
    // Utilities
    void skipWhitespace(const std::string& expr, size_t& pos);
    bool isIdentifierStart(char c) const;
    bool isIdentifierChar(char c) const;
    std::string parseIdentifier(const std::string& expr, size_t& pos);
    long long parseNumber(const std::string& expr, size_t& pos);
    
    void error(const std::string& message);
};

} // namespace pp
} // namespace cc1
