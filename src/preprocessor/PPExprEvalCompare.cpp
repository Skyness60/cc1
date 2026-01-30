#include <preprocessor/PPExprEval.hpp>

namespace cc1 {
namespace pp {

// EN: Evaluates equality operators (==, !=).
// FR: Evalue les operateurs d egalite (==, !=).
long long ExpressionEvaluator::evalEquality(const std::string& expr, size_t& pos) {
    long long left = evalRelational(expr, pos);

    while (true) {
        skipWhitespace(expr, pos);
        if (pos + 1 < expr.size() && expr[pos] == '=' && expr[pos + 1] == '=') {
            pos += 2;
            long long right = evalRelational(expr, pos);
            left = (left == right) ? 1 : 0;
        } else if (pos + 1 < expr.size() && expr[pos] == '!' && expr[pos + 1] == '=') {
            pos += 2;
            long long right = evalRelational(expr, pos);
            left = (left != right) ? 1 : 0;
        } else {
            break;
        }
    }

    return left;
}

// EN: Evaluates relational operators (<, >, <=, >=).
// FR: Evalue les operateurs relationnels (<, >, <=, >=).
long long ExpressionEvaluator::evalRelational(const std::string& expr, size_t& pos) {
    long long left = evalShift(expr, pos);

    while (true) {
        skipWhitespace(expr, pos);
        if (pos + 1 < expr.size() && expr[pos] == '<' && expr[pos + 1] == '=') {
            pos += 2;
            long long right = evalShift(expr, pos);
            left = (left <= right) ? 1 : 0;
        } else if (pos + 1 < expr.size() && expr[pos] == '>' && expr[pos + 1] == '=') {
            pos += 2;
            long long right = evalShift(expr, pos);
            left = (left >= right) ? 1 : 0;
        } else if (pos < expr.size() && expr[pos] == '<' &&
                   (pos + 1 >= expr.size() || expr[pos + 1] != '<')) {
            pos++;
            long long right = evalShift(expr, pos);
            left = (left < right) ? 1 : 0;
        } else if (pos < expr.size() && expr[pos] == '>' &&
                   (pos + 1 >= expr.size() || expr[pos + 1] != '>')) {
            pos++;
            long long right = evalShift(expr, pos);
            left = (left > right) ? 1 : 0;
        } else {
            break;
        }
    }

    return left;
}

} 
} 

// TODO(cc1) EN: Add signed/unsigned comparison semantics for literal suffixes.
// FR: Ajouter les semantiques de comparaison signe/unsigned selon suffixes.
