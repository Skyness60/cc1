#include <parser/ast/Type.hpp>
#include <parser/ast/Expression.hpp>

namespace AST {

// EN: Accepts a type visitor for function types.
// FR: Accepte un visiteur de type pour les fonctions.
void FunctionType::accept(TypeVisitor& visitor) const {
    visitor.visit(*this);
}

// EN: Compares function types by return type, params, and variadic flag.
// FR: Compare les types fonction par retour, params et variadique.
bool FunctionType::equals(const Type& other) const {
    struct VisitorImpl : TypeVisitor {
        const FunctionType& self;
        bool equal = false;

        explicit VisitorImpl(const FunctionType& s) : self(s) {}

        void visit(const PrimitiveType&) override {}
        void visit(const PointerType&) override {}
        void visit(const QualifiedType&) override {}
        void visit(const ArrayType&) override {}
        void visit(const FunctionType& type) override {
            if (!self.returnType->equals(*type.returnType)) return;
            if (self.parameterTypes.size() != type.parameterTypes.size()) return;
            if (self.isVariadic != type.isVariadic) return;
            for (size_t i = 0; i < self.parameterTypes.size(); ++i) {
                if (!self.parameterTypes[i]->equals(*type.parameterTypes[i])) return;
            }
            equal = true;
        }
        void visit(const StructType&) override {}
        void visit(const EnumType&) override {}
        void visit(const TypedefType&) override {}
    } visitor(*this);

    other.accept(visitor);
    return visitor.equal;
}

// EN: Returns a string representation of the function type.
// FR: Renvoie une representation chaine du type fonction.
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

// EN: Clones the function type with parameter list copied.
// FR: Clone le type fonction avec copie des parametres.
Ptr<Type> FunctionType::clone() const {
    std::vector<Ptr<Type>> params;
    for (const auto& p : parameterTypes) {
        params.push_back(p->clone());
    }
    return make<FunctionType>(returnType->clone(), std::move(params), isVariadic, line, column);
}

} 

// TODO(cc1) EN: Add support for old-style K&R function prototypes.
// FR: Ajouter le support des prototypes K&R si necessaire.
