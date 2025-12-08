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
    while (pos < expr.size() && std::isspace(expr[pos])) {
        pos++;
    }
}

bool ExpressionEvaluator::isIdentifierStart(char c) const {
    return std::isalpha(c) || c == '_';
}

bool ExpressionEvaluator::isIdentifierChar(char c) const {
    return std::isalnum(c) || c == '_';
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
        } else if (std::isdigit(str[pos + 1])) {
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
    
    // Skip suffix
    while (pos < str.size() && (str[pos] == 'u' || str[pos] == 'U' || 
                                 str[pos] == 'l' || str[pos] == 'L')) {
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

long long ExpressionEvaluator::evalBitwiseOr(const std::string& expr, size_t& pos) {
    long long left = evalBitwiseXor(expr, pos);
    
    while (true) {
        skipWhitespace(expr, pos);
        if (pos < expr.size() && expr[pos] == '|' && (pos + 1 >= expr.size() || expr[pos + 1] != '|')) {
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
        if (pos < expr.size() && expr[pos] == '&' && (pos + 1 >= expr.size() || expr[pos + 1] != '&')) {
            pos++;
            long long right = evalEquality(expr, pos);
            left = left & right;
        } else {
            break;
        }
    }
    
    return left;
}

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
        } else if (pos < expr.size() && expr[pos] == '<' && (pos + 1 >= expr.size() || expr[pos + 1] != '<')) {
            pos++;
            long long right = evalShift(expr, pos);
            left = (left < right) ? 1 : 0;
        } else if (pos < expr.size() && expr[pos] == '>' && (pos + 1 >= expr.size() || expr[pos + 1] != '>')) {
            pos++;
            long long right = evalShift(expr, pos);
            left = (left > right) ? 1 : 0;
        } else {
            break;
        }
    }
    
    return left;
}

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

long long ExpressionEvaluator::evalMultiplicative(const std::string& expr, size_t& pos) {
    long long left = evalUnary(expr, pos);
    
    while (true) {
        skipWhitespace(expr, pos);
        if (pos < expr.size() && expr[pos] == '*') {
            pos++;
            long long right = evalUnary(expr, pos);
            left = left * right;
        } else if (pos < expr.size() && expr[pos] == '/') {
            pos++;
            long long right = evalUnary(expr, pos);
            if (right != 0) {
                left = left / right;
            } else {
                error("division by zero");
                left = 0;
            }
        } else if (pos < expr.size() && expr[pos] == '%') {
            pos++;
            long long right = evalUnary(expr, pos);
            if (right != 0) {
                left = left % right;
            } else {
                error("division by zero");
                left = 0;
            }
        } else {
            break;
        }
    }
    
    return left;
}

long long ExpressionEvaluator::evalUnary(const std::string& expr, size_t& pos) {
    skipWhitespace(expr, pos);
    
    if (pos < expr.size()) {
        if (expr[pos] == '!') {
            pos++;
            return evalUnary(expr, pos) ? 0 : 1;
        }
        if (expr[pos] == '~') {
            pos++;
            return ~evalUnary(expr, pos);
        }
        if (expr[pos] == '-') {
            pos++;
            return -evalUnary(expr, pos);
        }
        if (expr[pos] == '+') {
            pos++;
            return evalUnary(expr, pos);
        }
    }
    
    return evalPrimary(expr, pos);
}

long long ExpressionEvaluator::evalPrimary(const std::string& expr, size_t& pos) {
    skipWhitespace(expr, pos);
    
    if (pos >= expr.size()) {
        return 0;
    }
    
    // Parenthesized expression
    if (expr[pos] == '(') {
        pos++;
        long long val = evalTernary(expr, pos);
        skipWhitespace(expr, pos);
        if (pos < expr.size() && expr[pos] == ')') {
            pos++;
        }
        return val;
    }
    
    // Character literal
    if (expr[pos] == '\'') {
        pos++;
        long long val = 0;
        while (pos < expr.size() && expr[pos] != '\'') {
            if (expr[pos] == '\\' && pos + 1 < expr.size()) {
                pos++;
                switch (expr[pos]) {
                    case 'n': val = '\n'; break;
                    case 't': val = '\t'; break;
                    case 'r': val = '\r'; break;
                    case '0': val = '\0'; break;
                    case '\\': val = '\\'; break;
                    case '\'': val = '\''; break;
                    default: val = expr[pos]; break;
                }
                pos++;
            } else {
                val = static_cast<unsigned char>(expr[pos++]);
            }
        }
        if (pos < expr.size()) pos++;
        return val;
    }
    
    // Number
    if (std::isdigit(expr[pos])) {
        return parseNumber(expr, pos);
    }
    
    // Identifier (undefined identifiers become 0)
    if (isIdentifierStart(expr[pos])) {
        std::string ident = parseIdentifier(expr, pos);
        return 0;
    }
    
    return 0;
}

} // namespace pp
} // namespace cc1
