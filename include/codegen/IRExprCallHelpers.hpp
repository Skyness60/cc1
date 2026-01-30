#pragma once



#include <codegen/IRGenerator.hpp>

#include <cctype>

namespace cc1 {
namespace ir_expr_call_helpers {

// EN: Trims leading/trailing whitespace without modifying the input.
// FR: Supprime les espaces en debut/fin sans modifier l entree.
inline std::string trimCopy(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(start, end - start);
}

// EN: Checks whether a string starts with the given prefix.
// FR: Verifie si une chaine commence par le prefixe donne.
inline bool startsWith(const std::string& s, const std::string& prefix) {
    return s.rfind(prefix, 0) == 0;
}

// EN: Detects aggregate LLVM types (struct/union/array) by textual shape.
// FR: Detecte les types LLVM agreges (struct/union/tableau) par leur forme.
inline bool isAggregateLLVMType(const std::string& t) {
    return !t.empty() && (t[0] == '[' || t[0] == '{' || startsWith(t, "%struct.") || startsWith(t, "%union."));
}

// EN: Checks if the resolved expression type is an unsigned integer.
// FR: Verifie si le type resolu de l expression est un entier non signe.
inline bool isUnsignedIntegralResolvedType(const AST::Expression& expr) {
    if (!expr.resolvedType) return false;

    if (auto* prim = dynamic_cast<AST::PrimitiveType*>(expr.resolvedType.get())) {
        switch (prim->kind) {
            case AST::PrimitiveKind::UnsignedChar:
            case AST::PrimitiveKind::UnsignedShort:
            case AST::PrimitiveKind::UnsignedInt:
            case AST::PrimitiveKind::UnsignedLong:
            case AST::PrimitiveKind::UnsignedLongLong:
                return true;
            default:
                return false;
        }
    }
    return false;
}

// EN: Decides whether a call argument should be loaded from pointer storage.
// FR: Decide si un argument d appel doit etre charge depuis un pointeur.
inline bool shouldLoadCallArgument(const IRValue& v) {
    if (!v.isPointer || v.isConstant) return false;
    std::string dt = v.derefType();

    if (isAggregateLLVMType(dt)) return false;
    return true;
}

// EN: Parses LLVM function declaration line to extract parameter types.
// FR: Parse une declaration LLVM pour extraire les types de parametres.
inline bool parseFunctionDeclParamTypes(const std::string& declLine,
                                       std::vector<std::string>& outParamTypes,
                                       bool& outIsVariadic) {
    outParamTypes.clear();
    outIsVariadic = false;
    auto l = declLine.find('(');
    auto r = declLine.rfind(')');
    if (l == std::string::npos || r == std::string::npos || r <= l) return false;
    std::string inside = declLine.substr(l + 1, r - l - 1);
    inside = trimCopy(inside);
    if (inside.empty()) return true;

    size_t pos = 0;
    while (pos < inside.size()) {
        size_t comma = inside.find(',', pos);
        std::string tok = (comma == std::string::npos) ? inside.substr(pos) : inside.substr(pos, comma - pos);
        tok = trimCopy(tok);
        if (tok == "...") {
            outIsVariadic = true;
        } else if (!tok.empty()) {
            outParamTypes.push_back(tok);
        }
        if (comma == std::string::npos) break;
        pos = comma + 1;
    }
    return true;
}

}
}
