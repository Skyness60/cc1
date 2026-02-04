#include <parser/Parser.hpp>
#include <utils/Diagnostic.hpp>

namespace cc1 {

// EN: Parses a character literal and decodes escape sequences.
// FR: Parse un litteral caractere et decode les echappements.
AST::Ptr<AST::Expression> Parser::parsePrimaryCharLiteral() {
    Token tok = current();
    advance();

    // If the lexer provides an invalid character literal (empty or multi-char),
    // emulate clang's follow-up parser diagnostics:
    //   - error: expected expression                at the opening quote
    //   - error: expected ';' after top level ...   at the end of the token, with insertion hint
    //
    // Note: The lexer may still have already emitted warnings (e.g. empty constant,
    // missing terminating quote). Here we only mirror the parser-side errors.
    //
    // Important: handle common escape sequences so valid literals like '\x2A' do
    // not trigger these errors.
    std::string raw = (tok.value.size() >= 2) ? tok.value.substr(1, tok.value.length() - 2) : "";

    auto isValidSingleCharLiteral = [](const std::string& r) -> bool {
        // Exactly one non-escape character, e.g. 'a'
        if (r.size() == 1) return true;

        // Escape sequence must start with backslash.
        if (r.size() < 2 || r[0] != '\\') return false;

        // Simple one-char escapes: '\n', '\t', '\'', '\\', etc.
        switch (r[1]) {
            case 'n': case 't': case 'r': case '0':
            case '\\': case '"': case '\'':
            case 'a': case 'b': case 'f': case 'v':
                return r.size() == 2;

            case 'x': {
                // Hex escape: '\x' followed by 1+ hex digits.
                if (r.size() < 3) return false;
                for (size_t i = 2; i < r.size(); ++i) {
                    char c = r[i];
                    bool isHex =
                        (c >= '0' && c <= '9') ||
                        (c >= 'a' && c <= 'f') ||
                        (c >= 'A' && c <= 'F');
                    if (!isHex) return false;
                }
                return true;
            }

            default: {
                // Octal escape: '\' followed by 1..3 octal digits.
                if (r[1] < '0' || r[1] > '7') return false;
                if (r.size() > 4) return false; // backslash + up to 3 digits
                for (size_t i = 1; i < r.size(); ++i) {
                    if (r[i] < '0' || r[i] > '7') return false;
                }
                return true;
            }
        }
    };

    if (!isValidSingleCharLiteral(raw)) {
        std::string line = getSourceLine(tok.line);

        // 1) expected expression at the opening quote.
        //
        // IMPORTANT: Do not print it here, because we throw a ParseError with the
        // same message/location and the driver prints e.what(). Printing both
        // would duplicate the diagnostic.
        SourceLocation locExpr(filename_, tok.line, tok.column);

        // 2) expected ';' after top level declarator at the end of the token.
        // For `'''`, tok.column == 10 and tok.value.size() == 3, clang points at col 14.
        //
        // Clang's caret is positioned one past the last character of the malformed
        // token stream as printed on the line. With 1-based columns, that's:
        //   endCol = startCol + printed_length_of_token_stream
        int endCol = tok.column + static_cast<int>(tok.value.size()) + 2;
        SourceLocation locSemi(filename_, tok.line, endCol);
        DiagnosticPrinter::print(DiagnosticKind::Error, locSemi, "expected ';' after top level declarator", line);

        // Stop parsing: the driver will treat this as a hard error and fcc should not link.
        throw ParseError("expected expression", locExpr, line);
    }

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
