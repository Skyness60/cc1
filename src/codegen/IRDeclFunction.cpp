#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for function declarations/definitions and prolog/epilog.
// FR: Genere l IR pour fonctions, prologue/epilogue inclus.
void IRGenerator::visit(AST::FunctionDecl& node) {
    if (debugInfo_) {
        initDebugMetadataIfNeeded();
    }

    std::string returnType = typeToLLVM(node.returnType.get());
    std::string funcName = "@" + node.name;
    
    // WORKAROUND: Detect malformed return type strings that contain "(" - these are
    // likely the result of the parser bug with complex function declarators.
    // LLVM can't handle these, so we use the innermost simple type.
    if (returnType.find('(') != std::string::npos) {
        // This returnType contains a function signature, which is invalid.
        // Extract just the simple type part. For "i32* (i32 (i32)*, i32)", we want "i32*"
        size_t parenPos = returnType.find('(');
        if (parenPos > 0) {
            returnType = returnType.substr(0, parenPos);
            // Trim trailing spaces
            while (!returnType.empty() && returnType.back() == ' ') {
                returnType.pop_back();
            }
        }
    }

    // Build two strings:
    // - params: for the function definition/declaration (includes names for definitions)
    // - paramSig: for the function type signature (types only, used for sym.type)
    std::string params = "(";
    std::string paramSig = "(";
    for (size_t i = 0; i < node.parameters.size(); ++i) {
        if (i > 0) {
            params += ", ";
            paramSig += ", ";
        }
        auto& param = node.parameters[i];
        std::string paramType = typeToLLVM(param->type.get());

        
        if (auto* arrType = dynamic_cast<AST::ArrayType*>(stripQualifiers(param->type.get()))) {
            paramType = typeToLLVM(arrType->elementType.get()) + "*";
        }

        
        // C "function parameter adjustment": function-typed parameters are adjusted to
        // pointers to function.
        if (auto* fnType = dynamic_cast<AST::FunctionType*>(stripQualifiers(param->type.get()))) {
            paramType = typeToLLVM(fnType) + "*";
        }

        params += paramType;
        paramSig += paramType;
        if (node.body) {
            params += " %" + std::to_string(i);
        }
    }
    if (node.isVariadic) {
        if (!node.parameters.empty()) {
            params += ", ";
            paramSig += ", ";
        }
        params += "...";
        paramSig += "...";
    }
    params += ")";
    paramSig += ")";

    if (!node.body) {
        
        
        if (!declaredFunctions_.count(node.name)) {
            std::string declLine = "declare " + returnType + " " + funcName + params + "\n";
            functionDeclarations_[node.name] = declLine;
            declaredFunctions_.insert(node.name);
        }

        // Use paramSig (types only) for the symbol type so that function references
        // have the proper LLVM function type without parameter names.
        IRSymbol sym;
        sym.name = node.name;
        sym.irName = funcName;
        sym.type = returnType + " " + paramSig;
        sym.isFunction = true;
        defineSymbol(node.name, sym);
        return;
    }

    
    if (definedFunctions_.count(node.name)) {
        
        return;
    }
    declaredFunctions_.insert(node.name);
    definedFunctions_.insert(node.name);

    
    functionDeclarations_[node.name] = "declare " + returnType + " " + funcName + params + "\n";

    
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

    
    currentFunction_ = &node;
    currentFunctionReturnType_ = returnType;
    inGlobalScope_ = false;
    tempCounter_ = node.parameters.size();  

    
    funcDefBuffer_ << "entry:\n";
    currentBuffer_ = &functionBuffer_;
    functionBuffer_.str("");

    
    enterScope();

    DebugLocGuard fnLoc(*this, node.line, node.column);

    
    for (size_t i = 0; i < node.parameters.size(); ++i) {
        auto& param = node.parameters[i];
        std::string paramType = typeToLLVM(param->type.get());

        
        if (auto* arrType = dynamic_cast<AST::ArrayType*>(stripQualifiers(param->type.get()))) {
            paramType = typeToLLVM(arrType->elementType.get()) + "*";
        }

        
        // C "function parameter adjustment": function-typed parameters are adjusted to
        // pointers to function.
        if (auto* fnType = dynamic_cast<AST::FunctionType*>(stripQualifiers(param->type.get()))) {
            paramType = typeToLLVM(fnType) + "*";
        }

        std::string ptrName = "%" + param->name + ".addr";

        emit(ptrName + " = alloca " + paramType);
        emit("store " + paramType + " %" + std::to_string(i) + ", " + paramType + "* " + ptrName);

        
        IRSymbol sym;
        sym.name = param->name;
        sym.irName = ptrName;
        sym.type = paramType;
        sym.isParameter = true;
        sym.paramIndex = static_cast<int>(i);
        defineSymbol(param->name, sym);
    }

    
    if (returnType != "void") {
        returnValuePtr_ = "%retval";
        emit(returnValuePtr_ + " = alloca " + returnType);
        
        
        if (node.name == "main") {
            emit("store " + returnType + " 0, " + returnType + "* " + returnValuePtr_);
        }
    }
    returnLabel_ = newLabel("return");

    
    node.body->accept(*this);

    
    emit("br label %" + returnLabel_);

    
    emitLabel(returnLabel_);
    if (returnType != "void") {
        std::string retTemp = newTemp();
        emit(retTemp + " = load " + returnType + ", " + returnType + "* " + returnValuePtr_);
        emit("ret " + returnType + " " + retTemp);
    } else {
        emit("ret void");
    }

    
    exitScope();

    
    funcDefBuffer_ << functionBuffer_.str();
    funcDefBuffer_ << "}\n";

    
    currentFunction_ = nullptr;
    inGlobalScope_ = true;
    currentBuffer_ = &globalBuffer_;
    currentSubprogramId_ = savedSubprogram;

    // Use paramSig (types only) for the symbol type so that function references
    // have the proper LLVM function type without parameter names.
    IRSymbol sym;
    sym.name = node.name;
    sym.irName = funcName;
    sym.type = returnType + " " + paramSig;
    sym.isFunction = true;
    defineSymbol(node.name, sym);
}

} 
