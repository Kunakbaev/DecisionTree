#include <stdlib.h>
#include <inttypes.h>

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

#define DUMPER_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getDumperErrorMessage, DUMPER_STATUS_OK, DECISION_TREE_DUMPER_ERROR);

const size_t MIN_MEM_BUFF_SIZE  = 8;
const size_t OUTPUT_BUFFER_SIZE = 1 << 9;
const size_t MAX_OBJ_NAME_LEN   = 30;
const char   BREAK_CHAR         = '#';

static void initMemBuff(DecisionTree* tree) {
    assert(tree != NULL);

    for (size_t nodeInd = 0; nodeInd < tree->memBuffSize; ++nodeInd) {
        tree->memBuff[nodeInd].memBuffIndex = nodeInd;
    }
}

DecisionTreeErrors constructDecisionTree(DecisionTree* tree, Dumper* dumper) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(dumper);

    tree->root              = 0;
    tree->memBuff           = (Node*)calloc(MIN_MEM_BUFF_SIZE, sizeof(Node));
    IF_NOT_COND_RETURN(tree->memBuff != NULL,
                       DECISION_TREE_MEMORY_ALLOCATION_ERROR);
    tree->memBuffSize       = MIN_MEM_BUFF_SIZE;
    tree->freeNodeIndex     = 0; // 0 index is equal to NULL
    tree->dumper            = *dumper;
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
    size_t deltaSize = tree->memBuffSize > newSize
                            ? tree->memBuffSize - newSize
                            : newSize - tree->memBuffSize;
    size_t deltaBytes = deltaSize * sizeof(Node);

    if (oldSize > newSize) {
        memset(tree->memBuff + newSize, 0, deltaBytes);
    }

    LOG_DEBUG_VARS(oldSize, deltaSize, deltaBytes);
    Node* tmp = (Node*)realloc(tree->memBuff, newSize * sizeof(Node));
    IF_NOT_COND_RETURN(tmp != NULL, DECISION_TREE_MEMORY_ALLOCATION_ERROR);
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

    LOG_DEBUG_VARS("bruh");
    return DECISION_TREE_STATUS_OK;
}

static DecisionTreeErrors getNewNode(DecisionTree* tree, size_t* newNodeIndex) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(newNodeIndex);

    if (tree->freeNodeIndex + 1 >= tree->memBuffSize) {
        IF_ERR_RETURN(resizeMemBuffer(tree, tree->memBuffSize * 2));
    }
    assert(tree->freeNodeIndex < tree->memBuffSize);

    *newNodeIndex = ++tree->freeNodeIndex;
    LOG_DEBUG_VARS(*newNodeIndex, tree->memBuffSize);
    LOG_DEBUG_VARS(newNodeIndex, tree->memBuff[*newNodeIndex].memBuffIndex);

    return DECISION_TREE_STATUS_OK;
}

static bool isAnswerOnQuestionYes() {
    printf("Print yes or no: ");

    const size_t ANS_BUFF_SIZE = 10;
    char answerBuff[ANS_BUFF_SIZE];
    fgets(answerBuff, ANS_BUFF_SIZE, stdin);

    bool isCorrectGuess = false;
    LOG_DEBUG_VARS(answerBuff);
    return strcmp(answerBuff, "yes\n") == 0 ||
           strcmp(answerBuff, "\n")    == 0;
}

DecisionTreeErrors askQuestion(DecisionTree* tree, const Node* node, bool* isToLeftSon) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(node);

    // TODO: check that it's not leaf
    printf("%s\n", node->data);
    *isToLeftSon = !isAnswerOnQuestionYes();

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors isObjectGuessedCorrectly(DecisionTree* tree, const Node* node, bool* isCorrectGuess) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(isCorrectGuess);

    printf("Decision tree's guess is: \"%s\", is it your object?\n", node->data);
    *isCorrectGuess = isAnswerOnQuestionYes();

    return DECISION_TREE_STATUS_OK;
}

// if string contains break char it's invalid
static bool doesStringContainBreakChar(const char* word) {
    assert(word != NULL);

    const char* ptr = word;
    while (*ptr != '\0') {
        if (*ptr == BREAK_CHAR)
            return true;
        ++ptr;
    }

    return false;
}

