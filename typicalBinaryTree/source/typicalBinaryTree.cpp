#include <stdlib.h>
#include <inttypes.h>

#include "../include/typicalBinaryTree.hpp"
#include "../../include/dumper.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, TYPICAL_BIN_TREE_INVALID_ARGUMENT, getTypicalBinaryTreeErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getTypicalBinaryTreeErrorMessage, TYPICAL_BIN_TREE_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getTypicalBinaryTreeErrorMessage)\

#define DUMPER_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getDumperErrorMessage, DUMPER_STATUS_OK, TYPICAL_BIN_TREE_DUMPER_ERROR);

const size_t MIN_MEM_BUFF_SIZE  = 8;
const size_t OUTPUT_BUFFER_SIZE = 1 << 9;
const size_t MAX_OBJ_NAME_LEN   = 30;
const char   BREAK_CHAR         = '|';

static void initMemBuff(TypicalBinaryTree* tree) {
    assert(tree != NULL);

    for (size_t nodeInd = 0; nodeInd < tree->memBuffSize; ++nodeInd) {
        tree->memBuff[nodeInd].memBuffIndex = nodeInd;
    }
}

TypicalBinaryTreeErrors constructTypicalBinaryTree(TypicalBinaryTree* tree, Dumper* dumper,
                                                   nodeDataToStringFuncPtr nodeDataPrinter,
                                                   stringToNodeDataFuncPtr nodeDataReader,
                                                   cmpNodesByValueFuncPtr  nodesComparator) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(dumper);
    IF_ARG_NULL_RETURN(nodeDataPrinter);
    IF_ARG_NULL_RETURN(nodeDataReader);
    IF_ARG_NULL_RETURN(nodesComparator);

    tree->root              = 0;
    tree->memBuff           = (Node*)calloc(MIN_MEM_BUFF_SIZE, sizeof(Node));
    IF_NOT_COND_RETURN(tree->memBuff != NULL,
                       TYPICAL_BIN_TREE_MEMORY_ALLOCATION_ERROR);
    tree->memBuffSize       = MIN_MEM_BUFF_SIZE;
    tree->freeNodeIndex     = 0; // 0 index is equal to NULL
    tree->dumper            = *dumper;
    tree->nodeDataToString  = nodeDataPrinter;
    tree->stringToNodeData  = nodeDataReader;
    tree->cmpNodesByValue   = nodesComparator;
    initMemBuff(tree);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

static TypicalBinaryTreeErrors resizeMemBuffer(TypicalBinaryTree* tree, size_t newSize) {
    IF_ARG_NULL_RETURN(tree);

    if (newSize < MIN_MEM_BUFF_SIZE)
        newSize = MIN_MEM_BUFF_SIZE;

    if (tree->memBuffSize == newSize) // nothing to do
        return TYPICAL_BIN_TREE_STATUS_OK;

    size_t oldSize   = tree->memBuffSize;
    size_t deltaSize = tree->memBuffSize > newSize
                            ? tree->memBuffSize - newSize
                            : newSize - tree->memBuffSize;
    size_t deltaBytes = deltaSize * sizeof(Node);

    if (oldSize > newSize) {
        memset(tree->memBuff + newSize, 0, deltaBytes);
    }

    LOG_DEBUG_VARS(oldSize, deltaSize, deltaBytes);
    Node* tmp = (Node*)realloc(tree->memBuff, newSize * sizeof(Node));
    IF_NOT_COND_RETURN(tmp != NULL, TYPICAL_BIN_TREE_MEMORY_ALLOCATION_ERROR);
    tree->memBuff     = tmp;
    tree->memBuffSize = newSize;

    LOG_DEBUG_VARS(tmp);

    if (oldSize < newSize) {
        memset(tree->memBuff + oldSize, 0, deltaBytes - 1);
    }

    // if oldSize > newSize, no iterations will be executed
    for (size_t nodeInd = oldSize; nodeInd < newSize; ++nodeInd) {
        tree->memBuff[nodeInd].memBuffIndex = nodeInd;
    }

    return TYPICAL_BIN_TREE_STATUS_OK;
}

