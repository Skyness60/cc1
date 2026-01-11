#pragma once

#include <codegen/IRGenerator.hpp>

#include <cctype>

namespace cc1 {
namespace ir_expr_call_helpers {

inline std::string trimCopy(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(start, end - start);
}

inline bool startsWith(const std::string& s, const std::string& prefix) {
    return s.rfind(prefix, 0) == 0;
}

inline bool isAggregateLLVMType(const std::string& t) {
    return !t.empty() && (t[0] == '[' || t[0] == '{' || startsWith(t, "%struct.") || startsWith(t, "%union."));
}

inline bool isUnsignedIntegralResolvedType(const AST::Expression& expr) {
    if (!expr.resolvedType) return false;
    // Only handle primitive unsigneds; everything else defaults to signed.
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

inline bool shouldLoadCallArgument(const IRValue& v) {
    if (!v.isPointer || v.isConstant) return false;
    std::string dt = v.derefType();
    // Do not load aggregates (arrays/structs/unions). Keep pointer so it decays/passes by reference.
    if (isAggregateLLVMType(dt)) return false;
    return true;
}

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

} // namespace ir_expr_call_helpers
} // namespace cc1
