#include <stdlib.h>
#include <inttypes.h>

#include "../include/akinatorLib.hpp"
#include "../../include/dumper.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, AKINATOR_INVALID_ARGUMENT, getAkinatorErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getAkinatorErrorMessage, AKINATOR_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getAkinatorErrorMessage)\

#define DUMPER_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getDumperErrorMessage, DUMPER_STATUS_OK, AKINATOR_DUMPER_ERROR);

#define BIN_TREE_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getTypicalBinaryTreeErrorMessage, TYPICAL_BIN_TREE_STATUS_OK, AKINATOR_BINARY_TREE_ERROR);


const size_t MIN_MEM_BUFF_SIZE  = 8;
const size_t OUTPUT_BUFFER_SIZE = 1 << 9;
const size_t MAX_OBJ_NAME_LEN   = 30;
const char   BREAK_CHAR         = '#';

const char* getStringFromVoidPtr(const void* voidPtr) {
    //assert(voidPtr != NULL);
    const char* ptr = (const char*)(voidPtr);
    if (ptr == NULL)
        return "";
    return (const char*)(voidPtr);
}

static const char* nodeDataPrinter(const void* num) {
    // LOG_DEBUG("node data printer");
    const char* buffer = getStringFromVoidPtr(num);
    return buffer;
}

void nodeDataReader(void** num, const char* line) {
    // LOG_DEBUG("node data reader");
    if (line == NULL) return;

    *num = calloc(strlen(line) + 1, sizeof(char));
    assert(*num != NULL);

    LOG_DEBUG_VARS(line, num);
    sscanf(line, "%[^\n]s", *num);
    // LOG_ERROR("---------------------");
    // LOG_DEBUG_VARS(num, line, (const char*)(*num));
}

static int cmpNodesByValue(const void* num, const void* num2) {
    //LOG_DEBUG_VARS("cmp of nodes", num, num2);
    const char* one = getStringFromVoidPtr(num);
    const char* two = getStringFromVoidPtr(num2);
    return strcmp(one, two);
}


AkinatorErrors constructAkinator(Akinator* akinator, Dumper* dumper) {
    IF_ARG_NULL_RETURN(akinator);

    BIN_TREE_ERR_CHECK(constructTypicalBinaryTree(
        &akinator->tree,
        dumper,
        nodeDataPrinter,
        nodeDataReader,
        cmpNodesByValue
    ));

    return AKINATOR_STATUS_OK;
}



static bool isAnswerOnQuestionYes() {
    printf("Print yes or no: ");

    const size_t ANS_BUFF_SIZE = 10;
    char answerBuff[ANS_BUFF_SIZE];
    fgets(answerBuff, ANS_BUFF_SIZE, stdin);

    LOG_DEBUG_VARS(answerBuff);
    return strcmp(answerBuff, "yes\n") == 0 ||
           strcmp(answerBuff, "\n")    == 0;
}



static AkinatorErrors askQuestion(const Akinator* akinator, const Node* node, bool* isToLeftSon) {
    IF_ARG_NULL_RETURN(akinator);
    IF_ARG_NULL_RETURN(node);

    // TODO: check that it's not leaf
    printf("%s\n", node->data);
    *isToLeftSon = !isAnswerOnQuestionYes();

    return AKINATOR_STATUS_OK;
}

