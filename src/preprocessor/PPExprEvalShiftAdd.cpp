#include <preprocessor/PPExprEval.hpp>

namespace cc1 {
namespace pp {

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

} // namespace pp
} // namespace cc1
