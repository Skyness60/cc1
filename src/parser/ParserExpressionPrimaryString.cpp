#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses a string literal and decodes escape sequences.
// FR: Parse un litteral chaine et decode les echappements.
AST::Ptr<AST::Expression> Parser::parsePrimaryStringLiteral() {
    Token tok = current();
    advance();

    
    std::string raw = tok.value.substr(1, tok.value.length() - 2);
    std::string value;
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == '\\' && i + 1 < raw.size()) {
            char next = raw[i + 1];
            ++i;
            switch (next) {
                case 'n':
                    value += '\n';
                    break;
                case 't':
                    value += '\t';
                    break;
                case 'r':
                    value += '\r';
                    break;

                case '\\':
                    value += '\\';
                    break;
                case '"':
                    value += '"';
                    break;
                case '\'':
                    value += '\'';
                    break;
                case 'a':
                    value += '\a';
                    break;
                case 'b':
                    value += '\b';
                    break;
                case 'f':
                    value += '\f';
                    break;
                case 'v':
                    value += '\v';
                    break;
                case 'x': {
                    
                    if (i + 2 < raw.size()) {
                        std::string hex = raw.substr(i + 1, 2);
                        try {
                            value += static_cast<char>(std::stoi(hex, nullptr, 16));
                            i += 2;
                        } catch (...) {
                            value += next; 
                        }
                    } else {
                        value += next;
                    }
                    break;
                }
                default:
                    
                    if (next >= '0' && next <= '7') {
                        std::string oct;
                        oct += next;
                        while (i + 1 < raw.size() && raw[i + 1] >= '0' && raw[i + 1] <= '7' &&
                               oct.size() < 3) {
                            oct += raw[++i];
                        }
                        value += static_cast<char>(std::stoi(oct, nullptr, 8));
                    } else {
                        value += next; 
                    }
                    break;
            }
        } else {
            value += raw[i];
        }
    }

    return AST::make<AST::StringLiteral>(value, tok.value, tok.line, tok.column);
}

} 

// TODO(cc1) EN: Support wide/UTF string literal prefixes in parsing.
// FR: Supporter les prefixes wide/UTF pour les chaines.
