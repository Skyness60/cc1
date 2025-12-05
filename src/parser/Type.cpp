#include <parser/ast/Type.hpp>
#include <parser/ast/Expression.hpp>

namespace AST {

// ============================================================================
// PrimitiveType
// ============================================================================

void PrimitiveType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool PrimitiveType::equals(const Type& other) const {
    const PrimitiveType* p = dynamic_cast<const PrimitiveType*>(&other);
    if (!p) return false;
    return kind == p->kind;
}

std::string PrimitiveType::toString() const {
    switch (kind) {
        case PrimitiveKind::Void:           return "void";
        case PrimitiveKind::Char:           return "char";
        case PrimitiveKind::SignedChar:     return "signed char";
        case PrimitiveKind::UnsignedChar:   return "unsigned char";
        case PrimitiveKind::Short:          return "short";
        case PrimitiveKind::UnsignedShort:  return "unsigned short";
        case PrimitiveKind::Int:            return "int";
        case PrimitiveKind::UnsignedInt:    return "unsigned int";
        case PrimitiveKind::Long:           return "long";
        case PrimitiveKind::UnsignedLong:   return "unsigned long";
        case PrimitiveKind::LongLong:       return "long long";
        case PrimitiveKind::UnsignedLongLong: return "unsigned long long";
        case PrimitiveKind::Float:          return "float";
        case PrimitiveKind::Double:         return "double";
        case PrimitiveKind::LongDouble:     return "long double";
    }
    return "unknown";
}

Ptr<Type> PrimitiveType::clone() const {
    return make<PrimitiveType>(kind, line, column);
}

// ============================================================================
// PointerType
// ============================================================================

void PointerType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool PointerType::equals(const Type& other) const {
    const PointerType* p = dynamic_cast<const PointerType*>(&other);
    if (!p) return false;
    return pointee->equals(*p->pointee);
}

std::string PointerType::toString() const {
    return pointee->toString() + " *";
}

Ptr<Type> PointerType::clone() const {
    return make<PointerType>(pointee->clone(), line, column);
}

// ============================================================================
// QualifiedType
// ============================================================================

void QualifiedType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool QualifiedType::equals(const Type& other) const {
    const QualifiedType* q = dynamic_cast<const QualifiedType*>(&other);
    if (!q) return false;
    return baseType->equals(*q->baseType) &&
           isConst == q->isConst &&
           isVolatile == q->isVolatile;
}

std::string QualifiedType::toString() const {
    std::string result;
    if (isConst) result += "const ";
    if (isVolatile) result += "volatile ";
    result += baseType->toString();
    return result;
}

Ptr<Type> QualifiedType::clone() const {
    return make<QualifiedType>(baseType->clone(), isConst, isVolatile, line, column);
}

// ============================================================================
// ArrayType
// ============================================================================

void ArrayType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool ArrayType::equals(const Type& other) const {
    const ArrayType* a = dynamic_cast<const ArrayType*>(&other);
    if (!a) return false;
    // Don't compare size expressions for type equality
    return elementType->equals(*a->elementType);
}

std::string ArrayType::toString() const {
    return elementType->toString() + "[]";
}

Ptr<Type> ArrayType::clone() const {
    // Clone with computed size if available
    if (size >= 0) {
        return make<ArrayType>(elementType->clone(), size, line, column);
    }
    // If we have a size expression that's an integer literal, use it
    if (sizeExpr) {
        if (auto* intLit = dynamic_cast<IntegerLiteral*>(sizeExpr.get())) {
            return make<ArrayType>(elementType->clone(), intLit->value, line, column);
        }
    }
    // Fallback: no size info
    auto result = make<ArrayType>(elementType->clone(), nullptr, line, column);
    return result;
}

// ============================================================================
// FunctionType
// ============================================================================

void FunctionType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool FunctionType::equals(const Type& other) const {
    const FunctionType* f = dynamic_cast<const FunctionType*>(&other);
    if (!f) return false;
    if (!returnType->equals(*f->returnType)) return false;
    if (parameterTypes.size() != f->parameterTypes.size()) return false;
    if (isVariadic != f->isVariadic) return false;
    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        if (!parameterTypes[i]->equals(*f->parameterTypes[i])) return false;
    }
    return true;
}

std::string FunctionType::toString() const {
    std::string result = returnType->toString() + "(";
    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        if (i > 0) result += ", ";
        result += parameterTypes[i]->toString();
    }
    if (isVariadic) {
        if (!parameterTypes.empty()) result += ", ";
        result += "...";
    }
    result += ")";
    return result;
}

Ptr<Type> FunctionType::clone() const {
    std::vector<Ptr<Type>> params;
    for (const auto& p : parameterTypes) {
        params.push_back(p->clone());
    }
    return make<FunctionType>(returnType->clone(), std::move(params), 
                               isVariadic, line, column);
}

// ============================================================================
// StructType
// ============================================================================

void StructType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool StructType::equals(const Type& other) const {
    const StructType* s = dynamic_cast<const StructType*>(&other);
    if (!s) return false;
    // Structs are compared by name (or by structure if anonymous)
    if (!name.empty() && !s->name.empty()) {
        return name == s->name && isUnion == s->isUnion;
    }
    return false; // Anonymous structs are never equal
}

std::string StructType::toString() const {
    std::string result = isUnion ? "union" : "struct";
    if (!name.empty()) result += " " + name;
    return result;
}

Ptr<Type> StructType::clone() const {
    auto copy = make<StructType>(name, isUnion, line, column);
    for (const auto& member : members) {
        copy->members.emplace_back(member.name, member.type->clone(),
                                    member.line, member.column);
    }
    copy->isComplete = isComplete;
    return copy;
}

// ============================================================================
// EnumType
// ============================================================================

void EnumType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool EnumType::equals(const Type& other) const {
    const EnumType* e = dynamic_cast<const EnumType*>(&other);
    if (!e) return false;
    if (!name.empty() && !e->name.empty()) {
        return name == e->name;
    }
    return false;  // Anonymous enums are never equal
}

std::string EnumType::toString() const {
    std::string result = "enum";
    if (!name.empty()) result += " " + name;
    return result;
}

Ptr<Type> EnumType::clone() const {
    auto copy = make<EnumType>(name, line, column);
    // Enumerators are not cloned (would need expression cloning)
    return copy;
}

// ============================================================================
// TypedefType
// ============================================================================

void TypedefType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool TypedefType::equals(const Type& other) const {
    const TypedefType* t = dynamic_cast<const TypedefType*>(&other);
    if (!t) return false;
    return name == t->name;
}

std::string TypedefType::toString() const {
    return name;
}

Ptr<Type> TypedefType::clone() const {
    auto copy = make<TypedefType>(name, line, column);
    if (underlyingType) {
        copy->underlyingType = underlyingType->clone();
    }
    return copy;
}

} // namespace AST
