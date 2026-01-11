#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

void SemanticAnalyzer::visit(AST::VarDecl& node) {
    if (node.name.empty()) {
        if (auto* enumType = dynamic_cast<AST::EnumType*>(node.type.get())) {
            processEnumType(enumType);
            return;
        }
    }

    if (isVoidType(node.type.get())) {
        error(node.line, node.column, "variable has incomplete type 'void'");
        return;
    }

    if (inGlobalScope_) {
        if (auto* arrType = dynamic_cast<AST::ArrayType*>(stripQualifiers(node.type.get()))) {
            if (arrType->sizeExpr) {
                if (!isConstantExpr(arrType->sizeExpr.get())) {
                    if (dynamic_cast<AST::Identifier*>(arrType->sizeExpr.get())) {
                        error(node.line, node.column,
                              "variable length array declaration not allowed at file scope");
                        return;
                    }
                }
            }
        }
    }

    if (auto* enumType = dynamic_cast<AST::EnumType*>(stripQualifiers(node.type.get()))) {
        processEnumType(enumType);
    }

    if (!inGlobalScope_ && isConstQualified(node.type.get()) && !node.initializer) {
        std::string typeName = "const " + typeToString(stripQualifiers(node.type.get()));
        warning(node.line, node.column,
                "default initialization of an object of type '" + typeName +
                    "' leaves the object uninitialized [-Wdefault-const-init-var-unsafe]");
    }

    if (node.initializer) {
        node.initializer->accept(*this);
        AST::Ptr<AST::Type> initType = getExprType(node.initializer.get());

        if (initType) {
            AST::Type* destType = stripQualifiers(node.type.get());
            AST::Type* srcType = stripQualifiers(initType.get());

            if (isIntegerType(destType) && (isPointerType(srcType) || isArrayType(srcType))) {
                std::string initTypeStr;
                if (auto* strLit = dynamic_cast<AST::StringLiteral*>(node.initializer.get())) {
                    initTypeStr = "char[" + std::to_string(strLit->value.length() + 1) + "]";
                } else {
                    initTypeStr = typeToString(srcType);
                }
                error(node.line, node.column,
                      "incompatible pointer to integer conversion initializing '" +
                          typeToString(node.type.get()) + "' with an expression of type '" +
                          initTypeStr + "' [-Wint-conversion]");
            } else if (isPointerType(destType) && isIntegerType(srcType)) {
                std::string suggestion;
                if (dynamic_cast<AST::UnaryExpr*>(node.initializer.get())) {
                    auto* unary = dynamic_cast<AST::UnaryExpr*>(node.initializer.get());
                    if (unary->op == AST::UnaryOp::Dereference) {
                        suggestion = "; remove *";
                    }
                }
                error(node.line, node.column,
                      "incompatible integer to pointer conversion initializing '" +
                          typeToString(node.type.get()) + "' with an expression of type '" +
                          typeToString(srcType) + "'" + suggestion + " [-Wint-conversion]");
            }
        }
    }

    Symbol sym;
    sym.name = node.name;
    sym.isConst = isConstQualified(node.type.get());
    sym.line = node.line;
    sym.column = node.column;

    if (dynamic_cast<AST::FunctionType*>(stripQualifiers(node.type.get()))) {
        sym.isFunction = true;
    }

    currentScope_->define(node.name, sym, node.type ? node.type->clone() : nullptr);
}

void SemanticAnalyzer::visit(AST::ParamDecl& node) {
    Symbol sym;
    sym.name = node.name;
    sym.isConst = isConstQualified(node.type.get());
    sym.line = node.line;
    sym.column = node.column;

    if (!node.name.empty()) {
        currentScope_->define(node.name, sym, node.type ? node.type->clone() : nullptr);
    }
}

} // namespace cc1
