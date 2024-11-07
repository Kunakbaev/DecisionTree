#include <stdlib.h>

#include "../include/decisionTreeLib.hpp"
#include "../include/dumper.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, DECISION_TREE_INVALID_ARGUMENT, getDecisionTreeErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getDecisionTreeErrorMessage, DECISION_TREE_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getDecisionTreeErrorMessage)\

const size_t MIN_MEM_BUFF_SIZE  = 8;
const size_t OUTPUT_BUFFER_SIZE = 1 << 9;

static void initMemBuff(DecisionTree* tree) {
    assert(tree != NULL);

    for (size_t nodeInd = 0; nodeInd < tree->memBuffSize; ++nodeInd) {
        tree->memBuff[nodeInd].memBuffIndex = nodeInd;
    }
}

DecisionTreeErrors constructDecisionTree(DecisionTree* tree, Dumper* dumper) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(dumper);

    *tree = {};
    tree->root          = NULL;
    tree->memBuff       = (Node*)calloc(MIN_MEM_BUFF_SIZE, sizeof(Node));
    IF_NOT_COND_RETURN(tree->memBuff != NULL,
                       DECISION_TREE_MEMORY_ALLOCATION_ERROR);
    tree->memBuffSize   = MIN_MEM_BUFF_SIZE;
    tree->freeNodeIndex = 0; // 0 index is equal to NULL
    tree->dumper        = *dumper;
    initMemBuff(tree);

    return DECISION_TREE_STATUS_OK;
}

static DecisionTreeErrors resizeMemBuffer(DecisionTree* tree, size_t newSize) {
    IF_ARG_NULL_RETURN(tree);

    if (newSize < MIN_MEM_BUFF_SIZE)
        newSize = MIN_MEM_BUFF_SIZE;

    if (tree->memBuffSize == newSize) // nothing to do
        return DECISION_TREE_STATUS_OK;

    size_t oldSize = tree->memBuffSize;
    size_t deltaSize = tree->memBuffSize > newSize ?
                            tree->memBuffSize - newSize :
                            newSize - tree->memBuffSize;
    size_t deltaBytes = deltaSize * sizeof(Node);

    if (oldSize > newSize) {
        memset(tree->memBuff + newSize, 0, deltaBytes);
    }

    Node* tmp = (Node*)realloc(tree->memBuff, newSize * sizeof(Node));
    IF_NOT_COND_RETURN(tmp != NULL, DECISION_TREE_MEMORY_ALLOCATION_ERROR);
    tree->memBuff     = tmp;
    tree->memBuffSize = newSize;

    if (oldSize < newSize) {
        memset(tree->memBuff + oldSize, 0, deltaBytes - 1);
    }

    // if oldSize > newSize, no iterations will be executed
    for (size_t nodeInd = oldSize; nodeInd < newSize; ++nodeInd) {
        tree->memBuff[nodeInd].memBuffIndex = nodeInd;
    }

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors getNewNode(DecisionTree* tree, size_t* newNodeIndex) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(newNodeIndex);

    if (tree->freeNodeIndex + 1 >= tree->memBuffSize) {
        IF_ERR_RETURN(resizeMemBuffer(tree, tree->memBuffSize * 2));
    }
    assert(tree->freeNodeIndex < tree->memBuffSize);

    *newNodeIndex = ++tree->freeNodeIndex;
    LOG_DEBUG_VARS(newNodeIndex, tree->memBuff[*newNodeIndex].memBuffIndex);

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors addNewNodeToDecisionTree(DecisionTree* tree, node_data_type value) {
    IF_ARG_NULL_RETURN(tree);

    size_t currentNodeInd = tree->root;
    while (currentNodeInd != 0) {
        assert(currentNodeInd < tree->memBuffSize);
        Node node = tree->memBuff[currentNodeInd];

        size_t next = (value <= node.data ? node.left : node.right);
        if (next == 0) // ASK: cringe?
            break;
        currentNodeInd = next;
    }

    size_t newNodeIndex = 0;
    IF_ERR_RETURN(getNewNode(tree, &newNodeIndex));
    tree->memBuff[newNodeIndex].data = value;
    LOG_DEBUG_VARS(newNodeIndex);

    if (currentNodeInd == 0) { // tree was empty
        tree->root = newNodeIndex;
        // LOG_DEBUG_VARS(newNode->data);
        LOG_DEBUG("insert to empty tree");
    } else {
        assert(currentNodeInd < tree->memBuffSize);
        Node* node = &tree->memBuff[currentNodeInd];
        if (value <= node->data) {
            node->left = newNodeIndex;
            LOG_DEBUG("insert to left son");
        } else {
            node->right = newNodeIndex;
            LOG_DEBUG("insert to right son");
        }
    }

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors dumpDecisionTreeInConsole(const DecisionTree* tree, size_t nodeIndex,
                                             char** outputBuffer) {
    IF_ARG_NULL_RETURN(tree);

    // ASK: is strcat faster than sprintf
    if (nodeIndex == 0) {
        strncat(*outputBuffer, "?", OUTPUT_BUFFER_SIZE);
        ++(*outputBuffer);
        return DECISION_TREE_STATUS_OK;
    }

    strncat(*outputBuffer, "(", OUTPUT_BUFFER_SIZE);
    ++(*outputBuffer);

    LOG_DEBUG_VARS(nodeIndex, tree->memBuffSize);
    assert(nodeIndex < tree->memBuffSize);
    Node node = tree->memBuff[nodeIndex];
    IF_ERR_RETURN(dumpDecisionTreeInConsole(tree, node.left, outputBuffer));
    (*outputBuffer) += snprintf(*outputBuffer, OUTPUT_BUFFER_SIZE, " %d ", node.data);

    IF_ERR_RETURN(dumpDecisionTreeInConsole(tree, node.right, outputBuffer));

    strncat(*outputBuffer, ")", OUTPUT_BUFFER_SIZE);
    ++(*outputBuffer);

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors dumpDecisionTree(DecisionTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    LOG_DEBUG("--------------------------------------\n");
    LOG_DEBUG("decision tree:\n");
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, "--------------------------------------\n");
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, "decision tree:\n");

    char* outputBuffer = (char*)calloc(OUTPUT_BUFFER_SIZE, sizeof(char)); // ASK: is this ok?
    IF_NOT_COND_RETURN(outputBuffer != NULL,
                       DECISION_TREE_MEMORY_ALLOCATION_ERROR);

    char* targetPtr = outputBuffer;
    IF_ERR_RETURN(dumpDecisionTreeInConsole(tree, tree->root, &targetPtr));
    LOG_DEBUG(outputBuffer);
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, outputBuffer);
    FREE(outputBuffer);

    // FIXME: add dumper err check
    dumperDumpDecisionTree(&tree->dumper, tree, tree->freeNodeIndex);

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors destructDecisionTree(DecisionTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    FREE(tree->memBuff);
    tree->memBuffSize   = 0;
    tree->freeNodeIndex = 0;
    dumperDestructor(&tree->dumper);

    return DECISION_TREE_STATUS_OK;
}
