#include <semantics/SemanticAnalyzer.hpp>

namespace cc1 {

// ============================================================================
// Declaration Visitors
// ============================================================================

void SemanticAnalyzer::visit(AST::TranslationUnit& node) {
    inGlobalScope_ = true;
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
}

void SemanticAnalyzer::visit(AST::VarDecl& node) {
    // Check if this is a standalone enum definition (VarDecl with empty name and EnumType)
    if (node.name.empty()) {
        if (auto* enumType = dynamic_cast<AST::EnumType*>(node.type.get())) {
            processEnumType(enumType);
            return;
        }
    }
    
    // Check for void type - bad_semantic_0.c
    if (isVoidType(node.type.get())) {
        error(node.line, node.column, "variable has incomplete type 'void'");
        return;
    }
    
    // Check array size at file scope - bad_semantic_8.c
    if (inGlobalScope_) {
        if (auto* arrType = dynamic_cast<AST::ArrayType*>(stripQualifiers(node.type.get()))) {
            if (arrType->sizeExpr) {
                // Check if size is a constant expression
                if (!isConstantExpr(arrType->sizeExpr.get())) {
                    // Check if it's an identifier (VLA)
                    if (dynamic_cast<AST::Identifier*>(arrType->sizeExpr.get())) {
                        error(node.line, node.column, 
                              "variable length array declaration not allowed at file scope");
                        return;
                    }
                }
            }
        }
    }
    
    // Also check for enum types in variable declarations (e.g., "enum e { A = 1/0 } x;")
    if (auto* enumType = dynamic_cast<AST::EnumType*>(stripQualifiers(node.type.get()))) {
        processEnumType(enumType);
    }
    
    // Check for const variable without initialization - bad_semantic_1.c
    if (!inGlobalScope_ && isConstQualified(node.type.get()) && !node.initializer) {
        // Get the type name with const qualifier
        std::string typeName = "const " + typeToString(stripQualifiers(node.type.get()));
        warning(node.line, node.column,
                "default initialization of an object of type '" + typeName + 
                "' leaves the object uninitialized [-Wdefault-const-init-var-unsafe]");
    }
    
    // Check initializer type compatibility - bad_semantic_2.c, bad_semantic_3.c
    if (node.initializer) {
        node.initializer->accept(*this);
        AST::Ptr<AST::Type> initType = getExprType(node.initializer.get());
        
        if (initType) {
            AST::Type* destType = stripQualifiers(node.type.get());
            AST::Type* srcType = stripQualifiers(initType.get());
            
            // Check for pointer to integer conversion
            if (isIntegerType(destType) && (isPointerType(srcType) || isArrayType(srcType))) {
                // Get the initializer type string
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
            }
            // Check for integer to pointer conversion
            else if (isPointerType(destType) && isIntegerType(srcType)) {
                // Check if initializer is a dereference - suggest removing *
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
    
    // Register in symbol table
    Symbol sym;
    sym.name = node.name;
    sym.isConst = isConstQualified(node.type.get());
    sym.line = node.line;
    sym.column = node.column;
    
    // Check if this is actually a function prototype (VarDecl with FunctionType)
    if (dynamic_cast<AST::FunctionType*>(stripQualifiers(node.type.get()))) {
        sym.isFunction = true;
    }
    
    currentScope_->define(node.name, sym, node.type ? node.type->clone() : nullptr);
}

void SemanticAnalyzer::visit(AST::FunctionDecl& node) {
    // Register function in current scope
    Symbol sym;
    sym.name = node.name;
    sym.isFunction = true;
    sym.line = node.line;
    sym.column = node.column;
    
    // Build function type
    std::vector<AST::Ptr<AST::Type>> paramTypes;
    for (const auto& param : node.parameters) {
        if (param && param->type) {
            paramTypes.push_back(param->type->clone());
        }
    }
    auto funcType = AST::make<AST::FunctionType>(
        node.returnType ? node.returnType->clone() : nullptr,
        std::move(paramTypes),
        node.isVariadic,
        node.line, node.column
    );
    
    currentScope_->define(node.name, sym, std::move(funcType));
    
    // If this is a definition, analyze the body
    if (node.body) {
        currentFunction_ = &node;
        inGlobalScope_ = false;
        
        enterScope(true);
        
        // Add parameters to scope
        for (auto& param : node.parameters) {
            if (param) param->accept(*this);
        }
        
        // Analyze body
        node.body->accept(*this);
        
        exitScope();
        
        currentFunction_ = nullptr;
        inGlobalScope_ = true;
    }
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

void SemanticAnalyzer::visit(AST::StructDecl& node) {
    // Check for tag conflict - bad_semantic_5.c
    Symbol* existingTag = currentScope_->lookupTag(node.name);
    if (existingTag) {
        Symbol::TagKind expectedKind = node.isUnion ? Symbol::TagKind::Union : Symbol::TagKind::Struct;
        if (existingTag->tagKind != expectedKind) {
            error(node.line, node.column,
                  "use of '" + node.name + "' with tag type that does not match previous declaration");
            return;
        }
    }
    
    // Register struct tag
    Symbol sym;
    sym.name = node.name;
    sym.tagKind = node.isUnion ? Symbol::TagKind::Union : Symbol::TagKind::Struct;
    sym.line = node.line;
    sym.column = node.column;
    sym.structDecl = &node;  // Store pointer to struct declaration
    currentScope_->defineTag(node.name, sym);
    
    // Check members
    for (auto& member : node.members) {
        if (member) member->accept(*this);
    }
}

void SemanticAnalyzer::visit(AST::EnumDecl& node) {
    // Check for tag conflict - bad_semantic_5.c
    if (!node.name.empty()) {
        Symbol* existingTag = currentScope_->lookupTag(node.name);
        if (existingTag && existingTag->tagKind != Symbol::TagKind::Enum) {
            error(node.line, 1,  // Column 1 points to 'enum' keyword like clang
                  "use of '" + node.name + "' with tag type that does not match previous declaration");
            return;
        }
        
        // Register enum tag
        Symbol sym;
        sym.name = node.name;
        sym.tagKind = Symbol::TagKind::Enum;
        sym.line = node.line;
        sym.column = node.column;
        currentScope_->defineTag(node.name, sym);
    }
    
    // Process enumerators and check for division by zero - bad_semantic_7.c
    long long nextValue = 0;
    for (auto& enumerator : node.enumerators) {
        if (enumerator.value) {
            // Check if the value expression has division by zero
            if (hasDivisionByZero(enumerator.value.get())) {
                int col = enumerator.equalColumn > 0 ? enumerator.equalColumn : enumerator.column;
                error(enumerator.line, col,
                      "expression is not an integer constant expression");
                return;
            }
            
            // Evaluate the constant expression
            if (!evaluateConstantExpr(enumerator.value.get(), nextValue)) {
                int col = enumerator.equalColumn > 0 ? enumerator.equalColumn : enumerator.column;
                error(enumerator.line, col,
                      "expression is not an integer constant expression");
                return;
            }
        }
        
        // Register enumerator as constant
        Symbol sym;
        sym.name = enumerator.name;
        sym.isEnumConstant = true;
        sym.line = enumerator.line;
        sym.column = enumerator.column;
        currentScope_->define(enumerator.name, sym, AST::make<AST::PrimitiveType>(AST::PrimitiveKind::Int, 0, 0));
        
        nextValue++;
    }
}

void SemanticAnalyzer::visit(AST::TypedefDecl& node) {
    Symbol sym;
    sym.name = node.name;
    sym.isTypedef = true;
    sym.line = node.line;
    sym.column = node.column;
    currentScope_->define(node.name, sym, node.underlyingType ? node.underlyingType->clone() : nullptr);
}

} // namespace cc1
