#include <codegen/IRExprCallHelpers.hpp>

namespace cc1 {

// EN: Emits IR for function call expressions, including arg promotions/casts.
// FR: Genere l IR pour les appels, avec promotions/casts des arguments.
void IRGenerator::visit(AST::CallExpr& node) {
    using namespace ir_expr_call_helpers;

    
    std::string funcName;
    std::string funcType;
    std::string calleeIdName;
    if (auto* id = dynamic_cast<AST::Identifier*>(node.callee.get())) {
        calleeIdName = id->name;
        funcName = "@" + id->name;

        if (IRSymbol* sym = lookupSymbol(id->name)) {
            if (!sym->isFunction) {
                node.callee->accept(*this);
                funcType = lastValue_.type;
                funcName = lastValue_.name;
                calleeIdName.clear();
            }
        }
    } else {
        node.callee->accept(*this);
        IRValue funcVal = lastValue_;
        funcName = funcVal.name;
        funcType = funcVal.type;
    }

    
    if (!funcType.empty() && funcType.back() == '*') {
        funcType = funcType.substr(0, funcType.size() - 1);
    }

    
    std::vector<std::string> paramTypes;
    bool isVariadic = false;
    std::string protoReturnType;

    if (!calleeIdName.empty()) {
        auto it = functionDeclarations_.find(calleeIdName);
        if (it != functionDeclarations_.end()) {
            parseFunctionDeclParamTypes(it->second, paramTypes, isVariadic);

            
            const std::string& declLine = it->second;
            auto posDeclare = declLine.find("declare ");
            if (posDeclare != std::string::npos) {
                size_t start = posDeclare + std::string("declare ").size();
                size_t at = declLine.find(" @", start);
                if (at != std::string::npos && at > start) {
                    protoReturnType = declLine.substr(start, at - start);
                }
            }
        }
    }

    
    std::vector<IRValue> args;
    for (auto& arg : node.arguments) {
        arg->accept(*this);
        IRValue argVal = lastValue_;

        if (shouldLoadCallArgument(argVal)) {
            argVal = loadValue(argVal);
        }
        args.push_back(argVal);
    }

    
    const bool applyDefaultArgPromotions = isVariadic || (!calleeIdName.empty() && paramTypes.empty());

    
    std::string retType = "i32";
    if (!protoReturnType.empty()) {
        retType = protoReturnType;
    } else {
        auto pos = funcType.find('(');
        if (pos != std::string::npos) {
            retType = funcType.substr(0, pos);
        }
    }

    
    std::vector<std::pair<std::string, std::string>> finalArgs;
    finalArgs.reserve(args.size());

    for (size_t i = 0; i < args.size(); ++i) {
        std::string argType = args[i].type;
        std::string argName = args[i].name;

        
        if (i < paramTypes.size()) {
            const std::string& expected = paramTypes[i];
            if (expected != argType) {
                // EN: Maps integer LLVM types to bit-width for promotions.
                // FR: Mappe les types entiers LLVM vers la largeur en bits.
                auto getIntBits = [](const std::string& t) -> int {
                    if (t == "i8") return 8;
                    if (t == "i16") return 16;
                    if (t == "i32") return 32;
                    if (t == "i64") return 64;
                    return 0;
                };
                int fromBits = getIntBits(argType);
                int toBits = getIntBits(expected);
                if (fromBits > 0 && toBits > 0 && fromBits != toBits) {
                    std::string casted = newTemp();
                    bool isUnsigned = isUnsignedIntegralResolvedType(*node.arguments[i]);
                    if (fromBits < toBits) {
                        emit(casted + " = " + (isUnsigned ? "zext " : "sext ") + argType + " " + argName + " to " + expected);
                    } else {
                        emit(casted + " = trunc " + argType + " " + argName + " to " + expected);
                    }
                    argType = expected;
                    argName = casted;
                } else if (argType == "float" && expected == "double") {
                    std::string casted = newTemp();
                    emit(casted + " = fpext float " + argName + " to double");
                    argType = "double";
                    argName = casted;
                } else if (argType == "double" && expected == "float") {
                    std::string casted = newTemp();
                    emit(casted + " = fptrunc double " + argName + " to float");
                    argType = "float";
                    argName = casted;
                }
            }
        } else if (applyDefaultArgPromotions) {
            
            
            if (argType == "i1" || argType == "i8" || argType == "i16") {
                std::string casted = newTemp();
                bool isUnsigned = isUnsignedIntegralResolvedType(*node.arguments[i]);
                
                emit(casted + " = " + (isUnsigned ? "zext " : "sext ") + argType + " " + argName + " to i32");
                argType = "i32";
                argName = casted;
            } else if (argType == "float") {
                std::string casted = newTemp();
                emit(casted + " = fpext float " + argName + " to double");
                argType = "double";
                argName = casted;
            }
        }

        finalArgs.emplace_back(argType, argName);
    }

    std::string result;
    std::string callInst;
    if (retType == "void") {
        callInst = "call void " + funcName + "(";
    } else {
        result = newTemp();
        callInst = result + " = call " + retType + " " + funcName + "(";
    }

    for (size_t i = 0; i < finalArgs.size(); ++i) {
        if (i > 0) callInst += ", ";
        callInst += finalArgs[i].first + " " + finalArgs[i].second;
    }
    callInst += ")";

    emit(callInst);

    if (retType == "void") {
        lastValue_ = IRValue("0", "i32", false, true);
    } else {
        lastValue_ = IRValue(result, retType, false, false);
    }
}

} 
