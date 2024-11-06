#include <stdlib.h>

#include "../include/decisionTreeLib.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, DECISION_TREE_INVALID_ARGUMENT, getDecisionTreeErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getDecisionTreeErrorMessage, DECISION_TREE_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getDecisionTreeErrorMessage)\

DecisionTreeErrors getNewNode(Node** node) {
    IF_ARG_NULL_RETURN(node);

    *node = (Node*)calloc(1, sizeof(Node*));
    IF_NOT_COND_RETURN(*node != NULL,
                       DECISION_TREE_MEMORY_ALLOCATION_ERROR);

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors addNewNodeToDecisionTree(Node** startNode, node_data_type value) {
    IF_ARG_NULL_RETURN(startNode);

    Node* currentNode = *startNode;
    while ((currentNode) != NULL) {
        Node* next = (value <= (currentNode)->data ? (currentNode)->left : (currentNode)->right);
        if (next == NULL) // ASK: cringe?
            break;
        (currentNode) = next;
    }

    Node* newNode = NULL;
    IF_ERR_RETURN(getNewNode(&newNode));
    newNode->data = value;
    LOG_DEBUG_VARS(newNode);

    if (currentNode == NULL) { // tree was empty
        *startNode = newNode;
        LOG_DEBUG_VARS(newNode->data);
        LOG_DEBUG("insert to empty tree");
    } else {
        if (value <= (currentNode)->data) {
            (currentNode)->left = newNode;
            LOG_DEBUG("insert to left son");
        } else {
            (currentNode)->right = newNode;
            LOG_DEBUG("insert to right son");
        }
    }

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors dumpDecisionTreeInConsole(const Node* node) {
    if (node == NULL) {
        printf("?");
        return DECISION_TREE_STATUS_OK;
    }

    printf("(");

    IF_ERR_RETURN(dumpDecisionTreeInConsole(node->left));
    printf(" %d ", node->data);
    IF_ERR_RETURN(dumpDecisionTreeInConsole(node->right));

    printf(")");

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors dumpDecisionTree(const Node* tree) {
    // IF_ARG_NULL_RETURN(tree);

    printf("--------------------------------------\n");
    printf("decision tree:\n");
    IF_ERR_RETURN(dumpDecisionTreeInConsole(tree));
    printf("\n");

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors destructDecisionTree(Node* node) {
    if (node == NULL) // subtree is empty, nothing to destruct
        return DECISION_TREE_STATUS_OK;

    IF_ERR_RETURN(destructDecisionTree(node->left));
    IF_ERR_RETURN(destructDecisionTree(node->right));

    FREE(node); // TODO: check that this works

    return DECISION_TREE_STATUS_OK;
}
