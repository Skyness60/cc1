#include <codegen/IRGenerator.hpp>

#include <cstdint>
#include <cstring>
#include <iomanip>

namespace cc1 {

static std::string formatLLVMFloatConstant(float value) {
    // LLVM textual IR accepts float constants in a 64-bit hex form (same style clang emits).
    // We encode the rounded float value as a double bit-pattern.
    double asDouble = static_cast<double>(value);
    uint64_t bits = 0;
    static_assert(sizeof(bits) == sizeof(asDouble), "unexpected double size");
    std::memcpy(&bits, &asDouble, sizeof(bits));

    std::ostringstream oss;
    oss << "0x" << std::hex << std::nouppercase << std::setw(16) << std::setfill('0') << bits;
    return oss.str();
}

// ============================================================================
// Translation Unit
// ============================================================================

void IRGenerator::visit(AST::TranslationUnit& snode) {
    inGlobalScope_ = true;
    
    // First pass: collect typedef definitions and assign names to anonymous structs
    for (auto& decl : snode.declarations) {
        // Check for TypedefDecl
        if (auto* typedefDecl = dynamic_cast<AST::TypedefDecl*>(decl.get())) {
            if (typedefDecl->underlyingType) {
                typedefMap_[typedefDecl->name] = typedefDecl->underlyingType.get();
                
                // If the underlying type is an anonymous struct, assign the typedef name as struct name
                if (auto* structType = dynamic_cast<AST::StructType*>(typedefDecl->underlyingType.get())) {
                    if (structType->name.empty()) {
                        structType->name = typedefDecl->name;
                    }
                }
            }
        }
        // Also check for VarDecl with Typedef storage class (used by parser)
        else if (auto* varDecl = dynamic_cast<AST::VarDecl*>(decl.get())) {
            if (varDecl->storageClass == AST::StorageClass::Typedef && varDecl->type) {
                typedefMap_[varDecl->name] = varDecl->type.get();
                
                // If the underlying type is an anonymous struct, assign the typedef name as struct name
                if (auto* structType = dynamic_cast<AST::StructType*>(varDecl->type.get())) {
                    if (structType->name.empty()) {
                        structType->name = varDecl->name;
                    }
                }
            }
        }
    }
    
    // Second pass: collect struct definitions
    for (auto& decl : snode.declarations) {
        if (auto* structDecl = dynamic_cast<AST::StructDecl*>(decl.get())) {
            if (!structDecl->members.empty()) {
                StructLayout layout = computeStructLayout(structDecl);
                structLayouts_[structDecl->name] = layout;

                // Ensure the struct type definition is emitted exactly once via getIR().
                if (!namedStructDefs_.count(structDecl->name)) {
                    namedStructDefs_[structDecl->name] = {layout.llvmType, structDecl->declaredType.get()};
                }
            }
        }
    }
    
    // Third pass: process declarations
    for (auto& decl : snode.declarations) {
        if (decl) decl->accept(*this);
    }
}

// ============================================================================
// Variable Declaration
// ============================================================================

void IRGenerator::visit(AST::VarDecl& node) {
    // Skip typedef declarations - they don't generate code
    if (node.storageClass == AST::StorageClass::Typedef) {
        return;
    }
    
    if (node.name.empty()) {
        // Anonymous declaration (e.g., standalone enum definition)
        if (auto* enumType = dynamic_cast<AST::EnumType*>(stripQualifiers(node.type.get()))) {
            // Use pre-computed enum values from semantic analysis
            for (const auto& enumerator : enumType->enumerators) {
                enumValues_[enumerator.name] = enumerator.computedValue;
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
            if (llvmType == "float" || llvmType == "double") {
                double fpVal;
                if (evaluateConstantFloatExpr(node.initializer.get(), fpVal)) {
                    std::ostringstream oss;
                    if (llvmType == "float") {
                        initValue = formatLLVMFloatConstant(static_cast<float>(fpVal));
                    } else {
                        oss.setf(std::ios::scientific);
                        oss << std::setprecision(17) << fpVal;
                        initValue = oss.str();
                    }
                } else {
                    initValue = getDefaultValue(node.type.get());
                }
            } else {
                long long constVal;
                if (evaluateConstantExpr(node.initializer.get(), constVal)) {
                    initValue = std::to_string(constVal);
                } else if (auto* strLit = dynamic_cast<AST::StringLiteral*>(node.initializer.get())) {
                    // String literal initializer for char arrays
                    std::string strName = newGlobalString(strLit->value);
                    // This is tricky - need to handle differently
                    initValue = getDefaultValue(node.type.get());
                } else if (auto* initList = dynamic_cast<AST::InitializerList*>(node.initializer.get())) {
                    // InitializerList - try to generate the proper initializer
                    // For arrays of structs, use flattening (works with or without bitfields)
                    bool isArrayOfStruct = false;
                    if (auto* arrayType = dynamic_cast<AST::ArrayType*>(node.type.get())) {
                        auto* elemType = stripQualifiers(arrayType->elementType.get());
                        auto* structType = dynamic_cast<AST::StructType*>(elemType);
                        isArrayOfStruct = (structType != nullptr);
                    }
                    (void)isArrayOfStruct;
                    
                    // Use generateInitializerValue for all array types
                    initValue = generateInitializerValue(node.type.get(), initList);
                } else {
                    initValue = getDefaultValue(node.type.get());
                }
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
                if (llvmType == "float" || llvmType == "double") {
                    double fpVal;
                    if (evaluateConstantFloatExpr(node.initializer.get(), fpVal)) {
                        std::ostringstream oss;
                        if (llvmType == "float") {
                            initValue = formatLLVMFloatConstant(static_cast<float>(fpVal));
                        } else {
                            oss.setf(std::ios::scientific);
                            oss << std::setprecision(17) << fpVal;
                            initValue = oss.str();
                        }
                    } else {
                        initValue = getDefaultValue(node.type.get());
                    }
                } else {
                    long long constVal;
                    if (evaluateConstantExpr(node.initializer.get(), constVal)) {
                        initValue = std::to_string(constVal);
                    } else {
                        initValue = getDefaultValue(node.type.get());
                    }
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
            std::string valReg;
            std::string valType = llvmType;
            
            // Special handling for InitializerList on array/struct types
            if (auto* initList = dynamic_cast<AST::InitializerList*>(node.initializer.get())) {
                // Generate the initializer value directly
                std::string initValue = generateInitializerValue(node.type.get(), initList);
                valReg = newTemp();
                // For arrays and structs, we need a way to initialize them on the stack
                // Use undef and insert the values, OR use a global constant and copy it
                // For now, generate the initialization directly as the value
                emit("store " + llvmType + " " + initValue + ", " + llvmType + "* " + ptrName);
                
                // Register symbol
                IRSymbol sym;
                sym.name = node.name;
                sym.irName = ptrName;
                sym.type = llvmType;
                defineSymbol(node.name, sym);
                return;
            }
            
            // For non-initializer-list expressions
            node.initializer->accept(*this);
            IRValue initVal = lastValue_;
            
            valReg = initVal.name;
            valType = initVal.type;
            
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
    if (debugInfo_) {
        initDebugMetadataIfNeeded();
    }

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
    int savedSubprogram = currentSubprogramId_;
    if (debugInfo_) {
        int subTyId = newDebugMetaId();
        debugMetaBuffer_ << "!" << subTyId << " = !DISubroutineType(types: !{null})\n";

        currentSubprogramId_ = newDebugMetaId();
        int line = node.line > 0 ? node.line : 1;
        debugMetaBuffer_ << "!" << currentSubprogramId_
                         << " = distinct !DISubprogram(name: \"" << node.name << "\", scope: !" << diFileId_
                         << ", file: !" << diFileId_ << ", line: " << line
                         << ", type: !" << subTyId << ", unit: !" << diCompileUnitId_
                         << ", spFlags: DISPFlagDefinition)\n";

        funcDefBuffer_ << "\ndefine dso_local " << returnType << " " << funcName << params
                      << " !dbg !" << currentSubprogramId_ << " {\n";
    } else {
        funcDefBuffer_ << "\ndefine dso_local " << returnType << " " << funcName << params << " {\n";
    }
    
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

    DebugLocGuard fnLoc(*this, node.line, node.column);
    
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
    currentSubprogramId_ = savedSubprogram;
    
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
    // Use pre-computed enum values from semantic analysis
    for (auto& enumerator : node.enumerators) {
        enumValues_[enumerator.name] = enumerator.computedValue;
    }
}

// ============================================================================
// Typedef Declaration
// ============================================================================



} // namespace cc1