static DecisionTreeErrors checkThatObjNotInTree(const DecisionTree* tree, const char* objName) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(objName);

    // TODO: check only leaf nodes, because nodes are only in leaves
    for (size_t ind = 1; ind < tree->memBuffSize; ++ind) {
        const char* const ptr = tree->memBuff[ind].data;
        // LOG_DEBUG_VARS(ptr, objName);
        if (ptr != NULL && strcmp(ptr, objName) == 0) {
            return DECISION_TREE_OBJ_ALREADY_EXISTS;
        }
    }

    return DECISION_TREE_STATUS_OK;
}

static DecisionTreeErrors readStringToNodesData(DecisionTree* tree, Node* node, bool isObject) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(node);

    char inputBuff[MAX_OBJ_NAME_LEN];
    fgets(inputBuff, MAX_OBJ_NAME_LEN, stdin);

    size_t len = strlen(inputBuff);
    assert(len >= 1);
    inputBuff[len - 1] = '\0';
    node->data = (char*)calloc(len, sizeof(char));
    assert(node->data != NULL);

    // TODO: check only for objects
    if (isObject)
        IF_ERR_RETURN(checkThatObjNotInTree(tree, inputBuff));
    strcpy(node->data, inputBuff);

    IF_NOT_COND_RETURN(!doesStringContainBreakChar(node->data),
                        DECISION_TREE_INVALID_INPUT_STRING);
}

static DecisionTreeErrors getDecisionTreeNodeByObjName(const DecisionTree* tree, const char* objName, Node* result) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(objName);
    IF_ARG_NULL_RETURN(result);

    for (size_t ind = 1; ind < tree->memBuffSize; ++ind) {
        const char* const ptr = tree->memBuff[ind].data;
        if (ptr != NULL && strcmp(ptr, objName) == 0) {
            *result = tree->memBuff[ind];
            return DECISION_TREE_STATUS_OK;
        }
    }

    return DECISION_TREE_OBJ_NOT_FOUND;
}

static DecisionTreeErrors getNewNodeInitedWithInput(DecisionTree* tree, const char* inputMessage, size_t* newNodeInd, bool isObj) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(inputMessage);
    IF_ARG_NULL_RETURN(newNodeInd);

    size_t newObjectIndex = 0;
    IF_ERR_RETURN(getNewNode(tree, &newObjectIndex));

    printf("%s", inputMessage);
    IF_ERR_RETURN(readStringToNodesData(tree, &tree->memBuff[newObjectIndex], isObj));
    LOG_DEBUG_VARS(newObjectIndex, tree->memBuff[newObjectIndex].data);

    *newNodeInd = newObjectIndex;
    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors addNewObjectToDecisionTree(DecisionTree* tree, Node* parent, Node* wronglyGuessedObj) {
    IF_ARG_NULL_RETURN(tree);
    //IF_ARG_NULL_RETURN(wronglyGuessedObj);

    size_t newObjInd      = 0;
    IF_ERR_RETURN(getNewNodeInitedWithInput(tree, "What's your object name: ", &newObjInd, true));
    if (wronglyGuessedObj == NULL) { // tree was empty
        tree->root = newObjInd;
        return DECISION_TREE_STATUS_OK;
    }

    size_t newQuestionInd = 0;
    IF_ERR_RETURN(getNewNodeInitedWithInput(tree,
                    "What differs your object from wrongly guessed object (some quality): ", &newQuestionInd, false));

    size_t bruh = wronglyGuessedObj->memBuffIndex;
    Node* node  = &tree->memBuff[newQuestionInd];
    if (parent == NULL) {
        LOG_ERROR("no parent");
        tree->root = newQuestionInd;
        node->parent = 0;
    } else {
        if (bruh == parent->left)
            parent->left  = newQuestionInd;
        else
            parent->right = newQuestionInd;
        node->parent = parent->memBuffIndex;
    }

    node->left  = bruh;
    node->right = newObjInd;
    tree->memBuff[bruh].parent = newQuestionInd;
    tree->memBuff[newObjInd].parent = newQuestionInd;

    LOG_DEBUG_VARS(newQuestionInd, bruh, newObjInd, tree->root);

    return DECISION_TREE_STATUS_OK;
}

static bool isLeaf(const DecisionTree* tree, size_t currentNodeInd) {
    if (!currentNodeInd)
        return true;

    assert(currentNodeInd < tree->memBuffSize);
    Node node = tree->memBuff[currentNodeInd];
    return (node.left == 0 && node.right == 0);
}

