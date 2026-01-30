#include <lexer/Token.hpp>

// EN: Converts a token type to a readable string for diagnostics/logging.
// FR: Convertit un type de token en chaine lisible pour diagnostics/log.
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::Identifier: return "Identifier";
        case TokenType::IntegerLiteral: return "IntegerLiteral";
        case TokenType::FloatLiteral: return "FloatLiteral";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::CharLiteral: return "CharLiteral";
        
        case TokenType::Auto: return "Auto";
        case TokenType::Break: return "Break";
        case TokenType::Case: return "Case";
        case TokenType::Char: return "Char";
        case TokenType::Const: return "Const";
        case TokenType::Continue: return "Continue";
        case TokenType::Default: return "Default";
        case TokenType::Do: return "Do";
        case TokenType::Double: return "Double";
        case TokenType::Else: return "Else";
        case TokenType::Enum: return "Enum";
        case TokenType::Extern: return "Extern";
        case TokenType::Float: return "Float";
        case TokenType::For: return "For";
        case TokenType::Goto: return "Goto";
        case TokenType::If: return "If";
        case TokenType::Int: return "Int";
        case TokenType::Long: return "Long";
        case TokenType::Register: return "Register";
        case TokenType::Return: return "Return";
        case TokenType::Short: return "Short";
        case TokenType::Signed: return "Signed";
        case TokenType::Sizeof: return "Sizeof";
        case TokenType::Static: return "Static";
        case TokenType::Struct: return "Struct";
        case TokenType::Switch: return "Switch";
        case TokenType::Typedef: return "Typedef";
        case TokenType::Union: return "Union";
        case TokenType::Unsigned: return "Unsigned";
        case TokenType::Void: return "Void";
        case TokenType::Volatile: return "Volatile";
        case TokenType::While: return "While";
        
        case TokenType::Plus: return "Plus";
        case TokenType::Minus: return "Minus";
        case TokenType::Star: return "Star";
        case TokenType::Slash: return "Slash";
        case TokenType::Percent: return "Percent";
        case TokenType::PlusPlus: return "PlusPlus";
        case TokenType::MinusMinus: return "MinusMinus";
        case TokenType::Equal: return "Equal";
        case TokenType::EqualEqual: return "EqualEqual";
        case TokenType::NotEqual: return "NotEqual";
        case TokenType::Less: return "Less";
        case TokenType::LessEqual: return "LessEqual";
        case TokenType::Greater: return "Greater";
        case TokenType::GreaterEqual: return "GreaterEqual";
        case TokenType::Ampersand: return "Ampersand";
        case TokenType::Pipe: return "Pipe";
        case TokenType::Caret: return "Caret";
        case TokenType::Tilde: return "Tilde";
        case TokenType::Exclamation: return "Exclamation";
        case TokenType::AmpersandAmpersand: return "AmpersandAmpersand";
        case TokenType::PipePipe: return "PipePipe";
        case TokenType::LeftShift: return "LeftShift";
        case TokenType::RightShift: return "RightShift";
        case TokenType::PlusEqual: return "PlusEqual";
        case TokenType::MinusEqual: return "MinusEqual";
        case TokenType::StarEqual: return "StarEqual";
        case TokenType::SlashEqual: return "SlashEqual";
        case TokenType::PercentEqual: return "PercentEqual";
        case TokenType::AmpersandEqual: return "AmpersandEqual";
        case TokenType::PipeEqual: return "PipeEqual";
        case TokenType::CaretEqual: return "CaretEqual";
        case TokenType::LeftShiftEqual: return "LeftShiftEqual";
        case TokenType::RightShiftEqual: return "RightShiftEqual";
        
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";
        case TokenType::LeftBracket: return "LeftBracket";
        case TokenType::RightBracket: return "RightBracket";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Comma: return "Comma";
        case TokenType::Dot: return "Dot";
        case TokenType::Arrow: return "Arrow";
        case TokenType::Colon: return "Colon";
        case TokenType::Question: return "Question";
        case TokenType::Hash: return "Hash";
        case TokenType::HashHash: return "HashHash";
        case TokenType::Ellipsis: return "Ellipsis";
        default: return "Unknown";
    }
}

// EN: Prints a token in a structured debug-friendly format.
// FR: Affiche un token dans un format de debug structure.
std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(" << tokenTypeToString(token.type) << ", \"" 
       << token.value << "\", " << token.line << ":" << token.column << ")";
    return os;
}

// TODO(cc1) EN: Keep tokenTypeToString in sync automatically with TokenType.
// FR: Synchroniser tokenTypeToString automatiquement avec TokenType.
