#include <parser/ast/Type.hpp>
#include <parser/ast/Visitor.hpp>

namespace AST {

// EN: Accepts a type visitor for pointer types.
// FR: Accepte un visiteur de type pour les pointeurs.
void PointerType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

// EN: Compares pointer types by comparing pointee types.
// FR: Compare les types pointeurs via le type pointe.
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

// EN: Returns a string representation of the pointer type.
// FR: Renvoie une representation chaine du type pointeur.
std::string PointerType::toString() const {
    return pointee->toString() + " *";
}

// EN: Clones the pointer type with location preserved.
// FR: Clone le type pointeur en preservant la position.
Ptr<Type> PointerType::clone() const {
    return make<PointerType>(pointee->clone(), line, column);
}

// EN: Accepts a type visitor for qualified types.
// FR: Accepte un visiteur de type pour les types qualifies.
void QualifiedType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

// EN: Compares qualified types including qualifiers.
// FR: Compare les types qualifies y compris qualifiers.
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

// EN: Returns a string representation of the qualified type.
// FR: Renvoie une representation chaine du type qualifie.
std::string QualifiedType::toString() const {
    std::string result;
    if (isConst) result += "const ";
    if (isVolatile) result += "volatile ";
    result += baseType->toString();
    return result;
}

// EN: Clones the qualified type with location preserved.
// FR: Clone le type qualifie en preservant la position.
Ptr<Type> QualifiedType::clone() const {
    return make<QualifiedType>(baseType->clone(), isConst, isVolatile, line, column);
}

// EN: Accepts a type visitor for array types.
// FR: Accepte un visiteur de type pour les tableaux.
void ArrayType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

// EN: Compares array types by element type.
// FR: Compare les types tableau par type d element.
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

// EN: Returns a string representation of the array type.
// FR: Renvoie une representation chaine du type tableau.
std::string ArrayType::toString() const {
    return elementType->toString() + "[]";
}

// EN: Clones the array type, attempting to resolve constant sizes.
// FR: Clone le type tableau en essayant de resoudre la taille constante.
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

} 

// TODO(cc1) EN: Preserve array sizes from expressions beyond integer literals.
// FR: Preserver les tailles de tableau issues d expressions non literals.
