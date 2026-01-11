#include <preprocessor/PPExprEval.hpp>

namespace cc1 {
namespace pp {

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

} // namespace pp
} // namespace cc1
