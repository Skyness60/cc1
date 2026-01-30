#include <parser/Parser.hpp>

namespace cc1 {

// EN: Parses a character literal and decodes escape sequences.
// FR: Parse un litteral caractere et decode les echappements.
AST::Ptr<AST::Expression> Parser::parsePrimaryCharLiteral() {
    Token tok = current();
    advance();

    
    std::string raw = tok.value.substr(1, tok.value.length() - 2);
    int value = 0;
    if (!raw.empty()) {
        if (raw[0] == '\\' && raw.size() > 1) {
            switch (raw[1]) {
                case 'n':
                    value = '\n';
                    break;
                case 't':
                    value = '\t';
                    break;
                case 'r':
                    value = '\r';
                    break;
                case '0':
                    value = '\0';
                    break;
                case '\\':
                    value = '\\';
                    break;
                case '"':
                    value = '"';
                    break;
                case '\'':
                    value = '\'';
                    break;
                case 'a':
                    value = '\a';
                    break;
                case 'b':
                    value = '\b';
                    break;
                case 'f':
                    value = '\f';
                    break;
                case 'v':
                    value = '\v';
                    break;
                case 'x':
                    if (raw.size() > 3) {
                        std::string hex = raw.substr(2, 2);
                        try {
                            value = std::stoi(hex, nullptr, 16);
                        } catch (...) {
                            value = raw[1];
                        }
                    }
                    break;
                default:
                    if (raw[1] >= '0' && raw[1] <= '7') {
                        std::string oct = raw.substr(1);
                        if (oct.size() > 3) oct = oct.substr(0, 3);
                        try {
                            value = std::stoi(oct, nullptr, 8);
                        } catch (...) {
                            value = raw[1];
                        }
                    } else {
                        value = raw[1];
                    }
                    break;
            }
        } else {
            value = static_cast<unsigned char>(raw[0]);
        }
    }

    return AST::make<AST::CharLiteral>(value, tok.value, tok.line, tok.column);
}

} 

// TODO(cc1) EN: Validate multi-character constants per C rules.
// FR: Valider les constantes multi-caracteres selon les regles C.
