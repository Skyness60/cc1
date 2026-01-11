#include <preprocessor/Preprocessor.hpp>

#include <cctype>

namespace cc1 {

void Preprocessor::visit(pp::ElifDirective& dir) {
    if (conditionalStack_.empty()) {
        error("#elif without #if");
        return;
    }

    ConditionalState& state = conditionalStack_.top();

    if (state.isElse) {
        error("#elif after #else");
        return;
    }

    if (state.seenTrue) {
        state.active = false;
    } else {
        bool parentActive = true;
        if (conditionalStack_.size() > 1) {
            std::stack<ConditionalState> temp;
            temp.push(conditionalStack_.top());
            conditionalStack_.pop();
            parentActive = isActive();
            conditionalStack_.push(temp.top());
        }

        if (parentActive) {
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

            state.active = conditionTrue;
            if (conditionTrue) {
                state.seenTrue = true;
            }
        } else {
            state.active = false;
        }
    }
}

void Preprocessor::visit(pp::ElseDirective&) {
    if (conditionalStack_.empty()) {
        error("#else without #if");
        return;
    }

    ConditionalState& state = conditionalStack_.top();

    if (state.isElse) {
        error("duplicate #else");
        return;
    }

    state.isElse = true;

    if (state.seenTrue) {
        state.active = false;
    } else {
        bool parentActive = true;
        if (conditionalStack_.size() > 1) {
            std::stack<ConditionalState> temp;
            temp.push(conditionalStack_.top());
            conditionalStack_.pop();
            parentActive = isActive();
            conditionalStack_.push(temp.top());
        }

        state.active = parentActive;
        if (parentActive) {
            state.seenTrue = true;
        }
    }
}

void Preprocessor::visit(pp::EndifDirective&) {
    if (conditionalStack_.empty()) {
        error("#endif without #if");
        return;
    }

    conditionalStack_.pop();
}

} // namespace cc1