DecisionTreeErrors tryToGuessObject(DecisionTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    if (tree->root == 0) { // tree is empty, no guesses can be made
        printf("Decision tree is empty.\n");
        IF_ERR_RETURN(addNewObjectToDecisionTree(tree, NULL, NULL));
        LOG_DEBUG_VARS(tree->root);
        LOG_DEBUG_VARS(tree->memBuff[tree->root].data);
        return DECISION_TREE_STATUS_OK;
    }

    size_t currentNodeInd = tree->root;
    size_t parentInd = 0;
    while (!isLeaf(tree, currentNodeInd)) {
        assert(currentNodeInd < tree->memBuffSize);
        Node node = tree->memBuff[currentNodeInd];

        bool isToLeftSon = false;
        IF_ERR_RETURN(askQuestion(tree, &node, &isToLeftSon));
        size_t next = (isToLeftSon ? node.left : node.right);
        if (next == 0) // ASK: cringe?
            break;
        parentInd = currentNodeInd;
        currentNodeInd = next;
    }
    Node* parent = parentInd == 0 ? NULL : &tree->memBuff[parentInd];

    assert(currentNodeInd < tree->memBuffSize);
    Node* node = &tree->memBuff[currentNodeInd];
    node->parent = parentInd;
    LOG_DEBUG_VARS(currentNodeInd, parentInd);

    bool isCorrGuess = false;
    IF_ERR_RETURN(isObjectGuessedCorrectly(tree, node, &isCorrGuess));
    LOG_DEBUG_VARS(isCorrGuess);
    if (isCorrGuess) {
        return DECISION_TREE_STATUS_OK;
    }

    IF_ERR_RETURN(addNewObjectToDecisionTree(tree, parent, node));

    return DECISION_TREE_STATUS_OK;
}









static size_t getNodesDepth(const DecisionTree* tree, const Node* start) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(start);
    size_t currentNodeInd = start->memBuffIndex;
    size_t depth = 0;

    while (currentNodeInd != 0) {
        ++depth;
        assert(currentNodeInd < tree->memBuffSize);
        //LOG_DEBUG_VARS(depth, currentNodeInd, tree->memBuff[currentNodeInd].parent);
        currentNodeInd = tree->memBuff[currentNodeInd].parent;
        //LOG_DEBUG_VARS(currentNodeInd);
    }

    return depth;
}

static DecisionTreeErrors fillPathArrayFromRootToNode(const DecisionTree* tree, const Node* start, size_t pathLen, size_t* path) {
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

    return DECISION_TREE_STATUS_OK;
}

static DecisionTreeErrors printPathArrayFromRootToNode(const DecisionTree* tree,
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

    return DECISION_TREE_STATUS_OK;
}

static DecisionTreeErrors getPathToDecisionTreeNode(const DecisionTree* tree, const Node* node, size_t* pathLen, size_t** path) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path);
    IF_ARG_NULL_RETURN(pathLen);

    *pathLen = getNodesDepth(tree, node);
    LOG_DEBUG_VARS(pathLen);

    *path = (size_t*)calloc(*pathLen, sizeof(size_t));
    LOG_DEBUG_VARS(*pathLen);
    IF_NOT_COND_RETURN(*path != NULL,
                       DECISION_TREE_MEMORY_ALLOCATION_ERROR);

    IF_ERR_RETURN(fillPathArrayFromRootToNode(tree, node, *pathLen, *path));

    return DECISION_TREE_STATUS_OK;
}

static DecisionTreeErrors getPathToObjByName(const DecisionTree* tree, const char* objName, size_t* pathLen, size_t** path) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(objName);
    IF_ARG_NULL_RETURN(pathLen);
    IF_ARG_NULL_RETURN(path);

    Node node = {};
    IF_ERR_RETURN(getDecisionTreeNodeByObjName(tree, objName, &node));
    LOG_DEBUG_VARS(node.data, objName);

    IF_ERR_RETURN(getPathToDecisionTreeNode(tree, &node, pathLen, path));

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors printPathToObjByName(const DecisionTree* tree, const char* objName) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(objName);