static TypicalBinaryTreeErrors getNewNode(TypicalBinaryTree* tree, size_t* newNodeIndex) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(newNodeIndex);

    if (tree->freeNodeIndex + 1 >= tree->memBuffSize) {
        IF_ERR_RETURN(resizeMemBuffer(tree, tree->memBuffSize * 2));
    }
    assert(tree->freeNodeIndex < tree->memBuffSize);

    *newNodeIndex = ++tree->freeNodeIndex;
    LOG_DEBUG_VARS(*newNodeIndex, tree->memBuffSize);
    LOG_DEBUG_VARS(newNodeIndex, tree->memBuff[*newNodeIndex].memBuffIndex);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors isValueInBinTree(const TypicalBinaryTree* tree, const void* value, bool* isIn) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(value);
    IF_ARG_NULL_RETURN(isIn);

    *isIn = false;
    for (size_t ind = 1; ind < tree->memBuffSize; ++ind) {
        const void* const ptr = tree->memBuff[ind].data;
        if (ptr == NULL) continue;

        LOG_DEBUG_VARS(ind, ptr, value, tree->cmpNodesByValue);
        int cmpResult = (*tree->cmpNodesByValue)(ptr, value);
        if (cmpResult == 0) {
            *isIn = true;
            return TYPICAL_BIN_TREE_STATUS_OK;
        }
    }
    LOG_DEBUG("ok");

    *isIn = false;
    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors getTypicalBinaryTreeNodeByValue(const TypicalBinaryTree* tree, const void* value, Node* result) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(value);
    IF_ARG_NULL_RETURN(result);

    for (size_t ind = 1; ind < tree->memBuffSize; ++ind) {
        const void* const ptr = tree->memBuff[ind].data;
        if (ptr == NULL) continue;

        int cmpResult = (*tree->cmpNodesByValue)(ptr, value);
        if (cmpResult == 0) {
            *result = tree->memBuff[ind];
            return TYPICAL_BIN_TREE_STATUS_OK;
        }
    }

    return TYPICAL_BIN_TREE_OBJ_NOT_FOUND;
}

TypicalBinaryTreeErrors addNewObjectToTypicalBinaryTree(TypicalBinaryTree* tree, size_t parentInd,
                                                        const void* value, bool isToLeftSon,
                                                        size_t* newNodeIndex) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(value);
    IF_ARG_NULL_RETURN(newNodeIndex);
    //IF_NOT_COND_RETURN(parentInd != 0, TYPICAL_BIN_TREE_INVALID_ARGUMENT);

    *newNodeIndex = 0;
    IF_ERR_RETURN(getNewNode(tree, newNodeIndex));
    Node* node = &tree->memBuff[*newNodeIndex];
    node->data = (void*)value;

    IF_NOT_COND_RETURN(parentInd < tree->memBuffSize,
                       TYPICAL_BIN_TREE_INVALID_ARGUMENT); // TODO: add error
    Node* parent = &tree->memBuff[parentInd];
    LOG_DEBUG_VARS(parentInd, *newNodeIndex);
    if (!parentInd) {
        LOG_DEBUG("no parent, tree is empty");
        tree->root = *newNodeIndex;
        node->parent = 0;
    } else {
        if (isToLeftSon)
            parent->left  = *newNodeIndex;
        else
            parent->right = *newNodeIndex;
        node->parent = parent->memBuffIndex;
    }

    LOG_DEBUG_VARS(*newNodeIndex, tree->root, node->parent, node->data, parentInd);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors isNodeAleftSonOfParent(const TypicalBinaryTree* tree, size_t parentInd, size_t vertInd, bool* is) {
    IF_ARG_NULL_RETURN(tree);
    IF_NOT_COND_RETURN(parentInd < tree->memBuffSize,
                       TYPICAL_BIN_TREE_INVALID_ARGUMENT);

    Node parent = tree->memBuff[parentInd];
    *is = vertInd == parent.left;

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors getBinTreeNodeByVertIndex(const TypicalBinaryTree* tree, size_t vertInd, Node* result) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(result);
    IF_NOT_COND_RETURN(vertInd < tree->memBuffSize,
                       TYPICAL_BIN_TREE_INVALID_ARGUMENT); // TODO: add error

    *result = tree->memBuff[vertInd];

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors isBinTreeEmpty(const TypicalBinaryTree* tree, bool* is) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(is);

    *is = tree->root == 0;

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors isBinTreeNodeLeaf(const TypicalBinaryTree* tree, size_t currentNodeInd, bool* is) {
    if (!currentNodeInd) {
        *is = true;
        return TYPICAL_BIN_TREE_STATUS_OK;
    }

    assert(currentNodeInd < tree->memBuffSize);
    Node node = tree->memBuff[currentNodeInd];
    *is = node.left == 0 && node.right == 0;
    return TYPICAL_BIN_TREE_STATUS_OK;
}





static size_t getNodesDepth(const TypicalBinaryTree* tree, const Node* start) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(start);
    size_t currentNodeInd = start->memBuffIndex;
    size_t depth = 0;

    while (currentNodeInd != 0) {
        ++depth;
        assert(currentNodeInd < tree->memBuffSize);
        currentNodeInd = tree->memBuff[currentNodeInd].parent;
    }

    return depth;
}

