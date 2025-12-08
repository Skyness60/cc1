#include <codegen/IRGenerator.hpp>

namespace cc1 {

// ============================================================================
// Translation Unit
// ============================================================================

void IRGenerator::visit(AST::TranslationUnit& node) {
    inGlobalScope_ = true;
    
    // First pass: collect typedef definitions ONLY
    for (auto& decl : node.declarations) {
        // Check for TypedefDecl (if used by semantic analysis)
        if (auto* typedefDecl = dynamic_cast<AST::TypedefDecl*>(decl.get())) {
            if (typedefDecl->underlyingType) {
                typedefMap_[typedefDecl->name] = typedefDecl->underlyingType.get();
            }
        }
        // Also check for VarDecl with Typedef storage class (used by parser)
        else if (auto* varDecl = dynamic_cast<AST::VarDecl*>(decl.get())) {
            if (varDecl->storageClass == AST::StorageClass::Typedef && varDecl->type) {
                typedefMap_[varDecl->name] = varDecl->type.get();
            }
        }
    }
    
    // Second pass: collect struct definitions
    for (auto& decl : node.declarations) {
        if (auto* structDecl = dynamic_cast<AST::StructDecl*>(decl.get())) {
            if (!structDecl->members.empty()) {
                StructLayout layout = computeStructLayout(structDecl);
                structLayouts_[structDecl->name] = layout;
                emitGlobal(layout.llvmType);
            }
        }
    }
    
    // Third pass: process declarations
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
}

// ============================================================================
// Variable Declaration
// ============================================================================

void IRGenerator::visit(AST::VarDecl& node) {
    if (node.name.empty()) {
        // Anonymous declaration (e.g., standalone enum definition)
        if (auto* enumType = dynamic_cast<AST::EnumType*>(stripQualifiers(node.type.get()))) {
            // Process enum values
            long long nextValue = 0;
            for (const auto& enumerator : enumType->enumerators) {
                if (enumerator.value) {
                    evaluateConstantExpr(enumerator.value.get(), nextValue);
                }
                enumValues_[enumerator.name] = nextValue;
                nextValue++;
            }
        }
        return;
    }
    
    // Check if this is a function prototype (VarDecl with FunctionType)
    if (auto* funcType = dynamic_cast<AST::FunctionType*>(stripQualifiers(node.type.get()))) {
        // Skip if already declared
        if (declaredFunctions_.count(node.name)) {
            // Already declared, just register symbol if not already there
            if (!lookupSymbol(node.name)) {
                std::string returnType = typeToLLVM(funcType->returnType.get());
                std::string funcName = "@" + node.name;
                IRSymbol sym;
                sym.name = node.name;
                sym.irName = funcName;
                sym.type = returnType;
                sym.isFunction = true;
                defineSymbol(node.name, sym);
            }
            return;
        }
        
        // Emit as function declaration
        std::string returnType = typeToLLVM(funcType->returnType.get());
        std::string funcName = "@" + node.name;
        
        // Build parameter list
        std::string params = "(";
        for (size_t i = 0; i < funcType->parameterTypes.size(); ++i) {
            if (i > 0) params += ", ";
            params += typeToLLVM(funcType->parameterTypes[i].get());
        }
        if (funcType->isVariadic) {
            if (!funcType->parameterTypes.empty()) params += ", ";
            params += "...";
        }
        params += ")";
        
        // Store for later emission (only if not defined)
        std::string declLine = "declare " + returnType + " " + funcName + params + "\n";
        functionDeclarations_[node.name] = declLine;
        declaredFunctions_.insert(node.name);
        
        // Register symbol
        IRSymbol sym;
        sym.name = node.name;
        sym.irName = funcName;
        sym.type = returnType;
        sym.isFunction = true;
        defineSymbol(node.name, sym);
        return;
    }
    
    std::string llvmType = typeToLLVM(node.type.get());
    
    if (inGlobalScope_) {
        // Global variable
        std::string globalName = "@" + node.name;
        
        // Handle extern declarations
        if (node.storageClass == AST::StorageClass::Extern) {
            // Skip if already declared
            if (!declaredGlobals_.empty() && declaredGlobals_.count(node.name)) {
                return;
            }
            
            // Extern variable - just declare it
            emitGlobal(globalName + " = external global " + llvmType);
            declaredGlobals_.insert(node.name);
            
            IRSymbol sym;
            sym.name = node.name;
            sym.irName = globalName;
            sym.type = llvmType;
            sym.isGlobal = true;
            defineSymbol(node.name, sym);
            return;
        }
        
        // Skip if already declared (definition)
        if (!declaredGlobals_.empty() && declaredGlobals_.count(node.name)) {
            return;
        }
        
        // Determine initial value
        std::string initValue;
        if (node.initializer) {
            long long constVal;
            if (evaluateConstantExpr(node.initializer.get(), constVal)) {
                initValue = std::to_string(constVal);
            } else if (auto* strLit = dynamic_cast<AST::StringLiteral*>(node.initializer.get())) {
                // String literal initializer for char arrays
                std::string strName = newGlobalString(strLit->value);
                // This is tricky - need to handle differently
                initValue = getDefaultValue(node.type.get());
            } else {
                initValue = getDefaultValue(node.type.get());
            }
        } else {
            initValue = getDefaultValue(node.type.get());
        }
        
        // Check for static/extern
        std::string linkage = "dso_local global";
        
        emitGlobal(globalName + " = " + linkage + " " + llvmType + " " + initValue);
        declaredGlobals_.insert(node.name);
        
        // Register symbol
        // Global variables are accessed via their address (@name is a pointer to the value)
        IRSymbol sym;
        sym.name = node.name;
        sym.irName = globalName;
        sym.type = llvmType;  // Store the value type, not pointer type
        sym.isGlobal = true;
        defineSymbol(node.name, sym);
        
    } else {
        // Local variable - allocate on stack
        // BUT: static local variables are like globals with unique names
        if (node.storageClass == AST::StorageClass::Static) {
            // Static local variable - emit as global with unique name
            std::string globalName = "@" + currentFunction_->name + "." + node.name + "." + std::to_string(staticVarCounter_++);
            
            std::string initValue;
            if (node.initializer) {
                long long constVal;
                if (evaluateConstantExpr(node.initializer.get(), constVal)) {
                    initValue = std::to_string(constVal);
                } else {
                    initValue = getDefaultValue(node.type.get());
                }
            } else {
                initValue = getDefaultValue(node.type.get());
            }
            
            emitGlobal(globalName + " = internal global " + llvmType + " " + initValue);
            
            // Register as global-like symbol
            IRSymbol sym;
            sym.name = node.name;
            sym.irName = globalName;
            sym.type = llvmType;
            sym.isGlobal = true;
            defineSymbol(node.name, sym);
            return;
        }
        
        // Use a unique suffix to handle variables with same name in different scopes
        std::string ptrName = "%" + node.name + ".addr" + std::to_string(tempCounter_++);
        
        emit(ptrName + " = alloca " + llvmType);
        
        // Initialize if present
        if (node.initializer) {
            node.initializer->accept(*this);
            IRValue initVal = lastValue_;
            
            std::string valReg = initVal.name;
            std::string valType = initVal.type;
            
            // If initializer is a pointer (from load), we need to load it first
            if (initVal.isPointer && !initVal.isConstant) {
                valReg = newTemp();
                valType = initVal.derefType();
                emit(valReg + " = load " + valType + ", " + initVal.type + " " + initVal.name);
            }
            
            // Convert types if needed
            if (valType != llvmType) {
                std::string convertedReg = newTemp();
                // Float/double conversions
                if (valType == "double" && llvmType == "float") {
                    emit(convertedReg + " = fptrunc double " + valReg + " to float");
                    valReg = convertedReg;
                    valType = "float";
                } else if (valType == "float" && llvmType == "double") {
                    emit(convertedReg + " = fpext float " + valReg + " to double");
                    valReg = convertedReg;
                    valType = "double";
                }
                // Integer to float
                else if ((valType == "i32" || valType == "i16" || valType == "i8" || valType == "i64") &&
                         (llvmType == "float" || llvmType == "double")) {
                    emit(convertedReg + " = sitofp " + valType + " " + valReg + " to " + llvmType);
                    valReg = convertedReg;
                    valType = llvmType;
                }
                // Float to integer
                else if ((llvmType == "i32" || llvmType == "i16" || llvmType == "i8" || llvmType == "i64") &&
                         (valType == "float" || valType == "double")) {
                    emit(convertedReg + " = fptosi " + valType + " " + valReg + " to " + llvmType);
                    valReg = convertedReg;
                    valType = llvmType;
                }
                // Integer size conversions
                else {
                    auto getIntSize = [](const std::string& t) -> int {
                        if (t == "i8") return 8;
                        if (t == "i16") return 16;
                        if (t == "i32") return 32;
                        if (t == "i64") return 64;
                        return 0;
                    };
                    int srcSize = getIntSize(valType);
                    int dstSize = getIntSize(llvmType);
                    if (srcSize > 0 && dstSize > 0) {
                        if (srcSize < dstSize) {
                            emit(convertedReg + " = sext " + valType + " " + valReg + " to " + llvmType);
                        } else {
                            emit(convertedReg + " = trunc " + valType + " " + valReg + " to " + llvmType);
                        }
                        valReg = convertedReg;
                        valType = llvmType;
                    }
                }
            }
            
            emit("store " + llvmType + " " + valReg + ", " + llvmType + "* " + ptrName);
        }
        
        // Register symbol
        IRSymbol sym;
        sym.name = node.name;
        sym.irName = ptrName;
        sym.type = llvmType;
        defineSymbol(node.name, sym);
    }
}

// ============================================================================
// Function Declaration
// ============================================================================

void IRGenerator::visit(AST::FunctionDecl& node) {
    std::string returnType = typeToLLVM(node.returnType.get());
    std::string funcName = "@" + node.name;
    
    // Build parameter list
    std::string params = "(";
    for (size_t i = 0; i < node.parameters.size(); ++i) {
        if (i > 0) params += ", ";
        auto& param = node.parameters[i];
        std::string paramType = typeToLLVM(param->type.get());
        
        // Arrays decay to pointers in function parameters
        if (auto* arrType = dynamic_cast<AST::ArrayType*>(stripQualifiers(param->type.get()))) {
            paramType = typeToLLVM(arrType->elementType.get()) + "*";
        }
        
        params += paramType;
        if (node.body) {
            params += " %" + std::to_string(i);
        }
    }
    if (node.isVariadic) {
        if (!node.parameters.empty()) params += ", ";
        params += "...";
    }
    params += ")";
    
    if (!node.body) {
        // Function declaration (prototype)
        // Store declaration for later (only emit if not defined)
        if (!declaredFunctions_.count(node.name)) {
            std::string declLine = "declare " + returnType + " " + funcName + params + "\n";
            functionDeclarations_[node.name] = declLine;
            declaredFunctions_.insert(node.name);
        }
        
        // Register symbol
        IRSymbol sym;
        sym.name = node.name;
        sym.irName = funcName;
        sym.type = returnType;
        sym.isFunction = true;
        defineSymbol(node.name, sym);
        return;
    }
    
    // Function definition - skip if already defined (but not just declared)
    if (definedFunctions_.count(node.name)) {
        // Already defined, skip
        return;
    }
    declaredFunctions_.insert(node.name);
    definedFunctions_.insert(node.name);
    
    // Function definition
    funcDefBuffer_ << "\ndefine dso_local " << returnType << " " << funcName << params << " {\n";
    
    // Save state and switch to function context
    currentFunction_ = &node;
    currentFunctionReturnType_ = returnType;
    inGlobalScope_ = false;
    tempCounter_ = node.parameters.size();  // Start after param registers
    
    // Create entry block
    funcDefBuffer_ << "entry:\n";
    currentBuffer_ = &functionBuffer_;
    functionBuffer_.str("");
    
    // Enter function scope
    enterScope();
    
    // Allocate space for parameters and copy
    for (size_t i = 0; i < node.parameters.size(); ++i) {
        auto& param = node.parameters[i];
        std::string paramType = typeToLLVM(param->type.get());
        
        // Arrays decay to pointers
        if (auto* arrType = dynamic_cast<AST::ArrayType*>(stripQualifiers(param->type.get()))) {
            paramType = typeToLLVM(arrType->elementType.get()) + "*";
        }
        
        std::string ptrName = "%" + param->name + ".addr";
        
        emit(ptrName + " = alloca " + paramType);
        emit("store " + paramType + " %" + std::to_string(i) + ", " + paramType + "* " + ptrName);
        
        // Register parameter symbol
        IRSymbol sym;
        sym.name = param->name;
        sym.irName = ptrName;
        sym.type = paramType;
        sym.isParameter = true;
        sym.paramIndex = static_cast<int>(i);
        defineSymbol(param->name, sym);
    }
    
    // Allocate return value if non-void
    if (returnType != "void") {
        returnValuePtr_ = "%retval";
        emit(returnValuePtr_ + " = alloca " + returnType);
        // Initialize return value to 0 for main() (C99+ behavior)
        // This ensures main returns 0 if no explicit return is provided
        if (node.name == "main") {
            emit("store " + returnType + " 0, " + returnType + "* " + returnValuePtr_);
        }
    }
    returnLabel_ = newLabel("return");
    
    // Process function body
    node.body->accept(*this);
    
    // Emit branch to return if we haven't returned yet
    emit("br label %" + returnLabel_);
    
    // Emit return block
    emitLabel(returnLabel_);
    if (returnType != "void") {
        std::string retTemp = newTemp();
        emit(retTemp + " = load " + returnType + ", " + returnType + "* " + returnValuePtr_);
        emit("ret " + returnType + " " + retTemp);
    } else {
        emit("ret void");
    }
    
    // Exit function scope
    exitScope();
    
    // Write function buffer to function definition buffer
    funcDefBuffer_ << functionBuffer_.str();
    funcDefBuffer_ << "}\n";
    
    // Restore state
    currentFunction_ = nullptr;
    inGlobalScope_ = true;
    currentBuffer_ = &globalBuffer_;
    
    // Register function symbol
    IRSymbol sym;
    sym.name = node.name;
    sym.irName = funcName;
    sym.type = returnType;
    sym.isFunction = true;
    defineSymbol(node.name, sym);
}

// ============================================================================
// Parameter Declaration
// ============================================================================

void IRGenerator::visit(AST::ParamDecl& /*node*/) {
    // Parameters are handled in FunctionDecl
}

// ============================================================================
// Struct Declaration
// ============================================================================

void IRGenerator::visit(AST::StructDecl& node) {
    if (node.members.empty()) {
        // Forward declaration - nothing to emit
        return;
    }
    
    // Check if already processed
    if (structLayouts_.find(node.name) != structLayouts_.end()) {
        return;
    }
    
    // Compute and cache layout
    StructLayout layout = computeStructLayout(&node);
    structLayouts_[node.name] = layout;
    
    // Emit type definition if not already done
    emitGlobal(layout.llvmType);
}

// ============================================================================
// Enum Declaration
// ============================================================================

void IRGenerator::visit(AST::EnumDecl& node) {
    // Process enumerators
    long long nextValue = 0;
    for (auto& enumerator : node.enumerators) {
        if (enumerator.value) {
            evaluateConstantExpr(enumerator.value.get(), nextValue);
        }
        enumValues_[enumerator.name] = nextValue;
        nextValue++;
    }
}

// ============================================================================
// Typedef Declaration
// ============================================================================

void IRGenerator::visit(AST::TypedefDecl& /*node*/) {
    // Typedefs don't generate IR - they're resolved at compile time
    // The type system handles the aliasing
}

} // namespace cc1
