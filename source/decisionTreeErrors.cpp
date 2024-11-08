#include "../include/decisionTreeErrors.hpp"

const char* getDecisionTreeErrorMessage(DecisionTreeErrors error) {
    #define ERROR_UNPACK(errName, errCode, errMessage)          \
        case errName:                                           \
            return "Decision tree error: " errMessage ".\n";    \

    switch (error) {
        case DECISION_TREE_STATUS_OK:
            return "Decision tree: no errors occured.\n";

        // ASK: code generation
        #include "../include/decisionTreeErrorsPlainText.in"

        default:
            return "Decision tree error: unknown error.\n";
    }

    #undef UNPACK
}
