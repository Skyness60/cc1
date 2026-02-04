#include <codegen/IRExprCallHelpers.hpp>

#include <iostream>

namespace cc1 {

// EN: Emits IR for function call expressions, including arg promotions/casts.
// FR: Genere l IR pour les appels, avec promotions/casts des arguments.
void IRGenerator::visit(AST::CallExpr& node) {
    using namespace ir_expr_call_helpers;

    
    std::string funcName;
    std::string funcType;      // May be "ret (args)" or "ret (args)*" for function pointers.
    std::string calleeIdName;
    if (auto* id = dynamic_cast<AST::Identifier*>(node.callee.get())) {
        calleeIdName = id->name;
        funcName = "@" + id->name;

        if (IRSymbol* sym = lookupSymbol(id->name)) {
            if (!sym->isFunction) {
                node.callee->accept(*this);
                IRValue funcVal = lastValue_;
                // Function pointer variables need to be loaded before calling.
                // The identifier gives us a pointer to the function pointer (e.g. i32 (i32)**),
                // so we need to load once to get the actual function pointer (e.g. i32 (i32)*).
                if (funcVal.isPointer && !funcVal.isConstant) {
                    funcVal = loadValue(funcVal);
                }
                funcType = funcVal.type;
                funcName = funcVal.name;
                calleeIdName.clear();
            }
        }
    } else {
        node.callee->accept(*this);
        IRValue funcVal = lastValue_;
        // For non-identifier callees (e.g. complex expressions returning function pointers),
        // also ensure we have the actual function pointer value.
        if (funcVal.isPointer && !funcVal.isConstant) {
            funcVal = loadValue(funcVal);
        }
        funcName = funcVal.name;
        funcType = funcVal.type;
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
    } else if (!funcType.empty() && funcType.find('(') != std::string::npos) {
        // EN: Fallback: if calling through a function pointer, parse parameter types from funcType.
        // FR: Fallback: si on appelle via pointeur de fonction, parser les types de paramètres depuis funcType.
        parseFunctionDeclParamTypes(funcType, paramTypes, isVariadic);
    }

    
    std::vector<IRValue> args;
    for (auto& arg : node.arguments) {
        arg->accept(*this);
        IRValue argVal = lastValue_;

        if (shouldLoadCallArgument(argVal)) {
            argVal = loadValue(argVal);
        }
        
        // When a function is used as an argument (e.g., passing a function to another function),
        // the type is the function signature (e.g., "i32 (i32)") but LLVM requires it to be
        // a function pointer type (e.g., "i32 (i32)*"). Detect this case and add the pointer marker.
        if (!argVal.type.empty() && argVal.type.back() != '*' && argVal.type.find('(') != std::string::npos) {
            argVal.type += "*";
        }
        
        args.push_back(argVal);
    }

    
    const bool applyDefaultArgPromotions = isVariadic || (!calleeIdName.empty() && paramTypes.empty());

    
    // Determine callee signature (without the trailing '*') and return type.
    //
    // IMPORTANT: For function pointers, the type is encoded as "ret (args)*".
    // We must NOT use the full encoding as a return type (otherwise we'd emit
    // "call i8* (i32, i8) ..." and treat the return as a pointer).
    std::string calleeSig;     // "ret (args)"
    std::string calleeType;    // same as calleeSig (used in emitted call)
    std::string retType = "i32";

    if (!protoReturnType.empty()) {
        retType = protoReturnType;

        // Build a precise signature from the declaration if available.
        calleeSig = retType + " (";
        for (size_t i = 0; i < paramTypes.size(); ++i) {
            if (i > 0) calleeSig += ", ";
            calleeSig += paramTypes[i];
        }
        if (isVariadic) {
            if (!paramTypes.empty()) calleeSig += ", ";
            calleeSig += "...";
        }
        calleeSig += ")";
        calleeType = calleeSig;
    } else if (!funcType.empty()) {
        calleeSig = funcType;
        if (!calleeSig.empty() && calleeSig.back() == '*') {
            calleeSig.pop_back(); // strip function-pointer marker only
        }
        calleeType = calleeSig;

        auto pos = calleeSig.find('(');
        if (pos != std::string::npos) {
            retType = calleeSig.substr(0, pos);
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

    // Emit call with an explicit return type.
    //
    // IMPORTANT: For function pointers, `calleeType` is the full signature
    // (e.g. "i8 (i32, i8)") and MUST NOT be used as the *return* type. The
    // return type is `retType` (e.g. "i8"). Otherwise we would incorrectly
    // treat scalar returns as pointers and generate invalid IR (e.g. store i8*
    // into i8).
    //
    // For variadic direct function calls, we MUST include the full signature 
    // (with "...") in the call instruction so LLVM knows how to handle varargs.
    // This applies only when calleeIdName is not empty (direct function call).
    bool useFullSignature = isVariadic && !calleeIdName.empty();
    
    if (retType == "void") {
        if (!calleeType.empty() && useFullSignature) {
            // For variadic direct functions, include the full signature
            callInst = "call " + calleeType + " " + funcName + "(";
        } else if (!calleeType.empty() && calleeIdName.empty()) {
            callInst = "call " + calleeType + " " + funcName + "(";
        } else {
            callInst = "call void " + funcName + "(";
        }
    } else {
        result = newTemp();
        if (!calleeType.empty() && useFullSignature) {
            // For variadic direct functions, include the full signature with return type
            callInst = result + " = call " + calleeType + " " + funcName + "(";
        } else if (!calleeType.empty() && calleeIdName.empty()) {
            callInst = result + " = call " + calleeType + " " + funcName + "(";
        } else {
            callInst = result + " = call " + retType + " " + funcName + "(";
        }
    }

    for (size_t i = 0; i < finalArgs.size(); ++i) {
        if (i > 0) callInst += ", ";
        callInst += finalArgs[i].first + " " + finalArgs[i].second;
    }
    callInst += ")";

    emit(callInst);

    // Temporary debug logging to diagnose incorrect return type inference (e.g. i8 vs i8*).
    // Use DebugLogger's enabled flag (set by the compiler driver) instead of a non-existent
    // codegen-local debug_mode_ flag.
    if (DebugLogger::instance().isEnabled()) {
        DebugLogger::instance().log(std::string("[codegen][call] funcName=") + funcName +
                                    " funcType=" + funcType +
                                    " calleeSig=" + calleeSig +
                                    " calleeType=" + calleeType +
                                    " retType=" + retType +
                                    " emitted=" + callInst);
    }

    if (retType == "void") {
        lastValue_ = IRValue("0", "i32", false, true);
    } else {
        lastValue_ = IRValue(result, retType, false, false);
    }
}

} 
