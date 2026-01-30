#include <parser/ast/Type.hpp>
#include <parser/ast/Expression.hpp>

namespace AST {

// EN: Accepts a type visitor for primitive types.
// FR: Accepte un visiteur de type pour les types primitifs.
void PrimitiveType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

// EN: Compares this primitive type against another type.
// FR: Compare ce type primitif a un autre type.
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

// EN: Returns a string representation of the primitive type.
// FR: Renvoie une representation chaine du type primitif.
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

// EN: Clones this primitive type with location preserved.
// FR: Clone ce type primitif en preservant la position.
Ptr<Type> PrimitiveType::clone() const {
    return make<PrimitiveType>(kind, line, column);
}

} 

// TODO(cc1) EN: Add C23 _Bool/_Float16 if supported.
// FR: Ajouter _Bool/_Float16 si supporte.
