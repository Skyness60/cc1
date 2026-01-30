#include <preprocessor/PPExprEval.hpp>

namespace cc1 {
namespace pp {

// EN: Evaluates bit shifts (<<, >>) with correct precedence.
// FR: Evalue les decalages (<<, >>) avec la bonne precedence.
long long ExpressionEvaluator::evalShift(const std::string& expr, size_t& pos) {
    long long left = evalAdditive(expr, pos);

    while (true) {
        skipWhitespace(expr, pos);
        if (pos + 1 < expr.size() && expr[pos] == '<' && expr[pos + 1] == '<') {
            pos += 2;
            long long right = evalAdditive(expr, pos);
            left = left << right;
        } else if (pos + 1 < expr.size() && expr[pos] == '>' && expr[pos + 1] == '>') {
            pos += 2;
            long long right = evalAdditive(expr, pos);
            left = left >> right;
        } else {
            break;
        }
    }

    return left;
}

// EN: Evaluates addition/subtraction (+, -).
// FR: Evalue addition/soustraction (+, -).
long long ExpressionEvaluator::evalAdditive(const std::string& expr, size_t& pos) {
    long long left = evalMultiplicative(expr, pos);

    while (true) {
        skipWhitespace(expr, pos);
        if (pos < expr.size() && expr[pos] == '+') {
            pos++;
            long long right = evalMultiplicative(expr, pos);
            left = left + right;
        } else if (pos < expr.size() && expr[pos] == '-') {
            pos++;
            long long right = evalMultiplicative(expr, pos);
            left = left - right;
        } else {
            break;
        }
    }

    return left;
}

} 
} 

// TODO(cc1) EN: Add overflow checks for shifts and arithmetic where needed.
// FR: Ajouter des checks d overflow pour shifts et arithmetique si besoin.
