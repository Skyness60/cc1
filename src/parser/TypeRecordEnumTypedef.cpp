#include <parser/ast/Type.hpp>
#include <parser/ast/Expression.hpp>

namespace AST {

void StructType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

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

std::string StructType::toString() const {
    std::string result = isUnion ? "union" : "struct";
    if (!name.empty()) result += " " + name;
    return result;
}

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

void EnumType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

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

std::string EnumType::toString() const {
    std::string result = "enum";
    if (!name.empty()) result += " " + name;
    return result;
}

Ptr<Type> EnumType::clone() const {
    return make<EnumType>(name, line, column);
}

void TypedefType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

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
