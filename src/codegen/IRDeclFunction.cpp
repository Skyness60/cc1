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

    
    std::string params = "(";
    for (size_t i = 0; i < node.parameters.size(); ++i) {
        if (i > 0) params += ", ";
        auto& param = node.parameters[i];
        std::string paramType = typeToLLVM(param->type.get());

        
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
        
        
        if (!declaredFunctions_.count(node.name)) {
            std::string declLine = "declare " + returnType + " " + funcName + params + "\n";
            functionDeclarations_[node.name] = declLine;
            declaredFunctions_.insert(node.name);
        }

        
        IRSymbol sym;
        sym.name = node.name;
        sym.irName = funcName;
        sym.type = returnType + params;
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

    
    IRSymbol sym;
    sym.name = node.name;
    sym.irName = funcName;
    sym.type = returnType + params;
    sym.isFunction = true;
    defineSymbol(node.name, sym);
}

} 
