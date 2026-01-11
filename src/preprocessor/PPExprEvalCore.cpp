#include <preprocessor/PPExprEval.hpp>

#include <cctype>

namespace cc1 {
namespace pp {

ExpressionEvaluator::ExpressionEvaluator(MacroTable& table)
    : macroTable_(table) {}

void ExpressionEvaluator::error(const std::string& message) {
    hadError_ = true;
    errorMessage_ = message;
}

void ExpressionEvaluator::skipWhitespace(const std::string& expr, size_t& pos) {
    while (pos < expr.size() && std::isspace(static_cast<unsigned char>(expr[pos]))) {
        pos++;
    }
}

bool ExpressionEvaluator::isIdentifierStart(char c) const {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

bool ExpressionEvaluator::isIdentifierChar(char c) const {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

std::string ExpressionEvaluator::parseIdentifier(const std::string& expr, size_t& pos) {
    std::string result;
    if (pos < expr.size() && isIdentifierStart(expr[pos])) {
        result += expr[pos++];
        while (pos < expr.size() && isIdentifierChar(expr[pos])) {
            result += expr[pos++];
        }
    }
    return result;
}

long long ExpressionEvaluator::parseNumber(const std::string& str, size_t& pos) {
    long long val = 0;
    int base = 10;

    if (pos + 1 < str.size() && str[pos] == '0') {
        if (str[pos + 1] == 'x' || str[pos + 1] == 'X') {
            base = 16;
            pos += 2;
        } else if (std::isdigit(static_cast<unsigned char>(str[pos + 1]))) {
            base = 8;
            pos++;
        }
    }

    while (pos < str.size()) {
        char c = str[pos];
        int digit = -1;

        if (c >= '0' && c <= '9') {
            digit = c - '0';
        } else if (base == 16 && c >= 'a' && c <= 'f') {
            digit = c - 'a' + 10;
        } else if (base == 16 && c >= 'A' && c <= 'F') {
            digit = c - 'A' + 10;
        }

        if (digit < 0 || digit >= base) {
            break;
        }

        val = val * base + digit;
        pos++;
    }

    while (pos < str.size() &&
           (str[pos] == 'u' || str[pos] == 'U' || str[pos] == 'l' || str[pos] == 'L')) {
        pos++;
    }

    return val;
}

long long ExpressionEvaluator::evaluate(const std::string& expression) {
    hadError_ = false;
    errorMessage_.clear();
    size_t pos = 0;
    return evalTernary(expression, pos);
}

long long ExpressionEvaluator::evalTernary(const std::string& expr, size_t& pos) {
    long long cond = evalLogicalOr(expr, pos);
    skipWhitespace(expr, pos);

    if (pos < expr.size() && expr[pos] == '?') {
        pos++;
        long long trueVal = evalTernary(expr, pos);
        skipWhitespace(expr, pos);
        if (pos < expr.size() && expr[pos] == ':') {
            pos++;
            long long falseVal = evalTernary(expr, pos);
            return cond ? trueVal : falseVal;
        }
    }

    return cond;
}

} // namespace pp
} // namespace cc1
