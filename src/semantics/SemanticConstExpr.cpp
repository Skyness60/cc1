#include <semantics/SemanticAnalyzer.hpp>

#include <cstdint>
#include <functional>

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

    auto maskUnsigned = [](uint64_t value, int bits) -> uint64_t {
        if (bits >= 64) return value;
        if (bits <= 0) return 0;
        return value & ((1ULL << bits) - 1ULL);
    };

    auto widthBitsForPrim = [this](AST::PrimitiveKind kind) -> int {
        switch (kind) {
            case AST::PrimitiveKind::Char:
            case AST::PrimitiveKind::SignedChar:
            case AST::PrimitiveKind::UnsignedChar:
                return 8;
            case AST::PrimitiveKind::Short:
            case AST::PrimitiveKind::UnsignedShort:
                return 16;
            case AST::PrimitiveKind::Int:
            case AST::PrimitiveKind::UnsignedInt:
                return 32;
            case AST::PrimitiveKind::Long:
            case AST::PrimitiveKind::UnsignedLong:
                return is64bit_ ? 64 : 32;
            case AST::PrimitiveKind::LongLong:
            case AST::PrimitiveKind::UnsignedLongLong:
                return 64;
            default:
                return 32;
        }
    };

    std::function<int(AST::Expression*)> inferIntWidthBits;
    inferIntWidthBits = [&](AST::Expression* e) -> int {
        if (!e) return 32;
        if (auto* cast = dynamic_cast<AST::CastExpr*>(e)) {
            if (auto* prim = dynamic_cast<AST::PrimitiveType*>(cast->targetType.get())) {
                return widthBitsForPrim(prim->kind);
            }
        }
        if (auto* unary = dynamic_cast<AST::UnaryExpr*>(e)) {
            if (unary->op == AST::UnaryOp::BitwiseNot) {
                return inferIntWidthBits(unary->operand.get());
            }
        }
        return 32;
    };
    
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
        auto alignTo = [](long long offset, int alignment) -> long long {
            if (alignment <= 1) return offset;
            long long a = static_cast<long long>(alignment);
            return (offset + (a - 1)) & ~(a - 1);
        };

        std::function<long long(AST::Type*)> typeSize;
        std::function<int(AST::Type*)> typeAlign;

        typeAlign = [&](AST::Type* t) -> int {
            if (!t) return 1;

            // Strip qualifiers
            while (auto* qual = dynamic_cast<AST::QualifiedType*>(t)) {
                t = qual->baseType.get();
            }

            if (auto* td = dynamic_cast<AST::TypedefType*>(t)) {
                if (td->underlyingType) return typeAlign(td->underlyingType.get());
                return 4;
            }

            if (dynamic_cast<AST::PrimitiveType*>(t)) {
                long long sz = typeSize(t);
                int ptrSize = is64bit_ ? 8 : 4;
                return static_cast<int>(std::min<long long>(sz, ptrSize));
            }

            if (dynamic_cast<AST::PointerType*>(t)) {
                return is64bit_ ? 8 : 4;
            }

            if (auto* arr = dynamic_cast<AST::ArrayType*>(t)) {
                return typeAlign(arr->elementType.get());
            }

            if (auto* st = dynamic_cast<AST::StructType*>(t)) {
                if (st->isUnion) {
                    int maxA = 1;
                    for (const auto& m : st->members) {
                        maxA = std::max(maxA, typeAlign(m.type.get()));
                    }
                    return maxA;
                }

                int maxA = 1;
                size_t i = 0;
                while (i < st->members.size()) {
                    const auto& m = st->members[i];
                    int a = typeAlign(m.type.get());
                    maxA = std::max(maxA, a);
                    if (m.isBitfield()) {
                        size_t j = i + 1;
                        while (j < st->members.size() && st->members[j].isBitfield()) j++;
                        i = j;
                        continue;
                    }
                    i++;
                }
                return maxA;
            }

            if (dynamic_cast<AST::EnumType*>(t)) {
                return 4;
            }

            // Typedefs are resolved by the semantic analyzer elsewhere; fall back to int.
            return 4;
        };

        typeSize = [&](AST::Type* t) -> long long {
            if (!t) return 0;

            // Strip qualifiers
            while (auto* qual = dynamic_cast<AST::QualifiedType*>(t)) {
                t = qual->baseType.get();
            }

            if (auto* td = dynamic_cast<AST::TypedefType*>(t)) {
                if (td->underlyingType) return typeSize(td->underlyingType.get());
                return 4;
            }

            if (auto* prim = dynamic_cast<AST::PrimitiveType*>(t)) {
                switch (prim->kind) {
                    case AST::PrimitiveKind::Char:
                    case AST::PrimitiveKind::SignedChar:
                    case AST::PrimitiveKind::UnsignedChar:
                        return 1;
                    case AST::PrimitiveKind::Short:
                    case AST::PrimitiveKind::UnsignedShort:
                        return 2;
                    case AST::PrimitiveKind::Int:
                    case AST::PrimitiveKind::UnsignedInt:
                        return 4;
                    case AST::PrimitiveKind::Long:
                    case AST::PrimitiveKind::UnsignedLong:
                        return is64bit_ ? 8 : 4;
                    case AST::PrimitiveKind::LongLong:
                    case AST::PrimitiveKind::UnsignedLongLong:
                        return 8;
                    case AST::PrimitiveKind::Float:
                        return 4;
                    case AST::PrimitiveKind::Double:
                        return 8;
                    case AST::PrimitiveKind::LongDouble:
                        // Keep consistent with existing assumptions in this project.
                        return 16;
                    case AST::PrimitiveKind::Void:
                        return 1;
                }
                return 4;
            }

            if (dynamic_cast<AST::PointerType*>(t)) {
                return is64bit_ ? 8 : 4;
            }

            if (auto* arr = dynamic_cast<AST::ArrayType*>(t)) {
                long long elem = typeSize(arr->elementType.get());
                long long n = arr->size;
                if (n < 0 && arr->sizeExpr) {
                    long long evaluated;
                    if (!evaluateConstantExpr(arr->sizeExpr.get(), evaluated)) return 0;
                    n = evaluated;
                }
                if (n < 0) return 0;
                return elem * n;
            }

            if (auto* st = dynamic_cast<AST::StructType*>(t)) {
                if (st->isUnion) {
                    long long maxSize = 0;
                    int maxAlign = 1;
                    for (const auto& m : st->members) {
                        int ma = typeAlign(m.type.get());
                        long long ms = 0;
                        if (m.isBitfield()) {
                            // In a union, each bitfield member occupies at least one storage unit.
                            // Zero-width bitfields contribute alignment but no size.
                            ms = (m.bitWidth == 0) ? 0 : typeSize(m.type.get());
                        } else {
                            ms = typeSize(m.type.get());
                        }
                        maxSize = std::max(maxSize, ms);
                        maxAlign = std::max(maxAlign, ma);
                    }
                    long long desired = alignTo(std::max(1LL, maxSize), maxAlign);
                    return desired;
                }

                long long offset = 0;
                int maxAlign = 1;
                // Bitfield packing: model allocation into storage units of the declared base type.
                // - Bitfields do not cross a storage-unit boundary.
                // - A zero-width bitfield forces alignment to the next storage unit of its base type.
                long long currentUnitSizeBytes = 0;
                int currentUnitAlign = 1;
                int currentUnitBitsTotal = 0;
                int currentUnitBitsUsed = 0;

                auto flushUnit = [&]() {
                    if (currentUnitBitsUsed > 0) {
                        offset += currentUnitSizeBytes;
                    }
                    currentUnitSizeBytes = 0;
                    currentUnitAlign = 1;
                    currentUnitBitsTotal = 0;
                    currentUnitBitsUsed = 0;
                };

                for (const auto& m : st->members) {
                    if (!m.isBitfield()) {
                        flushUnit();
                        int a = typeAlign(m.type.get());
                        long long s = typeSize(m.type.get());
                        offset = alignTo(offset, a);
                        offset += s;
                        maxAlign = std::max(maxAlign, a);
                        continue;
                    }

                    long long unitSizeBytes = typeSize(m.type.get());
                    int unitAlign = typeAlign(m.type.get());
                    int unitBits = static_cast<int>(unitSizeBytes * 8);

                    maxAlign = std::max(maxAlign, unitAlign);

                    // Defensive: invalid/incomplete base types.
                    if (unitBits <= 0) return 0;

                    if (m.bitWidth == 0) {
                        // Force alignment to the next storage unit boundary for this base type.
                        flushUnit();
                        offset = alignTo(offset, unitAlign);
                        continue;
                    }

                    if (m.bitWidth < 0 || m.bitWidth > unitBits) {
                        // Semantic analysis should have rejected this, but don't crash here.
                        return 0;
                    }

                    // Start a new unit if needed (different base type, or none active yet).
                    if (currentUnitBitsTotal == 0 || currentUnitSizeBytes != unitSizeBytes || currentUnitAlign != unitAlign) {
                        flushUnit();
                        offset = alignTo(offset, unitAlign);
                        currentUnitSizeBytes = unitSizeBytes;
                        currentUnitAlign = unitAlign;
                        currentUnitBitsTotal = unitBits;
                    }

                    // If it doesn't fit, close current unit and start a new one.
                    if (currentUnitBitsUsed + m.bitWidth > currentUnitBitsTotal) {
                        flushUnit();
                        offset = alignTo(offset, unitAlign);
                        currentUnitSizeBytes = unitSizeBytes;
                        currentUnitAlign = unitAlign;
                        currentUnitBitsTotal = unitBits;
                    }

                    currentUnitBitsUsed += m.bitWidth;

                    // If unit is full, flush immediately.
                    if (currentUnitBitsUsed == currentUnitBitsTotal) {
                        flushUnit();
                    }
                }

                // Flush trailing partial unit.
                flushUnit();
                long long finalSize = alignTo(offset, maxAlign);
                return std::max(1LL, finalSize);
            }

            if (dynamic_cast<AST::EnumType*>(t)) {
                return 4;
            }

            return 4;
        };

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
                        result = is64bit_ ? 8 : 4; return true;
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
                result = is64bit_ ? 8 : 4;
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
                        // Build a temporary StructType from the declaration members so we can compute padding/align.
                        AST::StructType tmp(structType->name, tagSym->structDecl->isUnion, 0, 0);
                        tmp.isUnion = tagSym->structDecl->isUnion;
                        for (const auto& memberDecl : tagSym->structDecl->members) {
                            if (memberDecl && memberDecl->type) {
                                tmp.members.emplace_back(memberDecl->name, memberDecl->type->clone(), memberDecl->line, memberDecl->column);
                            }
                        }
                        result = typeSize(&tmp);
                        return true;
                    }
                }
                
                // Compute size with padding/align (and basic bitfield packing).
                result = typeSize(structType);
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
            case AST::UnaryOp::BitwiseNot:
            {
                // Perform bitwise NOT at the operand width (e.g., ~(unsigned long)1 differs on i386 vs x86_64)
                int bits = inferIntWidthBits(unary->operand.get());
                uint64_t uop = maskUnsigned(static_cast<uint64_t>(operand), bits);
                uint64_t ures = maskUnsigned(~uop, bits);
                result = static_cast<long long>(ures);
                return true;
            }
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
            {
                // Use unsigned arithmetic with the correct target width (important for enum constants on i386 vs x86_64)
                int bits = inferIntWidthBits(bin->left.get());
                uint64_t uleft = maskUnsigned(static_cast<uint64_t>(left), bits);
                uint64_t uright = static_cast<uint64_t>(right);
                if (uright == 0) return false;
                result = static_cast<long long>(uleft % uright);
                return true;
            }
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
    
    // Handle cast expression - may need to convert to unsigned
    if (auto* cast = dynamic_cast<AST::CastExpr*>(expr)) {
        if (!evaluateConstantExpr(cast->operand.get(), result)) return false;
        
        // Check if casting to unsigned type
        if (auto* prim = dynamic_cast<AST::PrimitiveType*>(cast->targetType.get())) {
            switch (prim->kind) {
                case AST::PrimitiveKind::UnsignedChar:
                    result = static_cast<long long>(static_cast<uint8_t>(result));
                    return true;
                case AST::PrimitiveKind::UnsignedShort:
                    result = static_cast<long long>(static_cast<uint16_t>(result));
                    return true;
                case AST::PrimitiveKind::UnsignedInt:
                    result = static_cast<long long>(static_cast<uint32_t>(result));
                    return true;
                case AST::PrimitiveKind::UnsignedLong:
                    result = static_cast<long long>(is64bit_ ? static_cast<uint64_t>(result) : static_cast<uint32_t>(result));
                    return true;
                case AST::PrimitiveKind::UnsignedLongLong:
                    result = static_cast<long long>(static_cast<uint64_t>(result));
                    return true;
                case AST::PrimitiveKind::SignedChar:
                    result = static_cast<long long>(static_cast<int8_t>(result));
                    return true;
                case AST::PrimitiveKind::Short:
                    result = static_cast<long long>(static_cast<int16_t>(result));
                    return true;
                case AST::PrimitiveKind::Int:
                    result = static_cast<long long>(static_cast<int32_t>(result));
                    return true;
                case AST::PrimitiveKind::Long:
                    result = static_cast<long long>(is64bit_ ? static_cast<int64_t>(result) : static_cast<int32_t>(result));
                    return true;
                case AST::PrimitiveKind::LongLong:
                    result = static_cast<long long>(static_cast<int64_t>(result));
                    return true;
                default:
                    break;
            }
        }
        return true;
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
        
        // Store computed value in AST
        enumerator.computedValue = nextValue;
        
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
