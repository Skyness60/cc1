#include <semantics/SemanticAnalyzer.hpp>
#include <utils/Diagnostic.hpp>
#include <sstream>
#include <iostream>

namespace cc1 {

// ============================================================================
// Scope Implementation
// ============================================================================

Symbol* Scope::lookup(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return &it->second;
    }
    if (parent) {
        return parent->lookup(name);
    }
    return nullptr;
}

Symbol* Scope::lookupTag(const std::string& name) {
    auto it = tags.find(name);
    if (it != tags.end()) {
        return &it->second;
    }
    if (parent) {
        return parent->lookupTag(name);
    }
    return nullptr;
}

Symbol* Scope::lookupLocal(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return &it->second;
    }
    return nullptr;
}

void Scope::define(const std::string& name, const Symbol& sym, AST::Ptr<AST::Type> type) {
    Symbol newSym = sym;
    if (type) {
        ownedTypes.push_back(std::move(type));
        newSym.type = ownedTypes.back().get();
    }
    symbols[name] = newSym;
}

void Scope::defineTag(const std::string& name, const Symbol& sym) {
    tags[name] = sym;
}

// ============================================================================
// SemanticAnalyzer Constructor
// ============================================================================

SemanticAnalyzer::SemanticAnalyzer(const std::string& filename, const std::string& source)
    : filename_(filename), source_(source) 
{
    // Create global scope
    enterScope(false);
}

void SemanticAnalyzer::analyze(AST::TranslationUnit& unit) {
    unit.accept(*this);
}

// ============================================================================
// Error Reporting
// ============================================================================

std::string SemanticAnalyzer::getSourceLine(int line) const {
    std::istringstream stream(source_);
    std::string currentLine;
    int currentNum = 0;
    while (std::getline(stream, currentLine)) {
        if (++currentNum == line) return currentLine;
    }
    return "";
}

void SemanticAnalyzer::error(int line, int column, const std::string& message) {
    hadError_ = true;
    SourceLocation loc(filename_, line, column);
    DiagnosticPrinter::print(DiagnosticKind::Error, loc, message, getSourceLine(line));
}

void SemanticAnalyzer::warning(int line, int column, const std::string& message) {
    SourceLocation loc(filename_, line, column);
    DiagnosticPrinter::print(DiagnosticKind::Warning, loc, message, getSourceLine(line));
}

// ============================================================================
// Scope Management
// ============================================================================

void SemanticAnalyzer::enterScope(bool isFunction) {
    auto scope = std::unique_ptr<Scope>(new Scope());
    scope->parent = currentScope_;
    scope->isFunction = isFunction;
    currentScope_ = scope.get();
    scopes_.push_back(std::move(scope));
}

void SemanticAnalyzer::exitScope() {
    if (currentScope_ && currentScope_->parent) {
        currentScope_ = currentScope_->parent;
    }
}

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

// ============================================================================
// Declaration Visitors
// ============================================================================

void SemanticAnalyzer::visit(AST::TranslationUnit& node) {
    inGlobalScope_ = true;
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
}

