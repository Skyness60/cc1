#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// ============================================================================
// Constant Expression Evaluation
// ============================================================================

bool SemanticAnalyzer::isConstantExpr(AST::Expression* expr) {
    if (!expr) return false;
    
    if (dynamic_cast<AST::IntegerLiteral*>(expr)) return true;
    if (dynamic_cast<AST::CharLiteral*>(expr)) return true;
    if (dynamic_cast<AST::FloatLiteral*>(expr)) return true;
    
    // Enum constants are constant
    if (auto* id = dynamic_cast<AST::Identifier*>(expr)) {
        Symbol* sym = currentScope_->lookup(id->name);
        if (sym && sym->isEnumConstant) return true;
    }
    
    // sizeof is constant
    if (dynamic_cast<AST::SizeofExpr*>(expr)) return true;
    
    // Binary expression with constant operands
    if (auto* bin = dynamic_cast<AST::BinaryExpr*>(expr)) {
        return isConstantExpr(bin->left.get()) && isConstantExpr(bin->right.get());
    }
    
    // Unary expression with constant operand
    if (auto* unary = dynamic_cast<AST::UnaryExpr*>(expr)) {
        return isConstantExpr(unary->operand.get());
    }
    
    // Ternary expression
    if (auto* tern = dynamic_cast<AST::TernaryExpr*>(expr)) {
        return isConstantExpr(tern->condition.get()) &&
               isConstantExpr(tern->thenExpr.get()) &&
               isConstantExpr(tern->elseExpr.get());
    }
    
    return false;
}