//     Node node = {};
//     IF_ERR_RETURN(getDecisionTreeNodeByObjName(tree, objName, &node));
//     LOG_DEBUG_VARS(node.data, objName);
//
//     size_t depth = getNodesDepth(tree, &node);
//     LOG_DEBUG_VARS(depth);
//     size_t* path = (size_t*)calloc(depth, sizeof(size_t));
//     IF_NOT_COND_RETURN(path != NULL,
//                        DECISION_TREE_MEMORY_ALLOCATION_ERROR);
//
//     LOG_DEBUG_VARS("okey");
//     IF_ERR_RETURN(fillPathArrayFromRootToNode(tree, &node, depth, path));

    size_t* path = NULL;
    size_t pathLen = 0;
    IF_ERR_RETURN(getPathToObjByName(tree, objName, &pathLen, &path));

    LOG_DEBUG_VARS("biba");
    IF_ERR_RETURN(printPathArrayFromRootToNode(tree, pathLen, path, true));
    FREE(path);

    return DECISION_TREE_STATUS_OK;
}







static const char* trimBeginningOfLine(const char* line) {
    assert(line != NULL);
    const char* ptr = line;
    // TODO: add delims const string
    while (*ptr == '\t' || *ptr == ' ')
        ++ptr;

    return ptr;
}

DecisionTreeErrors recursiveSaveOfTreeInFile(DecisionTree* tree, size_t nodeInd, size_t depth, FILE* file) {
    if (!nodeInd)
        return DECISION_TREE_STATUS_OK;

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
    IF_ERR_RETURN(recursiveSaveOfTreeInFile(tree, node.left, depth + 1, file));
    IF_ERR_RETURN(recursiveSaveOfTreeInFile(tree, node.right, depth + 1, file));
    fprintf(file, "%s}\n", tabs);

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors saveDecisionTreeToFile(DecisionTree* tree, const char* fileName) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(fileName);

    FILE* file = fopen(fileName, "w");
    IF_NOT_COND_RETURN(file != NULL, DECISION_TREE_FILE_OPENING_ERROR);

    IF_ERR_RETURN(recursiveSaveOfTreeInFile(tree, tree->root, 0, file));

    return DECISION_TREE_STATUS_OK;
}

static DecisionTreeErrors addNewNodeForFile(DecisionTree* tree, Node** node, size_t len,
                                            const char* tmp, size_t preInd) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(node);
    LOG_DEBUG("creating new node");

    size_t newNodeIndex = 0;
    getNewNode(tree, &newNodeIndex);
    *node = &tree->memBuff[newNodeIndex];

    (*node)->data = (char*)calloc(len, sizeof(char));
    assert((*node)->data != NULL);
    IF_ERR_RETURN(checkThatObjNotInTree(tree, tmp));
    strcpy((*node)->data, tmp);

    if (tree->root == 0)
        tree->root = (*node)->memBuffIndex;
    if (preInd != 0) {
        (*node)->parent = preInd;
        assert(preInd < tree->memBuffSize);
        Node* pre = &tree->memBuff[preInd];

        if (pre->left == 0)
            pre->left = (*node)->memBuffIndex;
        else
            pre->right = (*node)->memBuffIndex;
    }

    IF_ERR_RETURN(dumpDecisionTree(tree));
    IF_NOT_COND_RETURN(!doesStringContainBreakChar((*node)->data),
                        DECISION_TREE_INVALID_INPUT_STRING);
}

DecisionTreeErrors readDecisionTreeFromFile(DecisionTree* tree, const char* fileName) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(fileName);

    FILE* file = fopen(fileName, "r");
    IF_NOT_COND_RETURN(file != NULL, DECISION_TREE_FILE_OPENING_ERROR);

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

    return DECISION_TREE_STATUS_OK;
}