static TypicalBinaryTreeErrors fillPathArrayFromRootToNode(const TypicalBinaryTree* tree, const Node* start, size_t pathLen, size_t* path) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(start);
    IF_ARG_NULL_RETURN(path);
    size_t currentNodeInd = start->memBuffIndex;
    size_t depth = 0;

    LOG_DEBUG_VARS(currentNodeInd);
    while (currentNodeInd != 0 && depth < pathLen) {
        ++depth;
        assert(currentNodeInd < tree->memBuffSize);
        //LOG_DEBUG_VARS(pathLen, depth);
        path[pathLen - depth] = currentNodeInd;
        //LOG_DEBUG_VARS(currentNodeInd, pathLen - depth, pathLen, depth);
        currentNodeInd = tree->memBuff[currentNodeInd].parent;
    }

    return TYPICAL_BIN_TREE_STATUS_OK;
}

static TypicalBinaryTreeErrors printPathArrayFromRootToNode(const TypicalBinaryTree* tree,
                                                       size_t pathLen, size_t* path,
                                                       bool isLastIncluded) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path);

    char output[OUTPUT_BUFFER_SIZE] = {};
    char* ptr = output;
    size_t prev = 0;
    for (size_t elemInd = 0; elemInd < pathLen; ++elemInd) {
        size_t nodeInd = path[elemInd];
        assert(nodeInd < tree->memBuffSize);
        if (prev != 0) {
            const char* tmpWord = nodeInd == tree->memBuff[prev].right ? "yes||" : "no||";
            strncat(ptr, tmpWord, OUTPUT_BUFFER_SIZE - (ptr - output));
            ptr += strlen(tmpWord);
        }

        if (!isLastIncluded && elemInd == pathLen - 1)
            break;
        ptr += snprintf(ptr, OUTPUT_BUFFER_SIZE - (ptr - output), "%s||", tree->memBuff[nodeInd].data);
        prev = nodeInd;
    }
    LOG_INFO(output);
    DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&tree->dumper, pathLen, path, output);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

static TypicalBinaryTreeErrors getPathToTypicalBinaryTreeNode(const TypicalBinaryTree* tree, const Node* node,
                                                              size_t* pathLen, size_t** path) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path);
    IF_ARG_NULL_RETURN(pathLen);

    *pathLen = getNodesDepth(tree, node);
    LOG_DEBUG_VARS(pathLen);

    *path = (size_t*)calloc(*pathLen, sizeof(size_t));
    LOG_DEBUG_VARS(*pathLen);
    IF_NOT_COND_RETURN(*path != NULL,
                       TYPICAL_BIN_TREE_MEMORY_ALLOCATION_ERROR);

    IF_ERR_RETURN(fillPathArrayFromRootToNode(tree, node, *pathLen, *path));

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors getPathToObjByVertInd(const TypicalBinaryTree* tree, size_t vertInd,
                                              size_t* pathLen, size_t** path) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(pathLen);
    IF_ARG_NULL_RETURN(path);

    assert(vertInd < tree->memBuffSize);
    Node node = tree->memBuff[vertInd];

    IF_ERR_RETURN(getPathToTypicalBinaryTreeNode(tree, &node, pathLen, path));

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors getPathToObjByValue(const TypicalBinaryTree* tree, const void* value,
                                            size_t* pathLen, size_t** path) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(value);
    IF_ARG_NULL_RETURN(pathLen);
    IF_ARG_NULL_RETURN(path);

    Node node = {};
    IF_ERR_RETURN(getTypicalBinaryTreeNodeByValue(tree, value, &node));