bool SemanticAnalyzer::evaluateConstantExpr(AST::Expression* expr, long long& result) {
    if (!expr) return false;
    
    if (auto* lit = dynamic_cast<AST::IntegerLiteral*>(expr)) {
        result = lit->value;
        return true;
    }
    
    if (auto* lit = dynamic_cast<AST::CharLiteral*>(expr)) {
        result = lit->value;
        return true;
    }
    
    // Handle sizeof expression
    if (auto* sizeofExpr = dynamic_cast<AST::SizeofExpr*>(expr)) {
        // For simplicity, use common sizes
        if (sizeofExpr->targetType) {
            AST::Type* type = sizeofExpr->targetType.get();
            if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
                switch (prim->kind) {
                    case AST::PrimitiveKind::Char:
                    case AST::PrimitiveKind::SignedChar:
                    case AST::PrimitiveKind::UnsignedChar:
                        result = 1; return true;
                    case AST::PrimitiveKind::Short:
                    case AST::PrimitiveKind::UnsignedShort:
                        result = 2; return true;
                    case AST::PrimitiveKind::Int:
                    case AST::PrimitiveKind::UnsignedInt:
                        result = 4; return true;
                    case AST::PrimitiveKind::Long:
                    case AST::PrimitiveKind::UnsignedLong:
                        result = 8; return true;
                    case AST::PrimitiveKind::LongLong:
                    case AST::PrimitiveKind::UnsignedLongLong:
                        result = 8; return true;
                    case AST::PrimitiveKind::Float:
                        result = 4; return true;
                    case AST::PrimitiveKind::Double:
                        result = 8; return true;
                    case AST::PrimitiveKind::LongDouble:
                        result = 16; return true;
                    default:
                        return false;
                }
            }
            if (dynamic_cast<AST::PointerType*>(type)) {
                result = 8; // 64-bit pointers
                return true;
            }
            if (auto* arr = dynamic_cast<AST::ArrayType*>(type)) {
                long long elemSize, arraySize;
                if (arr->elementType) {
                    AST::SizeofExpr elemSizeof(arr->elementType->clone(), 0, 0);
                    if (!evaluateConstantExpr(&elemSizeof, elemSize)) return false;
                    
                    // First check computed size field
                    if (arr->size >= 0) {
                        result = elemSize * arr->size;
                        return true;
                    }
                    
                    // Then check sizeExpr
                    if (arr->sizeExpr) {
                        if (!evaluateConstantExpr(arr->sizeExpr.get(), arraySize)) return false;
                        result = elemSize * arraySize;
                        return true;
                    }
                }
                return false;
            }
            if (auto* structType = dynamic_cast<AST::StructType*>(type)) {
                // If struct has no members, try to look up the definition
                if (structType->members.empty() && !structType->name.empty()) {
                    // Look up the struct definition from tag namespace
                    Symbol* tagSym = currentScope_->lookupTag(structType->name);
                    if (tagSym && tagSym->structDecl) {
                        // Calculate struct size from declaration members
                        long long totalSize = 0;
                        for (const auto& memberDecl : tagSym->structDecl->members) {
                            if (memberDecl && memberDecl->type) {
                                AST::SizeofExpr memberSizeof(memberDecl->type->clone(), 0, 0);
                                long long memberSize;
                                if (!evaluateConstantExpr(&memberSizeof, memberSize)) return false;
                                totalSize += memberSize;
                            }
                        }
                        result = totalSize > 0 ? totalSize : 1;
                        return true;
                    }
                }
                
                // Calculate struct size (simplified - no padding)
                long long totalSize = 0;
                for (const auto& member : structType->members) {
                    if (member.type) {
                        AST::SizeofExpr memberSizeof(member.type->clone(), 0, 0);
                        long long memberSize;
                        if (!evaluateConstantExpr(&memberSizeof, memberSize)) return false;
                        totalSize += memberSize;
                    }
                }
                result = totalSize > 0 ? totalSize : 1;
                return true;
            }
        }
        // sizeof with expression operand
        if (sizeofExpr->operand) {
            AST::Ptr<AST::Type> exprType = getExprType(sizeofExpr->operand.get());
            if (exprType) {
                AST::SizeofExpr typeSizeof(std::move(exprType), 0, 0);
                return evaluateConstantExpr(&typeSizeof, result);
            }
        }
        return false;
    }
    
    // Handle identifier - could be enum constant
    if (auto* id = dynamic_cast<AST::Identifier*>(expr)) {
        Symbol* sym = currentScope_->lookup(id->name);
        if (sym && sym->isEnumConstant) {
            result = sym->enumValue;
            return true;
        }
        return false;
    }
    
    // Handle unary dereference for sizeof(*arr)
    if (auto* unary = dynamic_cast<AST::UnaryExpr*>(expr)) {
        if (unary->op == AST::UnaryOp::Dereference) {
            // sizeof(*arr) - get the element type
            AST::Ptr<AST::Type> operandType = getExprType(unary->operand.get());
            if (operandType) {
                if (auto* ptr = dynamic_cast<AST::PointerType*>(operandType.get())) {
                    if (ptr->pointee) {
                        AST::SizeofExpr elemSizeof(ptr->pointee->clone(), 0, 0);
                        return evaluateConstantExpr(&elemSizeof, result);
                    }
                }
                if (auto* arr = dynamic_cast<AST::ArrayType*>(operandType.get())) {
                    if (arr->elementType) {
                        AST::SizeofExpr elemSizeof(arr->elementType->clone(), 0, 0);
                        return evaluateConstantExpr(&elemSizeof, result);
                    }
                }
            }
        }
        long long operand;
        if (!evaluateConstantExpr(unary->operand.get(), operand)) return false;
        
        switch (unary->op) {
            case AST::UnaryOp::Plus: result = operand; return true;
            case AST::UnaryOp::Negate: result = -operand; return true;
            case AST::UnaryOp::BitwiseNot: result = ~operand; return true;
            case AST::UnaryOp::LogicalNot: result = !operand ? 1 : 0; return true;
            default: return false;
        }
    }
    
    if (auto* bin = dynamic_cast<AST::BinaryExpr*>(expr)) {
        long long left, right;
        if (!evaluateConstantExpr(bin->left.get(), left)) return false;
        if (!evaluateConstantExpr(bin->right.get(), right)) return false;
        
        switch (bin->op) {
            case AST::BinaryOp::Add: result = left + right; return true;
            case AST::BinaryOp::Sub: result = left - right; return true;
            case AST::BinaryOp::Mul: result = left * right; return true;
            case AST::BinaryOp::Div:
                if (right == 0) return false;  // Division by zero
                result = left / right;
                return true;
            case AST::BinaryOp::Mod:
                if (right == 0) return false;
                result = left % right;
                return true;
            case AST::BinaryOp::Equal: result = left == right ? 1 : 0; return true;
            case AST::BinaryOp::NotEqual: result = left != right ? 1 : 0; return true;
            case AST::BinaryOp::Less: result = left < right ? 1 : 0; return true;
            case AST::BinaryOp::LessEqual: result = left <= right ? 1 : 0; return true;
            case AST::BinaryOp::Greater: result = left > right ? 1 : 0; return true;
            case AST::BinaryOp::GreaterEqual: result = left >= right ? 1 : 0; return true;
            case AST::BinaryOp::LogicalAnd: result = (left && right) ? 1 : 0; return true;
            case AST::BinaryOp::LogicalOr: result = (left || right) ? 1 : 0; return true;
            case AST::BinaryOp::BitwiseAnd: result = left & right; return true;
            case AST::BinaryOp::BitwiseOr: result = left | right; return true;
            case AST::BinaryOp::BitwiseXor: result = left ^ right; return true;
            case AST::BinaryOp::LeftShift: result = left << right; return true;
            case AST::BinaryOp::RightShift: result = left >> right; return true;
            default: return false;
        }
    }
    
    if (auto* tern = dynamic_cast<AST::TernaryExpr*>(expr)) {
        long long cond;
        if (!evaluateConstantExpr(tern->condition.get(), cond)) return false;
        if (cond) {
            return evaluateConstantExpr(tern->thenExpr.get(), result);
        } else {
            return evaluateConstantExpr(tern->elseExpr.get(), result);
        }
    }
    
    // Handle cast expression - evaluate the inner expression
    if (auto* cast = dynamic_cast<AST::CastExpr*>(expr)) {
        return evaluateConstantExpr(cast->operand.get(), result);
    }
    
    return false;
}