static DecisionTreeErrors dumpDecisionTreeInConsole(const DecisionTree* tree, size_t nodeIndex,
                                                    char** outputBuffer) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(outputBuffer);
    IF_ARG_NULL_RETURN(*outputBuffer);

    // ASK: is strcat faster than sprintf
    if (nodeIndex == 0) {
        strncat(*outputBuffer, "?", OUTPUT_BUFFER_SIZE);
        ++(*outputBuffer);
        return DECISION_TREE_STATUS_OK;
    }

    strncat(*outputBuffer, "(", OUTPUT_BUFFER_SIZE);
    ++(*outputBuffer);

    //LOG_DEBUG_VARS(nodeIndex, tree->memBuffSize);
    assert(nodeIndex < tree->memBuffSize);
    Node node = tree->memBuff[nodeIndex];
    IF_ERR_RETURN(dumpDecisionTreeInConsole(tree, node.left, outputBuffer));
    (*outputBuffer) += snprintf(*outputBuffer, OUTPUT_BUFFER_SIZE, " %s ", node.data);

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

    for (size_t nodeInd = 0; nodeInd < 5; ++nodeInd) {
        Node node = tree->memBuff[nodeInd];
        const char* data = node.data;
        size_t parent = node.parent;
        size_t left = node.left;
        size_t right = node.right;
        LOG_DEBUG_VARS(nodeInd, data, parent, left, right);
        DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&tree->dumper, nodeInd, data, parent, left, right);
    }

    char* targetPtr = outputBuffer;
    IF_ERR_RETURN(dumpDecisionTreeInConsole(tree, tree->root, &targetPtr));
    LOG_DEBUG(outputBuffer);
    DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(&tree->dumper, outputBuffer);
    FREE(outputBuffer);

    // FIXME: add dumper err check
    DUMPER_ERR_CHECK(dumperDumpDecisionTree(&tree->dumper, tree, tree->freeNodeIndex));

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors openImageOfCurrentStateDecisionTree(DecisionTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    // first we need to create image of current state of tree
    IF_ERR_RETURN(dumpDecisionTree(tree));

    // FIXME: be carefull with nasty commands
    const char* fileName = getLastImageFileName((Dumper*)&tree->dumper);
    const size_t TMP_LEN = 100;
    char tmp[TMP_LEN] = {};
    snprintf(tmp, TMP_LEN, "xdg-open %s", fileName);
    system(tmp);

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors dumpCommonPathOf2Objects(DecisionTree* tree, const char* objName1, const char* objName2) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(objName1);
    IF_ARG_NULL_RETURN(objName2);

    size_t*   path1 = NULL;
    size_t pathLen1 = 0;
    IF_ERR_RETURN(getPathToObjByName(tree, objName1, &pathLen1, &path1));

    size_t*   path2 = NULL;
    size_t pathLen2 = 0;
    IF_ERR_RETURN(getPathToObjByName(tree, objName2, &pathLen2, &path2));

    // FIXME: add dumper err check
    DUMPER_ERR_CHECK(dumperDumpDecisionTreeDrawCommonPathes(&tree->dumper, tree, pathLen1, path1, pathLen2, path2));

    FREE(path1);
    FREE(path2);

    return DECISION_TREE_STATUS_OK;
}




DecisionTreeErrors destructDecisionTree(DecisionTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    for (size_t nodeInd = 0; nodeInd < tree->memBuffSize; ++nodeInd) {
        FREE(tree->memBuff[nodeInd].data);
    }
    FREE(tree->memBuff);

    tree->memBuffSize   = 0;
    tree->freeNodeIndex = 0;
    dumperDestructor(&tree->dumper);

    return DECISION_TREE_STATUS_OK;
}









const char* TERMINAL_HELP_MESSAGE =
"help message\n"
"--help shows this message\n"
"openTreeImgState opens image of current state of decision tree\n"
"guess asks questions and tries to guess object, if not found, adds object to the tree\n"
"definition prints definition of an object with a given name\n"
;

const char* QUIT_COMMAND = "quit";

DecisionTreeErrors terminalCmdHelp(DecisionTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    printf("%s\n", TERMINAL_HELP_MESSAGE);

    return DECISION_TREE_STATUS_OK;
}

const size_t BUFFER_SIZE = 100;

char* printInputMessageAndReadString(const char* inputMessage) {
    assert(inputMessage != NULL);
    printf("%s", inputMessage);

    char* buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
    assert(buffer != NULL);
    fgets(buffer, BUFFER_SIZE, stdin);
    size_t len = strlen(buffer);
    assert(len >= 1);
    buffer[len - 1] = '\0';
    LOG_DEBUG_VARS(buffer);

    return buffer;
}

