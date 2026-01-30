#include <preprocessor/PPExprEval.hpp>

#include <cctype>

namespace cc1 {
namespace pp {

// EN: Evaluates *, /, % with division-by-zero checks.
// FR: Evalue *, /, % avec verif division par zero.
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

// EN: Evaluates unary operators (!, ~, +, -).
// FR: Evalue les operateurs unaires (!, ~, +, -).
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

// EN: Evaluates primary expressions: parentheses, char literals, numbers, ids.
// FR: Evalue les primaires : parentheses, char, nombres, identifiants.
long long ExpressionEvaluator::evalPrimary(const std::string& expr, size_t& pos) {
    skipWhitespace(expr, pos);

    if (pos >= expr.size()) {
        return 0;
    }

    if (expr[pos] == '(') {
        pos++;
        long long val = evalTernary(expr, pos);
        skipWhitespace(expr, pos);
        if (pos < expr.size() && expr[pos] == ')') {
            pos++;
        }
        return val;
    }

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
        if (pos < expr.size()) {
            pos++;
        }
        return val;
    }

    if (std::isdigit(static_cast<unsigned char>(expr[pos]))) {
        return parseNumber(expr, pos);
    }

    if (isIdentifierStart(expr[pos])) {
        std::string ident = parseIdentifier(expr, pos);
        static_cast<void>(ident);
        return 0;
    }

    return 0;
}

} 
} 

// TODO(cc1) EN: Expand identifier handling to support defined macros in #if.
// FR: Etendre la gestion des identifiants pour supporter macros dans #if.
