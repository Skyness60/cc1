#include <codegen/IRGenerator.hpp>

namespace cc1 {

void IRGenerator::emitVarDeclFunctionPrototype(AST::VarDecl& node, AST::FunctionType& funcType) {
    // Skip if already declared
    if (declaredFunctions_.count(node.name)) {
        // Already declared, just register symbol if not already there
        if (!lookupSymbol(node.name)) {
            std::string returnType = typeToLLVM(funcType.returnType.get());
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
    std::string returnType = typeToLLVM(funcType.returnType.get());
    std::string funcName = "@" + node.name;

    // Build parameter list
    std::string params = "(";
    for (size_t i = 0; i < funcType.parameterTypes.size(); ++i) {
        if (i > 0) params += ", ";
        params += typeToLLVM(funcType.parameterTypes[i].get());
    }
    if (funcType.isVariadic) {
        if (!funcType.parameterTypes.empty()) params += ", ";
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
}

} // namespace cc1
