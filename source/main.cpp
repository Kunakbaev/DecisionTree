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

int main() {
    setLoggingLevel(DEBUG);

    LOG_INFO("Hello world\n");

    Node* root = NULL;
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, (void*)50));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, (void*)30));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, (void*)10));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, (void*)70));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, (void*)65));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, (void*)80));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, (void*)90));

    IF_MAIN_ERR_RETURN(dumpDecisionTree(root));

    Dumper dumper = {};
    dumperConstructor(&dumper, 10, "logs", "png");
    dumperDumpDecisionTree(&dumper, root);

    dumperDestructor(&dumper);

    return 0;
}
