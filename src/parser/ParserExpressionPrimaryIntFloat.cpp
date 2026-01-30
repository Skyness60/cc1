#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses an integer literal and validates its suffix.
// FR: Parse un litteral entier et valide son suffixe.
AST::Ptr<AST::Expression> Parser::parsePrimaryIntegerLiteral() {
    Token tok = current();
    advance();

    unsigned long long parsed = 0;
    size_t idx = 0;
    try {
        
        parsed = std::stoull(tok.value, &idx, 0);
    } catch (...) {
        errorAt(tok, "invalid integer constant");
    }

    
    
    if (idx < tok.value.size()) {
        std::string suf = tok.value.substr(idx);
        for (char& ch : suf) {
            if (ch >= 'A' && ch <= 'Z') ch = static_cast<char>(ch - 'A' + 'a');
        }

        auto isValidSuffix = [](const std::string& s) -> bool {
            if (s.empty()) return true;

            auto parseOne = [&](size_t& p, bool& sawU, int& longCount) -> bool {
                while (p < s.size()) {
                    if (s[p] == 'u') {
                        if (sawU) return false;
                        sawU = true;
                        ++p;
                        continue;
                    }
                    if (s[p] == 'l') {
                        if (p + 1 < s.size() && s[p + 1] == 'l') {
                            if (longCount != 0) return false;
                            longCount = 2;
                            p += 2;
                        } else {
                            if (longCount != 0) return false;
                            longCount = 1;
                            ++p;
                        }
                        continue;
                    }
                    return false;
                }
                return true;
            };

            {
                size_t p = 0;
                bool sawU = false;
                int longCount = 0;
                if (parseOne(p, sawU, longCount)) return true;
            }

            return false;
        };

        if (!isValidSuffix(suf)) {
            errorAt(tok, "invalid integer constant suffix");
        }
    }

    long long value = static_cast<long long>(parsed);
    return AST::make<AST::IntegerLiteral>(value, tok.value, tok.line, tok.column);
}

// EN: Parses a float literal using std::stod.
// FR: Parse un litteral flottant avec std::stod.
AST::Ptr<AST::Expression> Parser::parsePrimaryFloatLiteral() {
    Token tok = current();
    advance();
    double value = std::stod(tok.value);
    return AST::make<AST::FloatLiteral>(value, tok.value, tok.line, tok.column);
}

} 

// TODO(cc1) EN: Add diagnostics for float suffixes and hex floats.
// FR: Ajouter des diagnostics pour suffixes flottants et hex floats.
