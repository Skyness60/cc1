#include <codegen/IRGenerator.hpp>

namespace cc1 {

// EN: Emits IR for casts, applying numeric and pointer conversions.
// FR: Genere l IR pour les casts, conversions numeriques/pointeurs.
void IRGenerator::visit(AST::CastExpr& node) {
    
    node.operand->accept(*this);
    IRValue exprVal = loadValue(lastValue_);

    
    std::string targetType = typeToLLVM(node.targetType.get());

    
    if (exprVal.type == targetType) {
        lastValue_ = exprVal;
        return;
    }

    std::string result = newTemp();

    // EN: Checks if a type string is an integer scalar like i32.
    // FR: Verifie si un type est un scalaire entier comme i32.
    auto isIntScalar = [](const std::string& t) -> bool {
        if (t.size() < 2) return false;
        if (t[0] != 'i') return false;
        for (size_t k = 1; k < t.size(); ++k) {
            if (t[k] < '0' || t[k] > '9') return false;
        }
        return true;
    };

    
    if (isIntScalar(exprVal.type) && isIntScalar(targetType)) {
        int fromBits = std::stoi(exprVal.type.substr(1));
        int toBits = std::stoi(targetType.substr(1));

        if (fromBits < toBits) {
            
            emit(result + " = sext " + exprVal.type + " " + exprVal.name + " to " + targetType);
        } else {
            
            emit(result + " = trunc " + exprVal.type + " " + exprVal.name + " to " + targetType);
        }
    }
    
    else if ((exprVal.type == "float" || exprVal.type == "double") && (targetType == "float" || targetType == "double")) {
        if (exprVal.type == "float" && targetType == "double") {
            emit(result + " = fpext float " + exprVal.name + " to double");
        } else {
            emit(result + " = fptrunc double " + exprVal.name + " to float");
        }
    }
    
    else if (exprVal.type[0] == 'i' && (targetType == "float" || targetType == "double")) {
        emit(result + " = sitofp " + exprVal.type + " " + exprVal.name + " to " + targetType);
    }
    
    else if ((exprVal.type == "float" || exprVal.type == "double") && targetType[0] == 'i') {
        emit(result + " = fptosi " + exprVal.type + " " + exprVal.name + " to " + targetType);
    }
    
    else if (isIntScalar(exprVal.type) && !targetType.empty() && targetType.back() == '*') {
        emit(result + " = inttoptr " + exprVal.type + " " + exprVal.name + " to " + targetType);
    }
    
    else if (!exprVal.type.empty() && exprVal.type.back() == '*' && isIntScalar(targetType)) {
        emit(result + " = ptrtoint " + exprVal.type + " " + exprVal.name + " to " + targetType);
    }
    
    else if (exprVal.type.back() == '*' && targetType.back() == '*') {
        emit(result + " = bitcast " + exprVal.type + " " + exprVal.name + " to " + targetType);
    } else {
        
        lastValue_ = exprVal;
        return;
    }

    lastValue_ = IRValue(result, targetType, false, false);
}

// EN: Emits IR for sizeof by materializing the computed size.
// FR: Genere l IR pour sizeof en materialisant la taille.
void IRGenerator::visit(AST::SizeofExpr& node) {
    int size = 0;

    if (node.operand) {
        
        if (node.operand->resolvedType) {
            size = getTypeSize(node.operand->resolvedType.get());
        }
    } else if (node.targetType) {
        
        size = getTypeSize(node.targetType.get());
    }

    lastValue_ = IRValue(std::to_string(size), "i32", false, true);
}

} 
