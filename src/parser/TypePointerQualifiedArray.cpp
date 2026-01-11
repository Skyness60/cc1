#include <parser/ast/Type.hpp>
#include <parser/ast/Visitor.hpp>

namespace AST {

void PointerType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool PointerType::equals(const Type& other) const {
    struct VisitorImpl : TypeVisitor {
        const PointerType& self;
        bool equal = false;

        explicit VisitorImpl(const PointerType& s) : self(s) {}

        void visit(const PrimitiveType&) override {}
        void visit(const PointerType& type) override { equal = self.pointee->equals(*type.pointee); }
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

std::string PointerType::toString() const {
    return pointee->toString() + " *";
}

Ptr<Type> PointerType::clone() const {
    return make<PointerType>(pointee->clone(), line, column);
}

void QualifiedType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool QualifiedType::equals(const Type& other) const {
    struct VisitorImpl : TypeVisitor {
        const QualifiedType& self;
        bool equal = false;

        explicit VisitorImpl(const QualifiedType& s) : self(s) {}

        void visit(const PrimitiveType&) override {}
        void visit(const PointerType&) override {}
        void visit(const QualifiedType& type) override {
            equal = self.baseType->equals(*type.baseType) && self.isConst == type.isConst &&
                    self.isVolatile == type.isVolatile;
        }
        void visit(const ArrayType&) override {}
        void visit(const FunctionType&) override {}
        void visit(const StructType&) override {}
        void visit(const EnumType&) override {}
        void visit(const TypedefType&) override {}
    } visitor(*this);

    other.accept(visitor);
    return visitor.equal;
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

void ArrayType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

bool ArrayType::equals(const Type& other) const {
    struct VisitorImpl : TypeVisitor {
        const ArrayType& self;
        bool equal = false;

        explicit VisitorImpl(const ArrayType& s) : self(s) {}

        void visit(const PrimitiveType&) override {}
        void visit(const PointerType&) override {}
        void visit(const QualifiedType&) override {}
        void visit(const ArrayType& type) override { equal = self.elementType->equals(*type.elementType); }
        void visit(const FunctionType&) override {}
        void visit(const StructType&) override {}
        void visit(const EnumType&) override {}
        void visit(const TypedefType&) override {}
    } visitor(*this);

    other.accept(visitor);
    return visitor.equal;
}

std::string ArrayType::toString() const {
    return elementType->toString() + "[]";
}

Ptr<Type> ArrayType::clone() const {
    if (size >= 0) {
        return make<ArrayType>(elementType->clone(), size, line, column);
    }

    if (sizeExpr) {
        struct SizeExprVisitor : DefaultVisitor {
            bool hasValue = false;
            long long value = 0;

            void visit(IntegerLiteral& node) override {
                hasValue = true;
                value = node.value;
            }
        } v;

        sizeExpr->accept(v);
        if (v.hasValue) {
            return make<ArrayType>(elementType->clone(), v.value, line, column);
        }
    }

    return make<ArrayType>(elementType->clone(), nullptr, line, column);
}

} // namespace AST
