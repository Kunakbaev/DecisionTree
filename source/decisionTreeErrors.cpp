#include "../include/decisionTreeErrors.hpp"

const char* getDecisionTreeErrorMessage(DecisionTreeErrors error) {
    #define UNPACK(errName, errCode, errMessage)    \
        case errName:                               \
            return "Decision tree error: " errMessage ".\n";\

    switch (error) {
        case DECISION_TREE_STATUS_OK:
            return "Decision tree: no errors occured.\n";

        // #include "../include/decisionTreeErrorsPlainText.in"

        case DECISION_TREE_INVALID_ARGUMENT:
            return "Decision tree error: invalid argument, possilby set to NULL.\n";

        default:
            return "Decision tree error: unknown error.\n";
    }
}
