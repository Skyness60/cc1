#include <preprocessor/PPExprEval.hpp>

namespace cc1 {
namespace pp {

// EN: Evaluates bitwise OR (|) with correct precedence.
// FR: Evalue le OR bit a bit (|) avec la bonne precedence.
long long ExpressionEvaluator::evalBitwiseOr(const std::string& expr, size_t& pos) {
    long long left = evalBitwiseXor(expr, pos);

    while (true) {
        skipWhitespace(expr, pos);
        if (pos < expr.size() && expr[pos] == '|' &&
            (pos + 1 >= expr.size() || expr[pos + 1] != '|')) {
            pos++;
            long long right = evalBitwiseXor(expr, pos);
            left = left | right;
        } else {
            break;
        }
    }

    return left;
}

// EN: Evaluates bitwise XOR (^) with correct precedence.
// FR: Evalue le XOR bit a bit (^) avec la bonne precedence.
long long ExpressionEvaluator::evalBitwiseXor(const std::string& expr, size_t& pos) {
    long long left = evalBitwiseAnd(expr, pos);

    while (true) {
        skipWhitespace(expr, pos);
        if (pos < expr.size() && expr[pos] == '^') {
            pos++;
            long long right = evalBitwiseAnd(expr, pos);
            left = left ^ right;
        } else {
            break;
        }
    }

    return left;
}

// EN: Evaluates bitwise AND (&) with correct precedence.
// FR: Evalue le AND bit a bit (&) avec la bonne precedence.
long long ExpressionEvaluator::evalBitwiseAnd(const std::string& expr, size_t& pos) {
    long long left = evalEquality(expr, pos);

    while (true) {
        skipWhitespace(expr, pos);
        if (pos < expr.size() && expr[pos] == '&' &&
            (pos + 1 >= expr.size() || expr[pos + 1] != '&')) {
            pos++;
            long long right = evalEquality(expr, pos);
            left = left & right;
        } else {
            break;
        }
    }

    return left;
}

} 
} 

// TODO(cc1) EN: Add diagnostics for invalid operator sequences like "| |".
// FR: Ajouter des diagnostics pour sequences d operateurs invalides.