//     size_t vertInd = node.memBuffIndex;
//
//     assert(vertInd < tree->memBuffSize);
//     node = tree->memBuff[vertInd];

    IF_ERR_RETURN(getPathToTypicalBinaryTreeNode(tree, &node, pathLen, path));

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors printPathToObjByName(const TypicalBinaryTree* tree, size_t vertInd) {
    IF_ARG_NULL_RETURN(tree);

    size_t*   path = NULL;
    size_t pathLen = 0;
    IF_ERR_RETURN(getPathToObjByVertInd(tree, vertInd, &pathLen, &path));

    IF_ERR_RETURN(printPathArrayFromRootToNode(tree, pathLen, path, true));
    FREE(path);

    return TYPICAL_BIN_TREE_STATUS_OK;
}







static const char* trimBeginningOfLine(const char* line) {
    assert(line != NULL);
    const char* ptr = line;
    // TODO: add delims const string
    while (*ptr == '\t' || *ptr == ' ')
        ++ptr;

    return ptr;
}

static TypicalBinaryTreeErrors recursiveSaveOfTreeInFile(TypicalBinaryTree* tree, size_t nodeInd, size_t depth, FILE* file) {
    if (!nodeInd)
        return TYPICAL_BIN_TREE_STATUS_OK;

    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(file);

    const size_t BUFF_SIZE = 100;
    char tabs[BUFF_SIZE] = {};
    for (size_t i = 0; i < depth; ++i)
        tabs[i] = '\t';

    assert(nodeInd < tree->memBuffSize);
    Node node = tree->memBuff[nodeInd];
    fprintf(file, "%s{\n", tabs);
    fprintf(file, "%s\t%s\n", tabs, node.data);
    LOG_DEBUG_VARS(tabs, node.data, nodeInd);
    IF_ERR_RETURN(recursiveSaveOfTreeInFile(tree, node.left, depth + 1, file));
    IF_ERR_RETURN(recursiveSaveOfTreeInFile(tree, node.right, depth + 1, file));
    fprintf(file, "%s}\n", tabs);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors saveTypicalBinaryTreeToFile(TypicalBinaryTree* tree, const char* fileName) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(fileName);

    FILE* file = fopen(fileName, "w");
    IF_NOT_COND_RETURN(file != NULL, TYPICAL_BIN_TREE_FILE_OPENING_ERROR);

    IF_ERR_RETURN(recursiveSaveOfTreeInFile(tree, tree->root, 0, file));
    fclose(file);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

static TypicalBinaryTreeErrors addNewNodeForFile(TypicalBinaryTree* tree, Node** node, size_t len,
                                                 const char* tmp, size_t preInd) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(node);
    LOG_DEBUG("creating new node");

    assert(preInd < tree->memBuffSize);
    Node* pre = &tree->memBuff[preInd];
    bool isToLeftSon = pre->left == 0;

    Node tmpNode = {};
    (*tree->stringToNodeData)(&tmpNode.data, tmp);
    //tmpNode.data = (void*)tmp;
    LOG_DEBUG_VARS(tmp, tmpNode.data);

    size_t newNodeIndex = 0;
    IF_ERR_RETURN(addNewObjectToTypicalBinaryTree(tree, preInd, tmpNode.data, isToLeftSon, &newNodeIndex));
    *node = &tree->memBuff[newNodeIndex];

    IF_ERR_RETURN(dumpTypicalBinaryTree(tree));
    // bool isBadChar = strchr((*node)->data, BREAK_CHAR) != NULL;
    // IF_NOT_COND_RETURN(!doesStringContainBreakChar((*node)->data),
    //                     DECISION_TREE_INVALID_INPUT_STRING);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors readTypicalBinaryTreeFromFile(TypicalBinaryTree* tree, const char* fileName) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(fileName);

    FILE* file = fopen(fileName, "r");
    IF_NOT_COND_RETURN(file != NULL, TYPICAL_BIN_TREE_FILE_OPENING_ERROR);

    tree->root = 0;
    const size_t LINE_BUFF_SIZE = 200;
    char lineBuffer[LINE_BUFF_SIZE];

    Node*  node = NULL;
    size_t preInd = 0;
    // TODO: check existence only for objects
    while (fgets(lineBuffer, LINE_BUFF_SIZE, file) != NULL) {
        char* tmp = (char*)trimBeginningOfLine(lineBuffer);
        size_t len = strlen(tmp);
        assert(len >= 1);
        tmp[len - 1] = '\0';
        LOG_DEBUG_VARS(lineBuffer, tmp);

        if (strcmp(tmp, "{") == 0) {
            LOG_DEBUG("entry in subtree");
            preInd = node == NULL ? 0 : node->memBuffIndex;
            continue;
        }
        if (strcmp(tmp, "}") == 0) {
            LOG_DEBUG("leaving subtree");
            assert(node != NULL);
            assert(node->parent < tree->memBuffSize);
            node = &tree->memBuff[node->parent];
            continue;
        }

        IF_ERR_RETURN(addNewNodeForFile(tree, &node, len, tmp, preInd));
    }

    fclose(file);

    return TYPICAL_BIN_TREE_STATUS_OK;
}








