#include <codegen/IRExprCallHelpers.hpp>

namespace cc1 {

// ============================================================================
// Function Call
// ============================================================================

void IRGenerator::visit(AST::CallExpr& node) {
    using namespace ir_expr_call_helpers;

    // Evaluate function expression
    std::string funcName;
    std::string funcType;
    std::string calleeIdName;
    if (auto* id = dynamic_cast<AST::Identifier*>(node.callee.get())) {
        calleeIdName = id->name;
        funcName = "@" + id->name;
        node.callee->accept(*this);
        funcType = lastValue_.type;
    } else {
        node.callee->accept(*this);
        IRValue funcVal = lastValue_;
        funcName = funcVal.name;
        funcType = funcVal.type;
    }

    // Remove pointer from function type
    if (!funcType.empty() && funcType.back() == '*') {
        funcType = funcType.substr(0, funcType.size() - 1);
    }

    // Determine parameter types from known declarations if available
    std::vector<std::string> paramTypes;
    bool isVariadic = false;

    if (!calleeIdName.empty()) {
        auto it = functionDeclarations_.find(calleeIdName);
        if (it != functionDeclarations_.end()) {
            parseFunctionDeclParamTypes(it->second, paramTypes, isVariadic);
        }
    }

    // Evaluate arguments
    std::vector<IRValue> args;
    for (auto& arg : node.arguments) {
        arg->accept(*this);
        IRValue argVal = lastValue_;

        if (shouldLoadCallArgument(argVal)) {
            argVal = loadValue(argVal);
        }
        args.push_back(argVal);
    }

    // Determine return type
    std::string retType = "i32";
    auto pos = funcType.find('(');
    if (pos != std::string::npos) {
        retType = funcType.substr(0, pos);
    }

    // Prepare call instruction
    std::string result = newTemp();
    std::string callInst;

    if (retType == "void") {
        callInst = "call void " + funcName + "(";
    } else {
        callInst = result + " = call " + retType + " " + funcName + "(";
    }

    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) callInst += ", ";

        std::string argType = args[i].type;
        std::string argName = args[i].name;

        // If we have declared parameter types, apply integer promotions/casts.
        if (i < paramTypes.size()) {
            const std::string& expected = paramTypes[i];
            if (expected != argType) {
                // Only handle integer widths here.
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
                }
            }
        } else if (isVariadic) {
            // Default argument promotions for variadic functions.
            // Promote small integer types to i32.
            if (argType == "i8" || argType == "i16") {
                std::string casted = newTemp();
                bool isUnsigned = isUnsignedIntegralResolvedType(*node.arguments[i]);
                emit(casted + " = " + (isUnsigned ? "zext " : "sext ") + argType + " " + argName + " to i32");
                argType = "i32";
                argName = casted;
            }
        }

        callInst += argType + " " + argName;
    }
    callInst += ")";

    emit(callInst);

    if (retType == "void") {
        lastValue_ = IRValue("0", "i32", false, true);
    } else {
        lastValue_ = IRValue(result, retType, false, false);
    }
}

} // namespace cc1
