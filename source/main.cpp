#include <iostream>
#include "logLib.hpp"
#include "../include/decisionTreeLib.hpp"
#include "../include/dumper.hpp"

#define IF_MAIN_ERR_RETURN(error)                                   \
    do {                                                            \
        DecisionTreeErrors tmp = error;\
        if (tmp != DECISION_TREE_STATUS_OK) {                         \
            LOG_ERROR(getDecisionTreeErrorMessage(tmp));             \
            assert(tmp != DECISION_TREE_STATUS_OK);                   \
            return tmp;                                           \
        }                                                           \
    } while(0)

DecisionTreeErrors addValueAndDump(DecisionTree* tree, const void* value);

int main() {
    setLoggingLevel(DEBUG);

    LOG_INFO("Hello world\n");

    Dumper dumper = {};
    dumperConstructor(&dumper, 10, "logs", "png");
    DecisionTree tree = {};

    IF_MAIN_ERR_RETURN(constructDecisionTree(&tree, &dumper, cmpStringsLessEqual, "%s"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"aba"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"hubabuba"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"abacaba"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"giraffe"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"bababoi"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"zebra"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"abba"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"baba"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"yamate"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"kudasai"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"yamayka wins"));
    IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)"zebroid"));

    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)50));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)30));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)10));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)70));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)65));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)80));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)90));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)70));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)65));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)80));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)90));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)70));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)65));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)80));
    // IF_MAIN_ERR_RETURN(addValueAndDump(&tree, (void*)90));

    dumperDestructor(&dumper);

    return 0;
}

DecisionTreeErrors addValueAndDump(DecisionTree* tree, const void* value) {
    if (tree == NULL) {
        LOG_ERROR(getDecisionTreeErrorMessage(DECISION_TREE_INVALID_ARGUMENT));
        return DECISION_TREE_INVALID_ARGUMENT;
    }

    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(tree, (void*)value));
    IF_MAIN_ERR_RETURN(dumpDecisionTree(tree));

    return DECISION_TREE_STATUS_OK;
}
