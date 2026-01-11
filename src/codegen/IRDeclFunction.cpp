#include <codegen/IRGenerator.hpp>

namespace cc1 {

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

} // namespace cc1
