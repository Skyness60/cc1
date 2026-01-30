#include <parser/ast/Type.hpp>
#include <parser/ast/Expression.hpp>

namespace AST {

// EN: Accepts a type visitor for struct/union types.
// FR: Accepte un visiteur de type pour struct/union.
void StructType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

// EN: Compares struct/union types by name and union flag.
// FR: Compare les types struct/union par nom et flag union.
bool StructType::equals(const Type& other) const {
    struct VisitorImpl : TypeVisitor {
        const StructType& self;
        bool equal = false;

        explicit VisitorImpl(const StructType& s) : self(s) {}

        void visit(const PrimitiveType&) override {}
        void visit(const PointerType&) override {}
        void visit(const QualifiedType&) override {}
        void visit(const ArrayType&) override {}
        void visit(const FunctionType&) override {}
        void visit(const StructType& type) override {
            if (!self.name.empty() && !type.name.empty()) {
                equal = (self.name == type.name && self.isUnion == type.isUnion);
            }
        }
        void visit(const EnumType&) override {}
        void visit(const TypedefType&) override {}
    } visitor(*this);

    other.accept(visitor);
    return visitor.equal;
}

// EN: Returns a string representation of the struct/union type.
// FR: Renvoie une representation chaine du type struct/union.
std::string StructType::toString() const {
    std::string result = isUnion ? "union" : "struct";
    if (!name.empty()) result += " " + name;
    return result;
}

// EN: Clones the struct/union type including member list.
// FR: Clone le type struct/union avec liste de membres.
Ptr<Type> StructType::clone() const {
    auto copy = make<StructType>(name, isUnion, line, column);
    for (const auto& member : members) {
        if (member.isBitfield()) {
            copy->members.emplace_back(member.name, member.type->clone(), member.line, member.column,
                                       member.bitWidth);
        } else {
            copy->members.emplace_back(member.name, member.type->clone(), member.line, member.column);
        }
    }
    copy->isComplete = isComplete;
    return copy;
}

// EN: Accepts a type visitor for enum types.
// FR: Accepte un visiteur de type pour enums.
void EnumType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

// EN: Compares enum types by name.
// FR: Compare les types enum par nom.
bool EnumType::equals(const Type& other) const {
    struct VisitorImpl : TypeVisitor {
        const EnumType& self;
        bool equal = false;

        explicit VisitorImpl(const EnumType& s) : self(s) {}

        void visit(const PrimitiveType&) override {}
        void visit(const PointerType&) override {}
        void visit(const QualifiedType&) override {}
        void visit(const ArrayType&) override {}
        void visit(const FunctionType&) override {}
        void visit(const StructType&) override {}
        void visit(const EnumType& type) override {
            if (!self.name.empty() && !type.name.empty()) {
                equal = (self.name == type.name);
            }
        }
        void visit(const TypedefType&) override {}
    } visitor(*this);

    other.accept(visitor);
    return visitor.equal;
}

// EN: Returns a string representation of the enum type.
// FR: Renvoie une representation chaine du type enum.
std::string EnumType::toString() const {
    std::string result = "enum";
    if (!name.empty()) result += " " + name;
    return result;
}

// EN: Clones the enum type (name only).
// FR: Clone le type enum (nom seulement).
Ptr<Type> EnumType::clone() const {
    return make<EnumType>(name, line, column);
}

// EN: Accepts a type visitor for typedef types.
// FR: Accepte un visiteur de type pour typedef.
void TypedefType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

// EN: Compares typedef types by name.
// FR: Compare les types typedef par nom.
bool TypedefType::equals(const Type& other) const {
    struct VisitorImpl : TypeVisitor {
        const TypedefType& self;
        bool equal = false;

        explicit VisitorImpl(const TypedefType& s) : self(s) {}

        void visit(const PrimitiveType&) override {}
        void visit(const PointerType&) override {}
        void visit(const QualifiedType&) override {}
        void visit(const ArrayType&) override {}
        void visit(const FunctionType&) override {}
        void visit(const StructType&) override {}
        void visit(const EnumType&) override {}
        void visit(const TypedefType& type) override { equal = (self.name == type.name); }
    } visitor(*this);

    other.accept(visitor);
    return visitor.equal;
}

// EN: Returns the typedef name as its string representation.
// FR: Renvoie le nom typedef comme representation chaine.
std::string TypedefType::toString() const {
    return name;
}

// EN: Clones the typedef type, preserving underlying type when available.
// FR: Clone le type typedef en preservant le type sous-jacent si dispo.
Ptr<Type> TypedefType::clone() const {
    auto copy = make<TypedefType>(name, line, column);
    if (underlyingType) {
        copy->underlyingType = underlyingType->clone();
    }
    return copy;
}

} 

// TODO(cc1) EN: Compare structural equality for anonymous structs/enums.
// FR: Comparer l egalite structurelle pour structs/enums anonymes.