void SemanticAnalyzer::visit(AST::VarDecl& node) {
    // Check if this is a standalone enum definition (VarDecl with empty name and EnumType)
    if (node.name.empty()) {
        if (auto* enumType = dynamic_cast<AST::EnumType*>(node.type.get())) {
            processEnumType(enumType);
            return;
        }
    }
    
    // Check for void type - bad_semantic_0.c
    if (isVoidType(node.type.get())) {
        error(node.line, node.column, "variable has incomplete type 'void'");
        return;
    }
    
    // Check array size at file scope - bad_semantic_8.c
    if (inGlobalScope_) {
        if (auto* arrType = dynamic_cast<AST::ArrayType*>(stripQualifiers(node.type.get()))) {
            if (arrType->sizeExpr) {
                // Check if size is a constant expression
                if (!isConstantExpr(arrType->sizeExpr.get())) {
                    // Check if it's an identifier (VLA)
                    if (dynamic_cast<AST::Identifier*>(arrType->sizeExpr.get())) {
                        error(node.line, node.column, 
                              "variable length array declaration not allowed at file scope");
                        return;
                    }
                }
            }
        }
    }
    
    // Also check for enum types in variable declarations (e.g., "enum e { A = 1/0 } x;")
    if (auto* enumType = dynamic_cast<AST::EnumType*>(stripQualifiers(node.type.get()))) {
        processEnumType(enumType);
    }
    
    // Check for const variable without initialization - bad_semantic_1.c
    if (!inGlobalScope_ && isConstQualified(node.type.get()) && !node.initializer) {
        // Get the type name with const qualifier
        std::string typeName = "const " + typeToString(stripQualifiers(node.type.get()));
        warning(node.line, node.column,
                "default initialization of an object of type '" + typeName + 
                "' leaves the object uninitialized [-Wdefault-const-init-var-unsafe]");
    }
    
    // Check initializer type compatibility - bad_semantic_2.c, bad_semantic_3.c
    if (node.initializer) {
        node.initializer->accept(*this);
        AST::Ptr<AST::Type> initType = getExprType(node.initializer.get());
        
        if (initType) {
            AST::Type* destType = stripQualifiers(node.type.get());
            AST::Type* srcType = stripQualifiers(initType.get());
            
            // Check for pointer to integer conversion
            if (isIntegerType(destType) && (isPointerType(srcType) || isArrayType(srcType))) {
                // Get the initializer type string
                std::string initTypeStr;
                if (auto* strLit = dynamic_cast<AST::StringLiteral*>(node.initializer.get())) {
                    initTypeStr = "char[" + std::to_string(strLit->value.length() + 1) + "]";
                } else {
                    initTypeStr = typeToString(srcType);
                }
                error(node.line, node.column,
                      "incompatible pointer to integer conversion initializing '" + 
                      typeToString(node.type.get()) + "' with an expression of type '" + 
                      initTypeStr + "' [-Wint-conversion]");
            }
            // Check for integer to pointer conversion
            else if (isPointerType(destType) && isIntegerType(srcType)) {
                // Check if initializer is a dereference - suggest removing *
                std::string suggestion;
                if (dynamic_cast<AST::UnaryExpr*>(node.initializer.get())) {
                    auto* unary = dynamic_cast<AST::UnaryExpr*>(node.initializer.get());
                    if (unary->op == AST::UnaryOp::Dereference) {
                        suggestion = "; remove *";
                    }
                }
                error(node.line, node.column,
                      "incompatible integer to pointer conversion initializing '" +
                      typeToString(node.type.get()) + "' with an expression of type '" +
                      typeToString(srcType) + "'" + suggestion + " [-Wint-conversion]");
            }
        }
    }
    
    // Register in symbol table
    Symbol sym;
    sym.name = node.name;
    sym.isConst = isConstQualified(node.type.get());
    sym.line = node.line;
    sym.column = node.column;
    
    // Check if this is actually a function prototype (VarDecl with FunctionType)
    if (dynamic_cast<AST::FunctionType*>(stripQualifiers(node.type.get()))) {
        sym.isFunction = true;
    }
    
    currentScope_->define(node.name, sym, node.type ? node.type->clone() : nullptr);
}

void SemanticAnalyzer::visit(AST::FunctionDecl& node) {
    // Register function in current scope
    Symbol sym;
    sym.name = node.name;
    sym.isFunction = true;
    sym.line = node.line;
    sym.column = node.column;
    
    // Build function type
    std::vector<AST::Ptr<AST::Type>> paramTypes;
    for (const auto& param : node.parameters) {
        if (param && param->type) {
            paramTypes.push_back(param->type->clone());
        }
    }
    auto funcType = AST::make<AST::FunctionType>(
        node.returnType ? node.returnType->clone() : nullptr,
        std::move(paramTypes),
        node.isVariadic,
        node.line, node.column
    );
    
    currentScope_->define(node.name, sym, std::move(funcType));
    
    // If this is a definition, analyze the body
    if (node.body) {
        currentFunction_ = &node;
        inGlobalScope_ = false;
        
        enterScope(true);
        
        // Add parameters to scope
        for (auto& param : node.parameters) {
            if (param) param->accept(*this);
        }
        
        // Analyze body
        node.body->accept(*this);
        
        exitScope();
        
        currentFunction_ = nullptr;
        inGlobalScope_ = true;
    }
}

void SemanticAnalyzer::visit(AST::ParamDecl& node) {
    Symbol sym;
    sym.name = node.name;
    sym.isConst = isConstQualified(node.type.get());
    sym.line = node.line;
    sym.column = node.column;
    
    if (!node.name.empty()) {
        currentScope_->define(node.name, sym, node.type ? node.type->clone() : nullptr);
    }
}