bool SemanticAnalyzer::hasDivisionByZero(AST::Expression* expr) {
    if (!expr) return false;
    
    if (auto* bin = dynamic_cast<AST::BinaryExpr*>(expr)) {
        if (bin->op == AST::BinaryOp::Div || bin->op == AST::BinaryOp::Mod) {
            long long rightVal;
            if (evaluateConstantExpr(bin->right.get(), rightVal) && rightVal == 0) {
                return true;
            }
        }
        return hasDivisionByZero(bin->left.get()) || hasDivisionByZero(bin->right.get());
    }
    
    if (auto* unary = dynamic_cast<AST::UnaryExpr*>(expr)) {
        return hasDivisionByZero(unary->operand.get());
    }
    
    if (auto* tern = dynamic_cast<AST::TernaryExpr*>(expr)) {
        // For ternary, check if condition is constant
        long long cond;
        if (evaluateConstantExpr(tern->condition.get(), cond)) {
            // Only check the branch that will be taken
            if (cond) {
                return hasDivisionByZero(tern->thenExpr.get());
            } else {
                return hasDivisionByZero(tern->elseExpr.get());
            }
        }
        // If condition is not constant, check all branches
        return hasDivisionByZero(tern->condition.get()) ||
               hasDivisionByZero(tern->thenExpr.get()) ||
               hasDivisionByZero(tern->elseExpr.get());
    }
    
    return false;
}

// ============================================================================
// Enum Processing
// ============================================================================

void SemanticAnalyzer::processEnumType(AST::EnumType* enumType) {
    if (!enumType) return;
    
    // Register enum tag if named
    if (!enumType->name.empty()) {
        Symbol* existingTag = currentScope_->lookupTag(enumType->name);
        if (existingTag && existingTag->tagKind != Symbol::TagKind::Enum) {
            error(enumType->line, 1,  // Column 1 points to 'enum' keyword like clang
                  "use of '" + enumType->name + "' with tag type that does not match previous declaration");
            return;
        }
        
        Symbol sym;
        sym.name = enumType->name;
        sym.tagKind = Symbol::TagKind::Enum;
        sym.line = enumType->line;
        sym.column = enumType->column;
        currentScope_->defineTag(enumType->name, sym);
    }
    
    // Process enumerators - check for division by zero (bad_semantic_7.c)
    long long nextValue = 0;
    for (const auto& enumerator : enumType->enumerators) {
        if (enumerator.value) {
            // Check for division by zero in the value expression
            if (hasDivisionByZero(enumerator.value.get())) {
                // Point to the expression like clang does
                int line = enumerator.value->line;
                int col = enumerator.value->column;
                error(line, col, "expression is not an integer constant expression");
                return;
            }
            
            // Evaluate the constant expression
            if (!evaluateConstantExpr(enumerator.value.get(), nextValue)) {
                // Point to the expression like clang does  
                int line = enumerator.value->line;
                int col = enumerator.value->column;
                error(line, col, "expression is not an integer constant expression");
                return;
            }
        }
        
        // Register enumerator as constant with its value
        Symbol sym;
        sym.name = enumerator.name;
        sym.isEnumConstant = true;
        sym.enumValue = nextValue;
        sym.line = enumType->line;
        sym.column = enumType->column;
        currentScope_->define(enumerator.name, sym, AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, 0, 0));
        
        nextValue++;
    }
}

} // namespace cc1