static AkinatorErrors isObjectGuessedCorrectly(Akinator* akinator, const Node* node, bool* isCorrectGuess) {
    IF_ARG_NULL_RETURN(akinator);
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(isCorrectGuess);

    printf("Decision tree's guess is: \"%s\", is it your object?\n", node->data);
    *isCorrectGuess = isAnswerOnQuestionYes();

    return AKINATOR_STATUS_OK;
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

// static AkinatorErrors checkThatObjNotInBase(const Akinator* akinator, const char* objName) {
//     IF_ARG_NULL_RETURN(akinator);
//     IF_ARG_NULL_RETURN(objName);
//
//     // BIN_TREE_ERR_CHECK();
//
//
//
//     return AKINATOR_STATUS_OK;
// }

// static AkinatorErrors getAkinatorNodeByObjName(const Akinator* tree, const char* objName, Node* result) {
//     IF_ARG_NULL_RETURN(tree);
//     IF_ARG_NULL_RETURN(objName);
//     IF_ARG_NULL_RETURN(result);
//
//     for (size_t ind = 1; ind < tree->memBuffSize; ++ind) {
//         const char* const ptr = tree->memBuff[ind].data;
//         if (ptr != NULL && strcmp(ptr, objName) == 0) {
//             *result = tree->memBuff[ind];
//             return AKINATOR_STATUS_OK;
//         }
//     }
//
//     return DECISION_TREE_OBJ_NOT_FOUND;
// }

static AkinatorErrors readQuestionOrObjName(Akinator* akinator, const char* inputMessage, char** result, bool isObj) {
    IF_ARG_NULL_RETURN(akinator);
    IF_ARG_NULL_RETURN(inputMessage);
    IF_ARG_NULL_RETURN(result);

    printf("%s", inputMessage);
    char tmpBuffer[MAX_OBJ_NAME_LEN] = {};
    fgets(tmpBuffer, MAX_OBJ_NAME_LEN, stdin);

    size_t len = strlen(tmpBuffer);
    *result = (char*)calloc(len + 1, sizeof(char));
    LOG_DEBUG_VARS(len, tmpBuffer);
    IF_NOT_COND_RETURN(*result != NULL, AKINATOR_MEMORY_ALLOCATION_ERROR);
    memcpy(*result, tmpBuffer, len);
    LOG_DEBUG_VARS(result);

    (*result)[len - 1] = '\0';
    // TODO:

    return AKINATOR_STATUS_OK;
}

static AkinatorErrors addNewObjectToAkinator(Akinator* akinator, size_t parent, size_t wronglyGuessedObj) {
    IF_ARG_NULL_RETURN(akinator);
    //IF_ARG_NULL_RETURN(wronglyGuessedObj);

    size_t newObjInd      = 0;
    char* newObjNameBuff  = NULL;
    IF_ERR_RETURN(readQuestionOrObjName(akinator, "What's your object name: ", &newObjNameBuff, true));
    LOG_DEBUG_VARS(newObjNameBuff);
    if (wronglyGuessedObj == 0) { // tree was empty
        BIN_TREE_ERR_CHECK(addNewObjectToTypicalBinaryTree(&akinator->tree, 0, newObjNameBuff, false, &newObjInd));
        return AKINATOR_STATUS_OK;
    }

    size_t newQuestionInd = 0;
    char* questionBuff = NULL;
    IF_ERR_RETURN(readQuestionOrObjName(akinator,
                    "What differs your object from wrongly guessed object (some quality): ", &questionBuff, false));

    Node oldLeaf = {};
    BIN_TREE_ERR_CHECK(getBinTreeNodeByVertIndex(&akinator->tree, wronglyGuessedObj, &oldLeaf));

    bool isLeftSon = false;
    BIN_TREE_ERR_CHECK(isNodeAleftSonOfParent(&akinator->tree, parent, wronglyGuessedObj, &isLeftSon));
    BIN_TREE_ERR_CHECK(addNewObjectToTypicalBinaryTree(&akinator->tree, parent, questionBuff,
                                                       isLeftSon, &newQuestionInd));
    BIN_TREE_ERR_CHECK(addNewObjectToTypicalBinaryTree(&akinator->tree, newQuestionInd, oldLeaf.data,
                                                       true, &wronglyGuessedObj));
    BIN_TREE_ERR_CHECK(addNewObjectToTypicalBinaryTree(&akinator->tree, newQuestionInd, newObjNameBuff,
                                                       false, &newObjInd));

    LOG_DEBUG_VARS(newQuestionInd, newObjInd, wronglyGuessedObj);
    LOG_DEBUG_VARS(questionBuff, newObjNameBuff);

    return AKINATOR_STATUS_OK;
}

AkinatorErrors tryToGuessObject(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    bool isTreeEmpty = false;
    BIN_TREE_ERR_CHECK(isBinTreeEmpty(&akinator->tree, &isTreeEmpty));
    if (isTreeEmpty) { // tree is empty, no guesses can be made
        printf("Decision tree is empty.\n");
        IF_ERR_RETURN(addNewObjectToAkinator(akinator, NULL, NULL));
        return AKINATOR_STATUS_OK;
    }

    size_t parentInd = 0;
    size_t currentNodeInd = akinator->tree.root;
    while (true) {
        bool isLeaf = false;
        BIN_TREE_ERR_CHECK(isBinTreeNodeLeaf(&akinator->tree, currentNodeInd, &isLeaf));
        if (isLeaf) break;

        Node node = {};
        BIN_TREE_ERR_CHECK(getBinTreeNodeByVertIndex(&akinator->tree, currentNodeInd, &node));

        bool isToLeftSon = false;
        IF_ERR_RETURN(askQuestion(akinator, &node, &isToLeftSon));
        size_t next = (isToLeftSon ? node.left : node.right);
        if (next == 0) // ASK: cringe?
            break;
        parentInd = currentNodeInd;
        currentNodeInd = next;
    }

    Node node = {};
    BIN_TREE_ERR_CHECK(getBinTreeNodeByVertIndex(&akinator->tree, currentNodeInd, &node));

    bool isCorrGuess = false;
    IF_ERR_RETURN(isObjectGuessedCorrectly(akinator, &node, &isCorrGuess));
    LOG_DEBUG_VARS(isCorrGuess);
    if (isCorrGuess) {
        return AKINATOR_STATUS_OK;
    }

    // Node parent = {};
    // BIN_TREE_ERR_CHECK(getBinTreeNodeByVertIndex(&akinator->tree, parentInd, &node));
    IF_ERR_RETURN(addNewObjectToAkinator(akinator, parentInd, currentNodeInd));

    return AKINATOR_STATUS_OK;
}









AkinatorErrors dumpCommonPathOf2Objects(Akinator* akinator, const char* objName1, const char* objName2) {
    IF_ARG_NULL_RETURN(akinator);
    IF_ARG_NULL_RETURN(objName1);
    IF_ARG_NULL_RETURN(objName2);

    BIN_TREE_ERR_CHECK(dumpCommonPathOf2ObjectsByValue(&akinator->tree, objName1, objName2));

    return AKINATOR_STATUS_OK;
}

AkinatorErrors showDefinitionOfObject(Akinator* akinator, const char* objName) {
    IF_ARG_NULL_RETURN(akinator);
    IF_ARG_NULL_RETURN(objName);

    size_t*   path = NULL;
    size_t pathLen = 0;

    BIN_TREE_ERR_CHECK(getPathToObjByValue(&akinator->tree, objName, &pathLen, &path));

    LOG_DEBUG_VARS(pathLen);
    LOG_INFO("definition: ");
    for (size_t i = 0; i < pathLen; ++i)
        printf("elem : %d, %d\n", i, path[i]);

    // IF_ERR_RETURN(printPathArrayFromRootToNode(tree, pathLen, path, true));
    // fds
    FREE(path);

    return AKINATOR_STATUS_OK;
}

AkinatorErrors saveAkinatorsDecisionTreeToFile(Akinator* akinator, const char* fileName) {
    IF_ARG_NULL_RETURN(akinator);
    IF_ARG_NULL_RETURN(fileName);

    BIN_TREE_ERR_CHECK(saveTypicalBinaryTreeToFile(&akinator->tree, fileName));

    return AKINATOR_STATUS_OK;
}

AkinatorErrors readAkinatorsDecisionTreeFromFile(Akinator* akinator, const char* fileName) {
    IF_ARG_NULL_RETURN(akinator);
    IF_ARG_NULL_RETURN(fileName);

    BIN_TREE_ERR_CHECK(readTypicalBinaryTreeFromFile(&akinator->tree, fileName));

    return AKINATOR_STATUS_OK;
}

AkinatorErrors dumpAkinator(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    LOG_DEBUG("--------------------------------------\n");
    LOG_DEBUG("akinator struct:\n");
    BIN_TREE_ERR_CHECK(dumpTypicalBinaryTree(&akinator->tree));
    LOG_DEBUG("--------------------------------------\n");

    return AKINATOR_STATUS_OK;
}

AkinatorErrors showImageOfAkinatorsDecisionTree(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    BIN_TREE_ERR_CHECK(openImageOfCurrentStateTypicalBinaryTree(&akinator->tree));

    return AKINATOR_STATUS_OK;
}

AkinatorErrors destructAkinator(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    BIN_TREE_ERR_CHECK(destructTypicalBinaryTree(&akinator->tree));

    return AKINATOR_STATUS_OK;
}