void SemanticAnalyzer::visit(AST::StructDecl& node) {
    // Check for tag conflict - bad_semantic_5.c
    Symbol* existingTag = currentScope_->lookupTag(node.name);
    if (existingTag) {
        Symbol::TagKind expectedKind = node.isUnion ? Symbol::TagKind::Union : Symbol::TagKind::Struct;
        if (existingTag->tagKind != expectedKind) {
            error(node.line, node.column,
                  "use of '" + node.name + "' with tag type that does not match previous declaration");
            return;
        }
    }
    
    // Register struct tag
    Symbol sym;
    sym.name = node.name;
    sym.tagKind = node.isUnion ? Symbol::TagKind::Union : Symbol::TagKind::Struct;
    sym.line = node.line;
    sym.column = node.column;
    sym.structDecl = &node;  // Store pointer to struct declaration
    currentScope_->defineTag(node.name, sym);
    
    // Check members
    for (auto& member : node.members) {
        if (member) member->accept(*this);
    }
}

void SemanticAnalyzer::visit(AST::EnumDecl& node) {
    // Check for tag conflict - bad_semantic_5.c
    if (!node.name.empty()) {
        Symbol* existingTag = currentScope_->lookupTag(node.name);
        if (existingTag && existingTag->tagKind != Symbol::TagKind::Enum) {
            error(node.line, 1,  // Column 1 points to 'enum' keyword like clang
                  "use of '" + node.name + "' with tag type that does not match previous declaration");
            return;
        }
        
        // Register enum tag
        Symbol sym;
        sym.name = node.name;
        sym.tagKind = Symbol::TagKind::Enum;
        sym.line = node.line;
        sym.column = node.column;
        currentScope_->defineTag(node.name, sym);
    }
    
    // Process enumerators and check for division by zero - bad_semantic_7.c
    long long nextValue = 0;
    for (auto& enumerator : node.enumerators) {
        if (enumerator.value) {
            // Check if the value expression has division by zero
            if (hasDivisionByZero(enumerator.value.get())) {
                int col = enumerator.equalColumn > 0 ? enumerator.equalColumn : enumerator.column;
                error(enumerator.line, col,
                      "expression is not an integer constant expression");
                return;
            }
            
            // Evaluate the constant expression
            if (!evaluateConstantExpr(enumerator.value.get(), nextValue)) {
                int col = enumerator.equalColumn > 0 ? enumerator.equalColumn : enumerator.column;
                error(enumerator.line, col,
                      "expression is not an integer constant expression");
                return;
            }
        }
        
        // Register enumerator as constant
        Symbol sym;
        sym.name = enumerator.name;
        sym.isEnumConstant = true;
        sym.line = enumerator.line;
        sym.column = enumerator.column;
        currentScope_->define(enumerator.name, sym, AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, 0, 0));
        
        nextValue++;
    }
}

void SemanticAnalyzer::visit(AST::TypedefDecl& node) {
    Symbol sym;
    sym.name = node.name;
    sym.isTypedef = true;
    sym.line = node.line;
    sym.column = node.column;
    currentScope_->define(node.name, sym, node.underlyingType ? node.underlyingType->clone() : nullptr);
}

// ============================================================================
// Statement Visitors
// ============================================================================

void SemanticAnalyzer::visit(AST::CompoundStmt& node) {
    // Don't create new scope if we just entered a function (already done)
    bool needsScope = currentFunction_ == nullptr || !currentScope_->isFunction;
    
    if (needsScope) {
        enterScope(false);
    }
    
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
    
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
    
    if (needsScope) {
        exitScope();
    }
}

void SemanticAnalyzer::visit(AST::ExpressionStmt& node) {
    if (node.expression) {
        node.expression->accept(*this);
    }
}

void SemanticAnalyzer::visit(AST::IfStmt& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.thenBranch) node.thenBranch->accept(*this);
    if (node.elseBranch) node.elseBranch->accept(*this);
}