DecisionTreeErrors terminalCmdPrintDefinition(DecisionTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    // TODO: remove copypaste
    char* buffer = printInputMessageAndReadString("print object name: ");
    IF_ERR_RETURN(printPathToObjByName(tree, buffer));
    FREE(buffer);

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors terminalCmdReadFromFile(DecisionTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    // TODO: remove copypaste
    char* buffer = printInputMessageAndReadString("print file name: ");
    IF_ERR_RETURN(readDecisionTreeFromFile(tree, buffer));
    FREE(buffer);

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors commonOrDifferenceHelperFunc(DecisionTree* tree, bool isShowSame) {
    IF_ARG_NULL_RETURN(tree);

    // TODO: remove copypaste
    char* objName1 = printInputMessageAndReadString("name of object_1: ");
    char* objName2 = printInputMessageAndReadString("name of object_2: ");
    assert(objName1 != NULL);
    assert(objName2 != NULL);

    // check if names are different

    size_t*   path1 = NULL;
    size_t pathLen1 = 0;
    IF_ERR_RETURN(getPathToObjByName(tree, objName1, &pathLen1, &path1));

    size_t*   path2 = NULL;
    size_t pathLen2 = 0;
    IF_ERR_RETURN(getPathToObjByName(tree, objName2, &pathLen2, &path2));

    size_t* cntArr = (size_t*)calloc(tree->memBuffSize, sizeof(size_t));
    IF_NOT_COND_RETURN(cntArr != NULL,
                       DECISION_TREE_MEMORY_ALLOCATION_ERROR);
    size_t* commonPath = (size_t*)calloc(pathLen1, sizeof(size_t));
    IF_NOT_COND_RETURN(commonPath != NULL,
                       DECISION_TREE_MEMORY_ALLOCATION_ERROR);

    for (int i = 0; i < pathLen1; ++i) {
        ++cntArr[path1[i]];
        LOG_DEBUG_VARS(i, path1[i]);
    }
    for (int i = 0; i < pathLen2; ++i) {
        ++cntArr[path2[i]];
        LOG_DEBUG_VARS(i, path2[i]);
    }

    size_t commonPathLen = 0;
    for (int i = 0; i + 1 < pathLen2; ++i) {
        if (cntArr[path2[i + 1]] != 2) {
            commonPath[commonPathLen] = path2[i];
            ++commonPathLen;
            break;
        }
        if (!isShowSame) continue;

        LOG_DEBUG_VARS(i, path2[i], path1[i]);
        commonPath[commonPathLen] = path2[i];
        ++commonPathLen;
    }
    LOG_DEBUG_VARS(objName1, objName2, commonPathLen);

    IF_ERR_RETURN(printPathArrayFromRootToNode(tree, commonPathLen, commonPath, !isShowSame));

    FREE(objName1);
    FREE(objName2);

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors terminalCmdShowCommonOf2Objects(DecisionTree* tree) {
    IF_ERR_RETURN(commonOrDifferenceHelperFunc(tree, true));
}

DecisionTreeErrors terminalCmdShowDifferenceOf2Objects(DecisionTree* tree) {
    IF_ERR_RETURN(commonOrDifferenceHelperFunc(tree, false));
}

typedef DecisionTreeErrors (*terminalCmdFuncPtr)(DecisionTree* tree);

struct TerminalCmd {
    const char*        cmdName;
    terminalCmdFuncPtr funcPtr;
};

#define CMD(commandName, commandFuncPtr) \
    {commandName, commandFuncPtr},

TerminalCmd arrayOfCommands[] = {
    #include "../include/plainTextTerminalCommandsForDecisionTree.in"
};

size_t NUM_OF_COMMANDS = sizeof(arrayOfCommands) / sizeof(*arrayOfCommands);

DecisionTreeErrors executeCommand(DecisionTree* tree, const char* commandName, bool* isQuit) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(commandName);

    if (strcmp(commandName, QUIT_COMMAND) == 0) {
        *isQuit = true;
        return DECISION_TREE_STATUS_OK;
    }

    for (size_t commandInd = 0; commandInd < NUM_OF_COMMANDS; ++commandInd) {
        TerminalCmd command = arrayOfCommands[commandInd];
        if (strcmp(command.cmdName, commandName) == 0) {
            IF_ERR_RETURN((*command.funcPtr)(tree));
        }
    }

    return DECISION_TREE_STATUS_OK;
}

DecisionTreeErrors mainProgramWhileTrue(DecisionTree* tree) {
    IF_ARG_NULL_RETURN(tree);

    bool isQuit = false;
    char* buffer = NULL;
    while (!isQuit) {
        buffer = printInputMessageAndReadString("Print your command: ");
        executeCommand(tree, buffer, &isQuit);
    }

    return DECISION_TREE_STATUS_OK;
}










