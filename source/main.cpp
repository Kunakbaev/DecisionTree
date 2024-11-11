#include <iostream>
#include "logLib.hpp"
#include "../include/decisionTreeLib.hpp"
#include "../include/dumper.hpp"

#define IF_MAIN_ERR_RETURN(error)                                   \
    do {                                                            \
        DecisionTreeErrors tmp = error;                             \
        if (tmp != DECISION_TREE_STATUS_OK) {                       \
            LOG_ERROR(getDecisionTreeErrorMessage(tmp));            \
            /* if error occurs in destructor, then we are screwed*/ \
            destructDecisionTree(&tree);                            \
            assert(tmp != DECISION_TREE_STATUS_OK);                 \
            return tmp;                                             \
        }                                                           \
    } while(0)

// DecisionTreeErrors addValueAndDump(DecisionTree* tree, const void* value);

int main() {
    setLoggingLevel(DEBUG);

    Dumper dumper = {};
    dumperConstructor(&dumper, 10, "logs", "png");
    DecisionTree tree = {};

    IF_MAIN_ERR_RETURN(constructDecisionTree(&tree, &dumper));

    IF_MAIN_ERR_RETURN(readDecisionTreeFromFile(&tree, "fruitsAndVegetablesDB.txt"));
    IF_MAIN_ERR_RETURN(dumpDecisionTree(&tree));

//     IF_MAIN_ERR_RETURN(tryToGuessObject(&tree));
//     IF_MAIN_ERR_RETURN(dumpDecisionTree(&tree));
//
//     IF_MAIN_ERR_RETURN(tryToGuessObject(&tree));
//     IF_MAIN_ERR_RETURN(dumpDecisionTree(&tree));
//
//     IF_MAIN_ERR_RETURN(tryToGuessObject(&tree));
//     IF_MAIN_ERR_RETURN(dumpDecisionTree(&tree));
//
//     IF_MAIN_ERR_RETURN(printPathToObjByName(&tree, "hui"));

    destructDecisionTree(&tree);

    return 0;
}

// DecisionTreeErrors addValueAndDump(DecisionTree* tree, const void* value) {
//     if (tree == NULL) {
//         LOG_ERROR(getDecisionTreeErrorMessage(DECISION_TREE_INVALID_ARGUMENT));
//         return DECISION_TREE_INVALID_ARGUMENT;
//     }
//
//     IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(tree, (void*)value));
//     IF_MAIN_ERR_RETURN(dumpDecisionTree(tree));
//
//     return DECISION_TREE_STATUS_OK;
// }
