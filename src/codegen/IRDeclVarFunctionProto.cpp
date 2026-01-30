#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits an IR function prototype from a function-typed var decl.
// FR: Genere un prototype IR depuis une declaration typee fonction.
void IRGenerator::emitVarDeclFunctionPrototype(AST::VarDecl& node, AST::FunctionType& funcType) {
    
    if (declaredFunctions_.count(node.name)) {
        
        if (!lookupSymbol(node.name)) {
            std::string returnType = typeToLLVM(funcType.returnType.get());
            std::string funcName = "@" + node.name;
            IRSymbol sym;
            sym.name = node.name;
            sym.irName = funcName;
            sym.type = returnType + "(";
            for (size_t i = 0; i < funcType.parameterTypes.size(); ++i) {
                if (i > 0) sym.type += ", ";
                sym.type += typeToLLVM(funcType.parameterTypes[i].get());
            }
            if (funcType.isVariadic) {
                if (!funcType.parameterTypes.empty()) sym.type += ", ";
                sym.type += "...";
            }
            sym.type += ")";
            sym.isFunction = true;
            defineSymbol(node.name, sym);
        }
        return;
    }

    
    std::string returnType = typeToLLVM(funcType.returnType.get());
    std::string funcName = "@" + node.name;

    
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

    
    std::string declLine = "declare " + returnType + " " + funcName + params + "\n";
    functionDeclarations_[node.name] = declLine;
    declaredFunctions_.insert(node.name);

    
    IRSymbol sym;
    sym.name = node.name;
    sym.irName = funcName;
    sym.type = returnType + params;
    sym.isFunction = true;
    defineSymbol(node.name, sym);
}

} 
