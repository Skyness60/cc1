#include <parser/Parser.hpp>

namespace cc1 {





// EN: Maps token types to AST binary operators.
// FR: Mappe les tokens vers operateurs binaires AST.
AST::BinaryOp Parser::tokenToBinaryOp(TokenType type) const {
    switch (type) {
        case TokenType::Plus:
            return AST::BinaryOp::Add;
        case TokenType::Minus:
            return AST::BinaryOp::Sub;
        case TokenType::Star:
            return AST::BinaryOp::Mul;
        case TokenType::Slash:
            return AST::BinaryOp::Div;
        case TokenType::Percent:
            return AST::BinaryOp::Mod;
        case TokenType::EqualEqual:
            return AST::BinaryOp::Equal;
        case TokenType::NotEqual:
            return AST::BinaryOp::NotEqual;
        case TokenType::Less:
            return AST::BinaryOp::Less;
        case TokenType::LessEqual:
            return AST::BinaryOp::LessEqual;
        case TokenType::Greater:
            return AST::BinaryOp::Greater;
        case TokenType::GreaterEqual:
            return AST::BinaryOp::GreaterEqual;
        case TokenType::AmpersandAmpersand:
            return AST::BinaryOp::LogicalAnd;
        case TokenType::PipePipe:
            return AST::BinaryOp::LogicalOr;
        case TokenType::Ampersand:
            return AST::BinaryOp::BitwiseAnd;
        case TokenType::Pipe:
            return AST::BinaryOp::BitwiseOr;
        case TokenType::Caret:
            return AST::BinaryOp::BitwiseXor;
        case TokenType::LeftShift:
            return AST::BinaryOp::LeftShift;
        case TokenType::RightShift:
            return AST::BinaryOp::RightShift;
        case TokenType::Equal:
            return AST::BinaryOp::Assign;
        case TokenType::PlusEqual:
            return AST::BinaryOp::AddAssign;
        case TokenType::MinusEqual:
            return AST::BinaryOp::SubAssign;
        case TokenType::StarEqual:
            return AST::BinaryOp::MulAssign;
        case TokenType::SlashEqual:
            return AST::BinaryOp::DivAssign;
        case TokenType::PercentEqual:
            return AST::BinaryOp::ModAssign;
        case TokenType::AmpersandEqual:
            return AST::BinaryOp::AndAssign;
        case TokenType::PipeEqual:
            return AST::BinaryOp::OrAssign;
        case TokenType::CaretEqual:
            return AST::BinaryOp::XorAssign;
        case TokenType::LeftShiftEqual:
            return AST::BinaryOp::LeftShiftAssign;
        case TokenType::RightShiftEqual:
            return AST::BinaryOp::RightShiftAssign;
        case TokenType::Comma:
            return AST::BinaryOp::Comma;
        default:
            return AST::BinaryOp::Add; 
    }
}

// EN: Maps token types to AST unary operators.
// FR: Mappe les tokens vers operateurs unaires AST.
AST::UnaryOp Parser::tokenToUnaryOp(TokenType type) const {
    switch (type) {
        case TokenType::Plus:
            return AST::UnaryOp::Plus;
        case TokenType::Minus:
            return AST::UnaryOp::Negate;
        case TokenType::Exclamation:
            return AST::UnaryOp::LogicalNot;
        case TokenType::Tilde:
            return AST::UnaryOp::BitwiseNot;
        case TokenType::Ampersand:
            return AST::UnaryOp::AddressOf;
        case TokenType::Star:
            return AST::UnaryOp::Dereference;
        case TokenType::PlusPlus:
            return AST::UnaryOp::PreIncrement;
        case TokenType::MinusMinus:
            return AST::UnaryOp::PreDecrement;
        default:
            return AST::UnaryOp::Plus; 
    }
}

// EN: Maps token types to AST postfix unary operators.
// FR: Mappe les tokens vers operateurs postfixes AST.
AST::UnaryOp Parser::tokenToPostfixOp(TokenType type) const {
    switch (type) {
        case TokenType::PlusPlus:
            return AST::UnaryOp::PostIncrement;
        case TokenType::MinusMinus:
            return AST::UnaryOp::PostDecrement;
        default:
            return AST::UnaryOp::PostIncrement;
    }
}

} 

// TODO(cc1) EN: Handle invalid tokens with explicit errors instead of defaults.
// FR: Gerer les tokens invalides avec erreurs explicites plutot que defaults.
