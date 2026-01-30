#include <preprocessor/Preprocessor.hpp>

#include <cctype>

namespace cc1 {

// EN: Evaluates a #if condition with defined() handling and pushes state.
// FR: Evalue une condition #if avec gestion de defined() et empile l etat.
void Preprocessor::visit(pp::IfDirective& dir) {
    if (!isActive()) {
        conditionalStack_.push(ConditionalState{false, false, false});
        return;
    }

    std::string condition = dir.condition;

    std::string expanded;
    size_t i = 0;
    while (i < condition.size()) {
        if (i + 7 <= condition.size() && condition.substr(i, 7) == "defined") {
            size_t j = i + 7;
            while (j < condition.size() && std::isspace(condition[j])) j++;

            bool hasParen = (j < condition.size() && condition[j] == '(');
            if (hasParen) j++;

            while (j < condition.size() && std::isspace(condition[j])) j++;

            std::string ident;
            while (j < condition.size() && (std::isalnum(condition[j]) || condition[j] == '_')) {
                ident += condition[j++];
            }

            while (j < condition.size() && std::isspace(condition[j])) j++;
            if (hasParen && j < condition.size() && condition[j] == ')') j++;

            expanded += macroTable_.isDefined(ident) ? "1" : "0";
            i = j;
        } else {
            expanded += condition[i++];
        }
    }

    expanded = macroExpander_->expand(expanded);
    long long result = exprEvaluator_->evaluate(expanded);
    bool conditionTrue = (result != 0);

    conditionalStack_.push(ConditionalState{conditionTrue, conditionTrue, false});
}

// EN: Handles #ifdef/#ifndef by checking macro existence and pushing state.
// FR: Traite #ifdef/#ifndef en testant l existence et empilant l etat.
void Preprocessor::visit(pp::IfdefDirective& dir) {
    if (!isActive()) {
        conditionalStack_.push(ConditionalState{false, false, false});
        return;
    }

    bool defined = macroTable_.isDefined(dir.name);
    bool conditionTrue = dir.isIfndef ? !defined : defined;
    conditionalStack_.push(ConditionalState{conditionTrue, conditionTrue, false});
}

// TODO(cc1) EN: Support the full defined operator grammar (macro names with
// expansion) and provide errors for malformed conditions. FR: Supporter la
// grammaire complete de defined et des erreurs pour conditions invalides.
} 