static TypicalBinaryTreeErrors dumpTypicalBinaryTreeInConsole(const TypicalBinaryTree* tree, size_t nodeIndex,
                                                              char** outputBuffer) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(outputBuffer);
    IF_ARG_NULL_RETURN(*outputBuffer);

    if (nodeIndex == 0) {
        strncat(*outputBuffer, "?", OUTPUT_BUFFER_SIZE);
        ++(*outputBuffer);
        return TYPICAL_BIN_TREE_STATUS_OK;
    }

    strncat(*outputBuffer, "(", OUTPUT_BUFFER_SIZE);
    ++(*outputBuffer);

    assert(nodeIndex < tree->memBuffSize);
    Node node = tree->memBuff[nodeIndex];
    IF_ERR_RETURN(dumpTypicalBinaryTreeInConsole(tree, node.left, outputBuffer));
    const char* nodeDataString = (*tree->nodeDataToString)(node.data);
    (*outputBuffer) += snprintf(*outputBuffer, OUTPUT_BUFFER_SIZE, " %s ", nodeDataString);

    IF_ERR_RETURN(dumpTypicalBinaryTreeInConsole(tree, node.right, outputBuffer));

    strncat(*outputBuffer, ")", OUTPUT_BUFFER_SIZE);
    ++(*outputBuffer);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors dumpTypicalBinaryTree(TypicalBinaryTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    LOG_DEBUG("--------------------------------------\n");
    LOG_DEBUG("decision tree:\n");
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, "--------------------------------------\n");
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, "decision tree:\n");

    char* outputBuffer = (char*)calloc(OUTPUT_BUFFER_SIZE, sizeof(char)); // ASK: is this ok?
    IF_NOT_COND_RETURN(outputBuffer != NULL,
                       TYPICAL_BIN_TREE_MEMORY_ALLOCATION_ERROR);

    for (size_t nodeInd = 0; nodeInd < tree->memBuffSize; ++nodeInd) {
        Node node = tree->memBuff[nodeInd];
        const char* data = (*tree->nodeDataToString)(node.data);
        size_t parent = node.parent;
        size_t left = node.left;
        size_t right = node.right;
        LOG_DEBUG_VARS(nodeInd, data, parent, left, right);
        DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, nodeInd, data, parent, left, right);
    }

    char* targetPtr = outputBuffer;
    IF_ERR_RETURN(dumpTypicalBinaryTreeInConsole(tree, tree->root, &targetPtr));
    LOG_DEBUG(outputBuffer);
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, outputBuffer);
    FREE(outputBuffer);

    // FIXME: add dumper err check
    size_t nodesArr[MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT] = {}; // ASK: is it ok?
    nodesArr[0] = {tree->freeNodeIndex};
    NodesWithColor rule1 = {
        "green",
        (size_t)1,
        nodesArr,
    };
    NodesWithColor coloringRule[] = {rule1};
    DUMPER_ERR_CHECK(dumperDumpBinaryTree(&tree->dumper, tree, coloringRule, 1));

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors openImageOfCurrentStateTypicalBinaryTree(TypicalBinaryTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    // first we need to create image of current state of tree
    IF_ERR_RETURN(dumpTypicalBinaryTree(tree));

    // FIXME: be carefull with nasty commands
    const char* fileName = getLastImageFileName(&tree->dumper);
    const size_t TMP_LEN = 100;
    char tmp[TMP_LEN] = {};
    snprintf(tmp, TMP_LEN, "xdg-open %s", fileName);
    system(tmp);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors getCommonPathCntArray(const TypicalBinaryTree* tree, size_t obj1, size_t obj2, size_t** cntArr) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(cntArr);

    size_t*   path1 = NULL;
    size_t pathLen1 = 0;
    IF_ERR_RETURN(getPathToObjByVertInd(tree, obj1, &pathLen1, &path1));

    size_t*   path2 = NULL;
    size_t pathLen2 = 0;
    IF_ERR_RETURN(getPathToObjByVertInd(tree, obj2, &pathLen2, &path2));

    *cntArr = (size_t*)calloc(tree->memBuffSize, sizeof(size_t));
    IF_NOT_COND_RETURN(cntArr != NULL,
                       TYPICAL_BIN_TREE_MEMORY_ALLOCATION_ERROR);

    for (size_t i = 0; i < pathLen1; ++i) ++(*cntArr)[path1[i]];
    for (size_t i = 0; i < pathLen2; ++i) ++(*cntArr)[path2[i]];

    FREE(path1);
    FREE(path2);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors dumpCommonPathOf2Objects(TypicalBinaryTree* tree, size_t obj1, size_t obj2) {
    IF_ARG_NULL_RETURN(tree);

    size_t* cntArr = NULL;
    IF_ERR_RETURN(getCommonPathCntArray(tree, obj1, obj2, &cntArr));

    size_t commonNodes[MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT] = {}; // ASK: is it ok?
    size_t diffNodes  [MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT] = {}; // ASK: is it ok?
    size_t commonNodesLen = 0;
    size_t diffNodesLen   = 0;
    for (size_t i = 0; i < tree->memBuffSize; ++i) {
        if (cntArr[i] == 0) continue; // node doesn't belong to any pathes

        if (cntArr[i] == 2) { // node lies in both pathes
            if (commonNodesLen >= MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT)
                continue; // ASK: throw error?
            LOG_DEBUG_VARS("common", i);
            commonNodes[commonNodesLen++] = i;
        } else {
            if (diffNodesLen >= MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT)
                continue; // ASK: throw error?
            LOG_DEBUG_VARS("different", i);
            diffNodes[diffNodesLen++] = i;
        }
    }

    NodesWithColor commonNodesRule = {
        "green",
        commonNodesLen,
        commonNodes,
    };
    NodesWithColor diffNodesRule = {
        "red",
        diffNodesLen,
        diffNodes,
    };
    NodesWithColor coloringRule[] = {commonNodesRule, diffNodesRule};
    DUMPER_ERR_CHECK(dumperDumpBinaryTree(&tree->dumper, tree, coloringRule, 2));

    const char* fileName = getLastImageFileName(&tree->dumper);
    const size_t TMP_LEN = 100;
    char tmp[TMP_LEN] = {};
    snprintf(tmp, TMP_LEN, "xdg-open %s", fileName);
    system(tmp);

    FREE(cntArr);

    return TYPICAL_BIN_TREE_STATUS_OK;
}

TypicalBinaryTreeErrors dumpCommonPathOf2ObjectsByValue(TypicalBinaryTree* tree,
                                                        const void* objValue_1, const void* objValue_2) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(objValue_1);
    IF_ARG_NULL_RETURN(objValue_2);

    Node obj1 = {};
    Node obj2 = {};
    IF_ERR_RETURN(getTypicalBinaryTreeNodeByValue(tree, objValue_1, &obj1));
    IF_ERR_RETURN(getTypicalBinaryTreeNodeByValue(tree, objValue_2, &obj2));

    LOG_DEBUG_VARS(obj1.memBuffIndex, obj2.memBuffIndex);

    IF_ERR_RETURN(dumpCommonPathOf2Objects(tree, obj1.memBuffIndex, obj2.memBuffIndex));

    return TYPICAL_BIN_TREE_STATUS_OK;
}


TypicalBinaryTreeErrors destructTypicalBinaryTree(TypicalBinaryTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    for (size_t nodeInd = 1; nodeInd < tree->memBuffSize; ++nodeInd) {
        FREE(tree->memBuff[nodeInd].data);
    }
    FREE(tree->memBuff);

    tree->memBuffSize   = 0;
    tree->freeNodeIndex = 0;
    dumperDestructor(&tree->dumper);

    return TYPICAL_BIN_TREE_STATUS_OK;
}
