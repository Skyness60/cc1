#include <preprocessor/PPExprEval.hpp>

namespace cc1 {
namespace pp {

long long ExpressionEvaluator::evalLogicalOr(const std::string& expr, size_t& pos) {
    long long left = evalLogicalAnd(expr, pos);

    while (true) {
        skipWhitespace(expr, pos);
        if (pos + 1 < expr.size() && expr[pos] == '|' && expr[pos + 1] == '|') {
            pos += 2;
            long long right = evalLogicalAnd(expr, pos);
            left = (left || right) ? 1 : 0;
        } else {
            break;
        }
    }

    return left;
}

long long ExpressionEvaluator::evalLogicalAnd(const std::string& expr, size_t& pos) {
    long long left = evalBitwiseOr(expr, pos);

    while (true) {
        skipWhitespace(expr, pos);
        if (pos + 1 < expr.size() && expr[pos] == '&' && expr[pos + 1] == '&') {
            pos += 2;
            long long right = evalBitwiseOr(expr, pos);
            left = (left && right) ? 1 : 0;
        } else {
            break;
        }
    }

    return left;
}

} // namespace pp
} // namespace cc1
