#include <parser/ast/Type.hpp>
#include <parser/ast/Expression.hpp>

namespace AST {

void PrimitiveType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool PrimitiveType::equals(const Type& other) const {
    struct VisitorImpl : TypeVisitor {
        const PrimitiveType& self;
        bool equal = false;

        explicit VisitorImpl(const PrimitiveType& s) : self(s) {}

        void visit(const PrimitiveType& type) override { equal = (self.kind == type.kind); }
        void visit(const PointerType&) override {}
        void visit(const QualifiedType&) override {}
        void visit(const ArrayType&) override {}
        void visit(const FunctionType&) override {}
        void visit(const StructType&) override {}
        void visit(const EnumType&) override {}
        void visit(const TypedefType&) override {}
    } visitor(*this);

    other.accept(visitor);
    return visitor.equal;
}

std::string PrimitiveType::toString() const {
    switch (kind) {
        case PrimitiveKind::Void: return "void";
        case PrimitiveKind::Char: return "char";
        case PrimitiveKind::SignedChar: return "signed char";
        case PrimitiveKind::UnsignedChar: return "unsigned char";
        case PrimitiveKind::Short: return "short";
        case PrimitiveKind::UnsignedShort: return "unsigned short";
        case PrimitiveKind::Int: return "int";
        case PrimitiveKind::UnsignedInt: return "unsigned int";
        case PrimitiveKind::Long: return "long";
        case PrimitiveKind::UnsignedLong: return "unsigned long";
        case PrimitiveKind::LongLong: return "long long";
        case PrimitiveKind::UnsignedLongLong: return "unsigned long long";
        case PrimitiveKind::Float: return "float";
        case PrimitiveKind::Double: return "double";
        case PrimitiveKind::LongDouble: return "long double";
    }
    return "unknown";
}

Ptr<Type> PrimitiveType::clone() const {
    return make<PrimitiveType>(kind, line, column);
}

} // namespace AST
