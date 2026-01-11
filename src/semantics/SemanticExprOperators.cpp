#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

static void setResolvedExprType(AST::Expression& expr, const AST::Ptr<AST::Type>& type) {
    expr.resolvedType = type ? type->clone() : nullptr;
}

void SemanticAnalyzer::visit(AST::BinaryExpr& node) {
    if (node.left) node.left->accept(*this);
    if (node.right) node.right->accept(*this);

    AST::Ptr<AST::Type> leftType = getExprType(node.left.get());
    AST::Ptr<AST::Type> rightType = getExprType(node.right.get());

    if (node.op == AST::BinaryOp::Assign) {
        if (auto* id = dynamic_cast<AST::Identifier*>(node.left.get())) {
            Symbol* sym = currentScope_->lookup(id->name);
            if (sym && sym->isConst) {
                error(node.line, node.column,
                      "cannot assign to variable '" + id->name + "' with const-qualified type '" +
                      typeToString(sym->type) + "'");
                return;
            }
        }

        if (leftType && rightType) {
            if (!canAssign(leftType.get(), rightType.get())) {
                if (isPointerType(leftType.get()) && isIntegerType(rightType.get())) {
                    error(node.line, node.column,
                          "incompatible integer to pointer conversion assigning to '" +
                          typeToString(leftType.get()) + "' from '" +
                          typeToString(rightType.get()) + "' [-Wint-conversion]");
                } else if (isIntegerType(leftType.get()) && isPointerType(rightType.get())) {
                    error(node.line, node.column,
                          "incompatible pointer to integer conversion assigning to '" +
                          typeToString(leftType.get()) + "' from '" +
                          typeToString(rightType.get()) + "' [-Wint-conversion]");
                }
            }
        }
    }

    if (leftType) {
        exprTypes_[&node] = std::move(leftType);
        setResolvedExprType(node, exprTypes_[&node]);
    }
}

void SemanticAnalyzer::visit(AST::UnaryExpr& node) {
    if (node.operand) node.operand->accept(*this);

    AST::Ptr<AST::Type> operandType = getExprType(node.operand.get());

    switch (node.op) {
        case AST::UnaryOp::AddressOf:
            if (operandType) {
                exprTypes_[&node] = AST::make<AST::PointerType>(std::move(operandType), node.line, node.column);
                setResolvedExprType(node, exprTypes_[&node]);
            }
            break;
        case AST::UnaryOp::Dereference:
            if (operandType) {
                AST::Type* base = stripQualifiers(operandType.get());
                if (auto* ptr = dynamic_cast<AST::PointerType*>(base)) {
                    exprTypes_[&node] = ptr->pointee->clone();
                    setResolvedExprType(node, exprTypes_[&node]);
                } else if (auto* arr = dynamic_cast<AST::ArrayType*>(base)) {
                    exprTypes_[&node] = arr->elementType->clone();
                    setResolvedExprType(node, exprTypes_[&node]);
                } else {
                    exprTypes_[&node] = std::move(operandType);
                    setResolvedExprType(node, exprTypes_[&node]);
                }
            }
            break;
        default:
            if (operandType) {
                exprTypes_[&node] = std::move(operandType);
                setResolvedExprType(node, exprTypes_[&node]);
            }
            break;
    }
}

void SemanticAnalyzer::visit(AST::CastExpr& node) {
    if (node.operand) node.operand->accept(*this);

    if (node.targetType) {
        exprTypes_[&node] = node.targetType->clone();
        setResolvedExprType(node, exprTypes_[&node]);
    }
}

void SemanticAnalyzer::visit(AST::CallExpr& node) {
    if (node.callee) node.callee->accept(*this);
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }

    if (auto* id = dynamic_cast<AST::Identifier*>(node.callee.get())) {
        Symbol* sym = currentScope_->lookup(id->name);
        if (sym && sym->isFunction && sym->type) {
            if (auto* funcType = dynamic_cast<AST::FunctionType*>(sym->type)) {
                size_t paramCount = funcType->parameterTypes.size();
                size_t argCount = node.arguments.size();

                for (size_t i = 0; i < argCount && i < paramCount; ++i) {
                    AST::Ptr<AST::Type> argType = getExprType(node.arguments[i].get());
                    AST::Type* paramType = funcType->parameterTypes[i].get();

                    if (argType && paramType) {
                        if (!canAssign(paramType, argType.get())) {
                            if (isIntegerType(stripQualifiers(paramType)) &&
                                (isPointerType(argType.get()) || isArrayType(argType.get()))) {
                                std::string argTypeStr;
                                if (auto* strLit = dynamic_cast<AST::StringLiteral*>(node.arguments[i].get())) {
                                    argTypeStr = "char[" + std::to_string(strLit->value.length() + 1) + "]";
                                } else {
                                    argTypeStr = typeToString(argType.get());
                                }
                                error(node.arguments[i]->line, node.arguments[i]->column,
                                      "incompatible pointer to integer conversion passing '" +
                                      argTypeStr + "' to parameter of type '" +
                                      typeToString(paramType) + "' [-Wint-conversion]");
                            } else if (isPointerType(paramType) && isIntegerType(argType.get())) {
                                error(node.arguments[i]->line, node.arguments[i]->column,
                                      "incompatible integer to pointer conversion passing '" +
                                      typeToString(argType.get()) + "' to parameter of type '" +
                                      typeToString(paramType) + "' [-Wint-conversion]");
                            }
                        }
                    }
                }

                if (funcType->returnType) {
                    exprTypes_[&node] = funcType->returnType->clone();
                    setResolvedExprType(node, exprTypes_[&node]);
                }
            }
        }
    }
}

void SemanticAnalyzer::visit(AST::MemberExpr& node) {
    if (node.object) node.object->accept(*this);
}

void SemanticAnalyzer::visit(AST::IndexExpr& node) {
    if (node.array) node.array->accept(*this);
    if (node.index) node.index->accept(*this);

    AST::Ptr<AST::Type> arrayType = getExprType(node.array.get());
    if (arrayType) {
        AST::Type* base = stripQualifiers(arrayType.get());
        if (auto* arr = dynamic_cast<AST::ArrayType*>(base)) {
            exprTypes_[&node] = arr->elementType->clone();
        } else if (auto* ptr = dynamic_cast<AST::PointerType*>(base)) {
            exprTypes_[&node] = ptr->pointee->clone();
        }
    }
}

void SemanticAnalyzer::visit(AST::TernaryExpr& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.thenExpr) node.thenExpr->accept(*this);
    if (node.elseExpr) node.elseExpr->accept(*this);

    AST::Ptr<AST::Type> thenType = getExprType(node.thenExpr.get());
    if (thenType) {
        exprTypes_[&node] = std::move(thenType);
    }
}

} // namespace cc1
