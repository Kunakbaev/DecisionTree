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
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, 50));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, 30));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, 10));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, 70));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, 65));
    IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(&root, 80));

    IF_MAIN_ERR_RETURN(dumpDecisionTree(root));

    Dumper dumper = {};
    dumperConstructor(&dumper, 10, "logs", "png");
    dumperDumpDecisionTree(&dumper, root);


    return 0;
}