void SemanticAnalyzer::visit(AST::WhileStmt& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SemanticAnalyzer::visit(AST::DoWhileStmt& node) {
    if (node.body) node.body->accept(*this);
    if (node.condition) node.condition->accept(*this);
}

void SemanticAnalyzer::visit(AST::ForStmt& node) {
    if (node.init) node.init->accept(*this);
    if (node.condition) node.condition->accept(*this);
    if (node.increment) node.increment->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SemanticAnalyzer::visit(AST::SwitchStmt& node) {
    if (node.expression) node.expression->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SemanticAnalyzer::visit(AST::CaseStmt& node) {
    if (node.value) node.value->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SemanticAnalyzer::visit(AST::DefaultStmt& node) {
    if (node.body) node.body->accept(*this);
}

void SemanticAnalyzer::visit(AST::ReturnStmt& node) {
    if (node.value) node.value->accept(*this);
}

void SemanticAnalyzer::visit(AST::LabelStmt& node) {
    if (node.body) node.body->accept(*this);
}

// ============================================================================
// Expression Visitors
// ============================================================================

void SemanticAnalyzer::visit(AST::IntegerLiteral& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, node.line, node.column);
}

void SemanticAnalyzer::visit(AST::FloatLiteral& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Double, node.line, node.column);
}

void SemanticAnalyzer::visit(AST::CharLiteral& node) {
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Char, node.line, node.column);
}

void SemanticAnalyzer::visit(AST::StringLiteral& node) {
    // String literal has type char[]
    auto charType = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Char, node.line, node.column);
    auto size = AST::make<AST::IntegerLiteral>(static_cast<long long>(node.value.length() + 1), "", node.line, node.column);
    exprTypes_[&node] = AST::make<AST::ArrayType>(std::move(charType), std::move(size), node.line, node.column);
}

void SemanticAnalyzer::visit(AST::Identifier& node) {
    Symbol* sym = currentScope_->lookup(node.name);
    if (sym && sym->type) {
        exprTypes_[&node] = sym->type->clone();
    }
}

void SemanticAnalyzer::visit(AST::BinaryExpr& node) {
    if (node.left) node.left->accept(*this);
    if (node.right) node.right->accept(*this);
    
    AST::Ptr<AST::Type> leftType = getExprType(node.left.get());
    AST::Ptr<AST::Type> rightType = getExprType(node.right.get());
    
    // Check assignment to const - bad_semantic_1.c
    if (node.op == AST::BinaryOp::Assign) {
        if (auto* id = dynamic_cast<AST::Identifier*>(node.left.get())) {
            Symbol* sym = currentScope_->lookup(id->name);
            if (sym && sym->isConst) {
                error(node.line, node.column,
                      "cannot assign to variable '" + id->name + "' with const-qualified type '" +
                      typeToString(sym->type) + "'");
                return;
            }
        }
        
        // Check type compatibility for assignment
        if (leftType && rightType) {
            if (!canAssign(leftType.get(), rightType.get())) {
                // Check for pointer/integer conversion
                if (isPointerType(leftType.get()) && isIntegerType(rightType.get())) {
                    error(node.line, node.column,
                          "incompatible integer to pointer conversion assigning to '" +
                          typeToString(leftType.get()) + "' from '" + 
                          typeToString(rightType.get()) + "' [-Wint-conversion]");
                } else if (isIntegerType(leftType.get()) && isPointerType(rightType.get())) {
                    error(node.line, node.column,
                          "incompatible pointer to integer conversion assigning to '" +
                          typeToString(leftType.get()) + "' from '" + 
                          typeToString(rightType.get()) + "' [-Wint-conversion]");
                }
            }
        }
    }
    
    // Determine result type
    if (leftType) {
        exprTypes_[&node] = std::move(leftType);
    }
}

void SemanticAnalyzer::visit(AST::UnaryExpr& node) {
    if (node.operand) node.operand->accept(*this);
    
    AST::Ptr<AST::Type> operandType = getExprType(node.operand.get());
    
    switch (node.op) {
        case AST::UnaryOp::AddressOf:
            // &x produces pointer to x's type
            if (operandType) {
                exprTypes_[&node] = AST::make<AST::PointerType>(std::move(operandType), node.line, node.column);
            }
            break;
            
        case AST::UnaryOp::Dereference:
            // *p produces pointee type - bad_semantic_3.c
            if (operandType) {
                AST::Type* base = stripQualifiers(operandType.get());
                if (auto* ptr = dynamic_cast<AST::PointerType*>(base)) {
                    exprTypes_[&node] = ptr->pointee->clone();
                } else if (auto* arr = dynamic_cast<AST::ArrayType*>(base)) {
                    exprTypes_[&node] = arr->elementType->clone();
                } else {
                    // Dereferencing non-pointer - this gives the underlying type
                    // (e.g., *int_ptr gives int)
                    exprTypes_[&node] = std::move(operandType);
                }
            }
            break;
            
        default:
            if (operandType) {
                exprTypes_[&node] = std::move(operandType);
            }
            break;
    }
}

