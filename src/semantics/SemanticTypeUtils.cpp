#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// ============================================================================
// Type Utilities
// ============================================================================

bool SemanticAnalyzer::isVoidType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        return prim->kind == AST::PrimitiveKind::Void;
    }
    return false;
}

bool SemanticAnalyzer::isIntegerType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        switch (prim->kind) {
            case AST::PrimitiveKind::Char:
            case AST::PrimitiveKind::SignedChar:
            case AST::PrimitiveKind::UnsignedChar:
            case AST::PrimitiveKind::Short:
            case AST::PrimitiveKind::UnsignedShort:
            case AST::PrimitiveKind::Int:
            case AST::PrimitiveKind::UnsignedInt:
            case AST::PrimitiveKind::Long:
            case AST::PrimitiveKind::UnsignedLong:
            case AST::PrimitiveKind::LongLong:
            case AST::PrimitiveKind::UnsignedLongLong:
                return true;
            default:
                return false;
        }
    }
    // Enum types are integer types
    if (dynamic_cast<AST::EnumType*>(type)) {
        return true;
    }
    return false;
}

bool SemanticAnalyzer::isFloatType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(type)) {
        switch (prim->kind) {
            case AST::PrimitiveKind::Float:
            case AST::PrimitiveKind::Double:
            case AST::PrimitiveKind::LongDouble:
                return true;
            default:
                return false;
        }
    }
    return false;
}

bool SemanticAnalyzer::isPointerType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    return dynamic_cast<AST::PointerType*>(type) != nullptr;
}

bool SemanticAnalyzer::isArrayType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    return dynamic_cast<AST::ArrayType*>(type) != nullptr;
}

bool SemanticAnalyzer::isStructType(AST::Type* type) {
    if (!type) return false;
    type = stripQualifiers(type);
    return dynamic_cast<AST::StructType*>(type) != nullptr;
}

bool SemanticAnalyzer::isConstQualified(AST::Type* type) {
    if (!type) return false;
    if (auto* qual = dynamic_cast<AST::QualifiedType*>(type)) {
        return qual->isConst;
    }
    return false;
}

bool SemanticAnalyzer::isScalarType(AST::Type* type) {
    return isIntegerType(type) || isFloatType(type) || isPointerType(type);
}

AST::Type* SemanticAnalyzer::stripQualifiers(AST::Type* type) {
    while (auto* qual = dynamic_cast<AST::QualifiedType*>(type)) {
        type = qual->baseType.get();
    }
    return type;
}

std::string SemanticAnalyzer::typeToString(AST::Type* type) {
    if (!type) return "unknown";
    return type->toString();
}

bool SemanticAnalyzer::typesCompatible(AST::Type* left, AST::Type* right) {
    if (!left || !right) return false;
    
    left = stripQualifiers(left);
    right = stripQualifiers(right);
    
    // Same type
    if (left->equals(*right)) return true;
    
    // Integer types are compatible with each other
    if (isIntegerType(left) && isIntegerType(right)) return true;
    
    // Float types are compatible with each other
    if (isFloatType(left) && isFloatType(right)) return true;
    
    // Integer and float can be converted
    if ((isIntegerType(left) && isFloatType(right)) ||
        (isFloatType(left) && isIntegerType(right))) return true;
    
    // Pointer compatibility
    if (isPointerType(left) && isPointerType(right)) {
        auto* ptrLeft = dynamic_cast<AST::PointerType*>(left);
        auto* ptrRight = dynamic_cast<AST::PointerType*>(right);
        
        // void* is compatible with any pointer
        if (isVoidType(ptrLeft->pointee.get()) || isVoidType(ptrRight->pointee.get())) {
            return true;
        }
        
        // Check pointee compatibility (ignoring qualifiers)
        return typesCompatible(ptrLeft->pointee.get(), ptrRight->pointee.get());
    }
    
    // Array decays to pointer
    if (isPointerType(left) && isArrayType(right)) {
        auto* ptr = dynamic_cast<AST::PointerType*>(left);
        auto* arr = dynamic_cast<AST::ArrayType*>(right);
        return typesCompatible(ptr->pointee.get(), arr->elementType.get());
    }
    if (isArrayType(left) && isPointerType(right)) {
        auto* arr = dynamic_cast<AST::ArrayType*>(left);
        auto* ptr = dynamic_cast<AST::PointerType*>(right);
        return typesCompatible(arr->elementType.get(), ptr->pointee.get());
    }
    
    return false;
}