void SemanticAnalyzer::visit(AST::CastExpr& node) {
    if (node.operand) node.operand->accept(*this);
    
    if (node.targetType) {
        exprTypes_[&node] = node.targetType->clone();
    }
}

void SemanticAnalyzer::visit(AST::CallExpr& node) {
    // Visit function expression
    if (node.callee) node.callee->accept(*this);
    
    // Visit arguments
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
    
    // Get function type and check argument types - bad_semantic_4.c, bad_semantic_9.c
    if (auto* id = dynamic_cast<AST::Identifier*>(node.callee.get())) {
        Symbol* sym = currentScope_->lookup(id->name);
        if (sym && sym->isFunction && sym->type) {
            if (auto* funcType = dynamic_cast<AST::FunctionType*>(sym->type)) {
                // Check argument count and types
                size_t paramCount = funcType->parameterTypes.size();
                size_t argCount = node.arguments.size();
                
                // Check each argument
                for (size_t i = 0; i < argCount && i < paramCount; ++i) {
                    AST::Ptr<AST::Type> argType = getExprType(node.arguments[i].get());
                    AST::Type* paramType = funcType->parameterTypes[i].get();
                    
                    if (argType && paramType) {
                        if (!canAssign(paramType, argType.get())) {
                            // Check for specific conversion errors
                            if (isIntegerType(stripQualifiers(paramType)) && 
                                (isPointerType(argType.get()) || isArrayType(argType.get()))) {
                                std::string argTypeStr;
                                if (auto* strLit = dynamic_cast<AST::StringLiteral*>(node.arguments[i].get())) {
                                    argTypeStr = "char[" + std::to_string(strLit->value.length() + 1) + "]";
                                } else {
                                    argTypeStr = typeToString(argType.get());
                                }
                                error(node.arguments[i]->line, node.arguments[i]->column,
                                      "incompatible pointer to integer conversion passing '" +
                                      argTypeStr + "' to parameter of type '" +
                                      typeToString(paramType) + "' [-Wint-conversion]");
                            } else if (isPointerType(paramType) && isIntegerType(argType.get())) {
                                error(node.arguments[i]->line, node.arguments[i]->column,
                                      "incompatible integer to pointer conversion passing '" +
                                      typeToString(argType.get()) + "' to parameter of type '" +
                                      typeToString(paramType) + "' [-Wint-conversion]");
                            }
                        }
                    }
                }
                
                // Set return type
                if (funcType->returnType) {
                    exprTypes_[&node] = funcType->returnType->clone();
                }
            }
        }
    }
}

void SemanticAnalyzer::visit(AST::MemberExpr& node) {
    if (node.object) node.object->accept(*this);
    
    // TODO: Check member exists and set type
}

void SemanticAnalyzer::visit(AST::IndexExpr& node) {
    if (node.array) node.array->accept(*this);
    if (node.index) node.index->accept(*this);
    
    AST::Ptr<AST::Type> arrayType = getExprType(node.array.get());
    if (arrayType) {
        AST::Type* base = stripQualifiers(arrayType.get());
        if (auto* arr = dynamic_cast<AST::ArrayType*>(base)) {
            exprTypes_[&node] = arr->elementType->clone();
        } else if (auto* ptr = dynamic_cast<AST::PointerType*>(base)) {
            exprTypes_[&node] = ptr->pointee->clone();
        }
    }
}

void SemanticAnalyzer::visit(AST::TernaryExpr& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.thenExpr) node.thenExpr->accept(*this);
    if (node.elseExpr) node.elseExpr->accept(*this);
    
    // Result type is the common type of then and else branches
    AST::Ptr<AST::Type> thenType = getExprType(node.thenExpr.get());
    if (thenType) {
        exprTypes_[&node] = std::move(thenType);
    }
}

void SemanticAnalyzer::visit(AST::SizeofExpr& node) {
    // sizeof always returns size_t (unsigned long in C89)
    exprTypes_[&node] = AST::make<AST::PrimitiveType>(AST::PrimitiveKind::UnsignedLong, node.line, node.column);
}

void SemanticAnalyzer::visit(AST::InitializerList& node) {
    for (auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

} // namespace cc1