bool SemanticAnalyzer::canAssign(AST::Type* target, AST::Type* source, bool /*isInit*/) {
    if (!target || !source) return false;
    
    AST::Type* targetBase = stripQualifiers(target);
    AST::Type* sourceBase = stripQualifiers(source);
    
    // Can't assign to void
    if (isVoidType(targetBase)) return false;
    
    // Integer to integer - always allowed
    if (isIntegerType(targetBase) && isIntegerType(sourceBase)) return true;
    
    // Float to float - always allowed
    if (isFloatType(targetBase) && isFloatType(sourceBase)) return true;
    
    // Integer to float and vice versa - allowed with implicit conversion
    if ((isIntegerType(targetBase) && isFloatType(sourceBase)) ||
        (isFloatType(targetBase) && isIntegerType(sourceBase))) return true;
    
    // Pointer to pointer
    if (isPointerType(targetBase) && isPointerType(sourceBase)) {
        auto* ptrTarget = dynamic_cast<AST::PointerType*>(targetBase);
        auto* ptrSource = dynamic_cast<AST::PointerType*>(sourceBase);
        
        // void* can be assigned from/to any pointer
        if (isVoidType(ptrTarget->pointee.get()) || isVoidType(ptrSource->pointee.get())) {
            return true;
        }
        
        // Check pointee compatibility
        return typesCompatible(ptrTarget->pointee.get(), ptrSource->pointee.get());
    }
    
    // Array to pointer (decay)
    if (isPointerType(targetBase) && isArrayType(sourceBase)) {
        auto* ptr = dynamic_cast<AST::PointerType*>(targetBase);
        auto* arr = dynamic_cast<AST::ArrayType*>(sourceBase);
        return typesCompatible(ptr->pointee.get(), arr->elementType.get());
    }
    
    // Struct assignment - same struct types
    if (isStructType(targetBase) && isStructType(sourceBase)) {
        return targetBase->equals(*sourceBase);
    }
    
    return false;
}

AST::Ptr<AST::Type> SemanticAnalyzer::getExprType(AST::Expression* expr) {
    if (!expr) return nullptr;
    
    // Check cache first
    auto it = exprTypes_.find(expr);
    if (it != exprTypes_.end()) {
        return it->second->clone();
    }
    
    // Compute type on-the-fly for common cases
    if (auto* lit = dynamic_cast<AST::IntegerLiteral*>(expr)) {
        return AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, lit->line, lit->column);
    }
    
    if (auto* lit = dynamic_cast<AST::CharLiteral*>(expr)) {
        return AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Char, lit->line, lit->column);
    }
    
    if (auto* lit = dynamic_cast<AST::StringLiteral*>(expr)) {
        auto charType = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Char, lit->line, lit->column);
        auto size = AST::make<AST::IntegerLiteral>(static_cast<long long>(lit->value.length() + 1), "", lit->line, lit->column);
        return AST::make<AST::ArrayType>(std::move(charType), std::move(size), lit->line, lit->column);
    }
    
    if (auto* id = dynamic_cast<AST::Identifier*>(expr)) {
        Symbol* sym = currentScope_->lookup(id->name);
        if (sym && sym->type) {
            return sym->type->clone();
        }
    }
    
    if (auto* index = dynamic_cast<AST::IndexExpr*>(expr)) {
        AST::Ptr<AST::Type> arrayType = getExprType(index->array.get());
        if (arrayType) {
            if (auto* arr = dynamic_cast<AST::ArrayType*>(arrayType.get())) {
                if (arr->elementType) {
                    return arr->elementType->clone();
                }
            }
            if (auto* ptr = dynamic_cast<AST::PointerType*>(arrayType.get())) {
                if (ptr->pointee) {
                    return ptr->pointee->clone();
                }
            }
        }
    }
    
    if (auto* unary = dynamic_cast<AST::UnaryExpr*>(expr)) {
        if (unary->op == AST::UnaryOp::Dereference) {
            AST::Ptr<AST::Type> operandType = getExprType(unary->operand.get());
            if (operandType) {
                if (auto* ptr = dynamic_cast<AST::PointerType*>(operandType.get())) {
                    if (ptr->pointee) return ptr->pointee->clone();
                }
                if (auto* arr = dynamic_cast<AST::ArrayType*>(operandType.get())) {
                    if (arr->elementType) return arr->elementType->clone();
                }
            }
        } else if (unary->op == AST::UnaryOp::AddressOf) {
            AST::Ptr<AST::Type> operandType = getExprType(unary->operand.get());
            if (operandType) {
                return AST::make<AST::PointerType>(std::move(operandType), expr->line, expr->column);
            }
        } else {
            // For other unary ops, return operand type
            return getExprType(unary->operand.get());
        }
    }
    
    if (dynamic_cast<AST::SizeofExpr*>(expr)) {
        // sizeof returns size_t, which we treat as unsigned long
        return AST::make<AST::PrimitiveType>(AST::PrimitiveKind::UnsignedLong, expr->line, expr->column);
    }
    
    return nullptr;
}

} // namespace cc1
