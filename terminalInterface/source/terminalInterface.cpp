#include <stdlib.h>
#include <strings.h>

#include "../include/terminalInterface.hpp"
#include "../../akinatorLib/include/akinatorLib.hpp"
#include "../include/nodeDataReadAndWrite.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, TERMINAL_INTERFACE_INVALID_ARGUMENT, getTerminalInterfaceErrorMessage)

#define IF_ERR_RETURN(error) \
    do {\
        COMMON_IF_ERR_RETURN(error, getTerminalInterfaceErrorMessage, TERMINAL_INTERFACE_STATUS_OK);\
        /*DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE((Dumper*)&list->dumper, #error);\*/\
    } while(0) \

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getTerminalInterfaceErrorMessage)\

#define DUMPER_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getDumperErrorMessage, DUMPER_STATUS_OK, TERMINAL_INTERFACE_DUMPER_ERROR);

#define BIN_TREE_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getTypicalBinaryTreeErrorMessage, TYPICAL_BIN_TREE_STATUS_OK, TERMINAL_INTERFACE_BIN_TREE_ERROR);

#define AKINATOR_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getAkinatorErrorMessage, AKINATOR_STATUS_OK, TERMINAL_INTERFACE_AKINATOR_ERROR);



TerminalInterfaceErrors isAnswerOnQuestionYes(bool* isYes) {
    IF_ARG_NULL_RETURN(isYes);

    printf("Print yes or no: ");

    const size_t ANS_BUFF_SIZE = 10;
    char answerBuff[ANS_BUFF_SIZE];
    fgets(answerBuff, ANS_BUFF_SIZE, stdin);

    LOG_DEBUG_VARS(answerBuff);
    *isYes  = strcmp(answerBuff, "yes\n") == 0 ||
              strcmp(answerBuff, "\n")    == 0;
    LOG_DEBUG_VARS(*isYes);

    return TERMINAL_INTERFACE_STATUS_OK;
}

// if no, than to left son, otherwise (yes) to right son
TerminalInterfaceErrors askQuestionFromNodesData(const Node* node, bool *isToLeftSon) {
    IF_ARG_NULL_RETURN(node);

    // TODO: check that it's not leaf
    const char* question = (*nodeDataPrinter)(node->data);
    printf("%s\n", question);
    //LOG_DEBUG_VARS("ask question ffrom nodes data", (const char*)node->data);
    IF_ERR_RETURN(isAnswerOnQuestionYes(isToLeftSon));
    *isToLeftSon = !(*isToLeftSon);

    return TERMINAL_INTERFACE_STATUS_OK;
}

TerminalInterfaceErrors isObjectGuessedCorrectly(const Node* node, bool* isCorrectGuess) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(isCorrectGuess);

    printf("Decision tree's guess is: \"%s\", is it your object?\n", node->data);
    IF_ERR_RETURN(isAnswerOnQuestionYes(isCorrectGuess));

    return TERMINAL_INTERFACE_STATUS_OK;
}



const char* satisfiesCondition[] = {
    "is",
    "satisfies",
    "can be called",
    "correspond to condition"
};

const char* doesnotSatisfyCondition[] = {
    "is not",
    "doesn't satisfy",
    "cannot be called",
    "doesn't correspond to condition",
};

TerminalInterfaceErrors getNodesDataArray(const TypicalBinaryTree* tree, size_t* path, size_t pathLen, const char*** nodesData) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path);
    IF_ARG_NULL_RETURN(nodesData);

    *nodesData = (const char**)calloc(pathLen, sizeof(const char*));
    IF_NOT_COND_RETURN(*nodesData != NULL, TERMINAL_INTERFACE_MEMORY_ALLOCATION_ERROR);

    for (size_t vertInd = 0; vertInd < pathLen; ++vertInd) {
        Node node = {};
        BIN_TREE_ERR_CHECK(getBinTreeNodeByVertIndex(tree, path[vertInd], &node));
        (*nodesData)[vertInd] = (*nodeDataPrinter)(node.data);
        LOG_DEBUG_VARS(vertInd, (*nodesData)[vertInd]);
    }

    return TERMINAL_INTERFACE_STATUS_OK;
}

// FIXME: make array of chars or of bools, size_t is too big
TerminalInterfaceErrors getNodesAnswerOnQuestionsArr(const TypicalBinaryTree* tree, size_t* path, size_t pathLen, size_t** answers) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path);
    IF_ARG_NULL_RETURN(answers);

    *answers = (size_t*)calloc(pathLen, sizeof(size_t));
    IF_NOT_COND_RETURN(*answers != NULL, TERMINAL_INTERFACE_MEMORY_ALLOCATION_ERROR);

    size_t prevNodeInd = path[0];
    for (size_t vertInd = 1; vertInd < pathLen; ++vertInd) {
        size_t curNodeInd = path[vertInd];
        bool isLeftSon = false;
        BIN_TREE_ERR_CHECK(isNodeAleftSonOfParent(tree, prevNodeInd, curNodeInd, &isLeftSon));
        (*answers)[vertInd - 1] = !isLeftSon; // isLeftSon = no, !isLeftSon = yes
        prevNodeInd = path[vertInd];
    }

    return TERMINAL_INTERFACE_STATUS_OK;
}

TerminalInterfaceErrors getGoodSentencesFromNodesData(const TypicalBinaryTree* tree, size_t pathLen, size_t* path,
                                                      char*** result) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path);
    IF_ARG_NULL_RETURN(result);

    const char** nodesData = NULL;
    size_t* answersArr     = NULL;
    IF_ERR_RETURN(getNodesDataArray(tree, path, pathLen, &nodesData));
    IF_ERR_RETURN(getNodesAnswerOnQuestionsArr(tree, path, pathLen, &answersArr));

    *result = (char**)calloc(pathLen, sizeof(const char*));
    IF_NOT_COND_RETURN(*result != NULL, TERMINAL_INTERFACE_MEMORY_ALLOCATION_ERROR);

    size_t prevNodeInd = path[0];
    for (size_t vertInd = 1; vertInd < pathLen + 1; ++vertInd) {
        const size_t TMP_BUFF_SIZE = 300;
        char tmpBuff[TMP_BUFF_SIZE] = {};
        if (vertInd < pathLen) {
            const char* ans = answersArr[vertInd - 1] ? "no" : "yes";
            snprintf(tmpBuff, TMP_BUFF_SIZE, "Answer on question: \"%-20s\", is: \"%s\"", nodesData[vertInd - 1], ans);
        } else {
            strcpy(tmpBuff, nodesData[vertInd - 1]);
        }

        size_t tmpBuffLen = strlen(tmpBuff);
        (*result)[vertInd - 1] = (char*)calloc(tmpBuffLen + 10, sizeof(char));
        IF_NOT_COND_RETURN((*result)[vertInd - 1],
                            TERMINAL_INTERFACE_MEMORY_ALLOCATION_ERROR);
        strcpy((*result)[vertInd - 1], tmpBuff);

        if (vertInd < pathLen)
            prevNodeInd = path[vertInd];
    }

    FREE(nodesData);

    LOG_DEBUG_VARS("sentences:");
    for (size_t i = 0; i < pathLen; ++i)
        LOG_DEBUG_VARS(i, (*result)[i]);

    return TERMINAL_INTERFACE_STATUS_OK;
}

TerminalInterfaceErrors getSentenceDefinitionFromPath(const TypicalBinaryTree* tree, size_t pathLen, size_t* path,
                                                      char** sentence, bool isEndOutput) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path);
    IF_ARG_NULL_RETURN(sentence);

    const size_t TMP_BUFF_SIZE = 1000;
    char tmpBuff[TMP_BUFF_SIZE] = {};
    char* tmpBuffPtr = tmpBuff;

    const char** nodesData = NULL;
    size_t* answersArr     = NULL;
    IF_ERR_RETURN(getNodesDataArray(tree, path, pathLen, &nodesData));
    IF_ERR_RETURN(getNodesAnswerOnQuestionsArr(tree, path, pathLen, &answersArr));

    if (isEndOutput)
        tmpBuffPtr += snprintf(tmpBuffPtr, TMP_BUFF_SIZE - (tmpBuffPtr - tmpBuff), "%s ", nodesData[pathLen - 1]);

    srand(time(NULL)); // FIXME: bruh
    for (size_t nodeInd = 0; nodeInd + isEndOutput < pathLen; ++nodeInd) {
        const size_t SMALL_TMP_BUFF_SIZE  = 200;
        char verb[SMALL_TMP_BUFF_SIZE]    = {};
        char quality[SMALL_TMP_BUFF_SIZE] = {};

        if (answersArr[nodeInd]) { // yes
            size_t satisfiesConditionArrLen = sizeof(satisfiesCondition) / sizeof(*satisfiesCondition);
            LOG_DEBUG_VARS(satisfiesConditionArrLen);
            size_t arrInd = rand() % satisfiesConditionArrLen;

            strcpy(verb, satisfiesCondition[arrInd]);
        } else { // no
            size_t doesnotSatisfyConditionArrLen = sizeof(doesnotSatisfyCondition) / sizeof(*doesnotSatisfyCondition);
            LOG_DEBUG_VARS(doesnotSatisfyConditionArrLen);
            size_t arrInd = rand() % doesnotSatisfyConditionArrLen;

            strcpy(verb, doesnotSatisfyCondition[arrInd]);
        }

        snprintf(quality, SMALL_TMP_BUFF_SIZE, "%s \"%s\"", verb, nodesData[nodeInd]);
        LOG_DEBUG_VARS(quality);
        tmpBuffPtr += snprintf(tmpBuffPtr, TMP_BUFF_SIZE - (tmpBuffPtr - tmpBuff), "%s, ", quality);
    }

    size_t buffLen = strlen(tmpBuff);
    *sentence = (char*)calloc(buffLen + 1, sizeof(char));
    IF_NOT_COND_RETURN(*sentence != NULL, TERMINAL_INTERFACE_MEMORY_ALLOCATION_ERROR);
    strcpy(*sentence, tmpBuff);
    LOG_DEBUG_VARS(tmpBuff);
    //printf("%s\n", tmpBuff);

    return TERMINAL_INTERFACE_STATUS_OK;
}

TerminalInterfaceErrors printDefinitionOfObject(const TypicalBinaryTree* tree, size_t pathLen, size_t* path) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path);

    char** sentences = NULL;
    IF_ERR_RETURN(getGoodSentencesFromNodesData(tree, pathLen, path, &sentences));

    // last name should be name of an object (because it's a leaf)
    printf("Definition for object: %s\n", sentences[pathLen - 1]);
    size_t prevNodeInd = path[0];
    for (size_t vertInd = 0; vertInd + 1 < pathLen; ++vertInd) {
        printf("    %s\n", sentences[vertInd]);
    }

    char* sentence = NULL;
    IF_ERR_RETURN(getSentenceDefinitionFromPath(tree, pathLen, path, &sentence, true));
    LOG_DEBUG_VARS(sentence);
    printf("%s\n", sentence);

    for (size_t sentenceInd = 0; sentenceInd < pathLen; ++sentenceInd)
        FREE(sentences[sentenceInd]);
    FREE(sentences);

    return TERMINAL_INTERFACE_STATUS_OK;
}


static const char* TERMINAL_HELP_MESSAGE =
"help message:\n"
"* help:         shows this message\n"
"* show:         opens image of current state of decision akinator\n"
"* guess:        asks questions and tries to guess object, if not found, adds object to the akinator\n"
"* read:         reads decision akinator from file\n"
"* save:         saves decision akinator to file\n"
"* definition:   prints definition of an object with a given name\n"
"* same:         prints same qualities for 2 objects\n"
"* diff:         prints different quality for 2 objects\n"
"* show diff:    shows image different qualities for 2 objects\n"
"* quit:         ends main game loop\n"
"* diffSame:     shows same and different qualities of 2 objects\n"
;

static const char* QUIT_COMMAND = "quit";
static const size_t BUFFER_SIZE = 100;

static TerminalInterfaceErrors terminalCmdShowHelpMessage(Akinator* akinator) {
    printf("%s\n", TERMINAL_HELP_MESSAGE);
    return TERMINAL_INTERFACE_STATUS_OK;
}

static char* printInputMessageAndReadString(const char* inputMessage) {
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

static TerminalInterfaceErrors terminalCmdPrintDefinition(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    // TODO: remove copypaste
    char* buffer = printInputMessageAndReadString("print object name: ");
    AKINATOR_ERR_CHECK(showDefinitionOfObject(akinator, buffer));
    FREE(buffer);

    return TERMINAL_INTERFACE_STATUS_OK;
}

static TerminalInterfaceErrors terminalCmdReadFromFile(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    // TODO: remove copypaste
    char* buffer = printInputMessageAndReadString("print file name: ");
    AKINATOR_ERR_CHECK(readAkinatorsDecisionTreeFromFile(akinator, buffer));
    FREE(buffer);

    return TERMINAL_INTERFACE_STATUS_OK;
}

static TerminalInterfaceErrors terminalCmdSaveToFile(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    // TODO: remove copypaste
    char* buffer = printInputMessageAndReadString("print file name: ");
    AKINATOR_ERR_CHECK(saveAkinatorsDecisionTreeToFile(akinator, buffer));
    FREE(buffer);

    return TERMINAL_INTERFACE_STATUS_OK;
}

static TerminalInterfaceErrors commonOrDifferenceHelperFunc(TypicalBinaryTree* tree, bool isShowSame) {
    IF_ARG_NULL_RETURN(tree);

    // TODO: remove copypaste
    char* objName1 = printInputMessageAndReadString("name of object_1: ");
    char* objName2 = printInputMessageAndReadString("name of object_2: ");
    assert(objName1 != NULL);
    assert(objName2 != NULL);

    IF_NOT_COND_RETURN(strcmp(objName1, objName2) != 0,
                       TERMINAL_INTERFACE_SAME_OBJ_NAMES);

    size_t*   path1 = NULL;
    size_t pathLen1 = 0;
    BIN_TREE_ERR_CHECK(getPathToObjByValue(tree, objName1, &pathLen1, &path1));

    size_t*   path2 = NULL;
    size_t pathLen2 = 0;
    BIN_TREE_ERR_CHECK(getPathToObjByValue(tree, objName2, &pathLen2, &path2));

    Node obj1 = {};
    Node obj2 = {};
    BIN_TREE_ERR_CHECK(getTypicalBinaryTreeNodeByValue(tree, objName1, &obj1));
    BIN_TREE_ERR_CHECK(getTypicalBinaryTreeNodeByValue(tree, objName2, &obj2));

    size_t* cntArr = NULL;
    BIN_TREE_ERR_CHECK(getCommonPathCntArray(tree, obj1.memBuffIndex, obj2.memBuffIndex, &cntArr));

    size_t* commonPath = (size_t*)calloc(pathLen1, sizeof(size_t));
    IF_NOT_COND_RETURN(commonPath != NULL,
                       TERMINAL_INTERFACE_MEMORY_ALLOCATION_ERROR);

    size_t commonPathLen = 0;
    for (size_t i = 0; i + 1 < pathLen2; ++i) {
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

    char** sentences = NULL;
    IF_ERR_RETURN(getGoodSentencesFromNodesData(tree, commonPathLen, commonPath, &sentences));

    size_t prevNodeInd = commonPath[0];
    if (isShowSame) {
        printf("Same properties of objects:\n");
    } else {
        printf("Different property of 2 objects:\n");
    }

    for (size_t vertInd = 0; vertInd + isShowSame < commonPathLen; ++vertInd) {
        printf("    %s\n", sentences[vertInd]);
    }

    for (size_t sentenceInd = 0; sentenceInd < commonPathLen; ++sentenceInd)
        FREE(sentences[sentenceInd]);
    FREE(sentences);

    FREE(objName1);
    FREE(objName2);
    FREE(path1);
    FREE(path2);
    FREE(cntArr);
    FREE(commonPath);

    return TERMINAL_INTERFACE_STATUS_OK;
}

static TerminalInterfaceErrors terminalCmdCommonAndDiffOf2Objects(Akinator* akinator) {
    // TODO: remove copypaste
    char* objName1 = printInputMessageAndReadString("name of object_1: ");
    char* objName2 = printInputMessageAndReadString("name of object_2: ");
    assert(objName1 != NULL);
    assert(objName2 != NULL);

    IF_NOT_COND_RETURN(strcmp(objName1, objName2) != 0,
                       TERMINAL_INTERFACE_SAME_OBJ_NAMES);

    size_t*   path1 = NULL;
    size_t pathLen1 = 0;
    BIN_TREE_ERR_CHECK(getPathToObjByValue(&akinator->tree, objName1, &pathLen1, &path1));

    size_t*   path2 = NULL;
    size_t pathLen2 = 0;
    BIN_TREE_ERR_CHECK(getPathToObjByValue(&akinator->tree, objName2, &pathLen2, &path2));

    Node obj1 = {};
    Node obj2 = {};
    BIN_TREE_ERR_CHECK(getTypicalBinaryTreeNodeByValue(&akinator->tree, objName1, &obj1));
    BIN_TREE_ERR_CHECK(getTypicalBinaryTreeNodeByValue(&akinator->tree, objName2, &obj2));

    size_t* cntArr = NULL;
    BIN_TREE_ERR_CHECK(getCommonPathCntArray(&akinator->tree, obj1.memBuffIndex, obj2.memBuffIndex, &cntArr));

    size_t* commonPath = (size_t*)calloc(pathLen1, sizeof(size_t));
    IF_NOT_COND_RETURN(commonPath != NULL,
                       TERMINAL_INTERFACE_MEMORY_ALLOCATION_ERROR);
    size_t* diffPath1 = (size_t*)calloc(pathLen1, sizeof(size_t));
    IF_NOT_COND_RETURN(diffPath1 != NULL,
                       TERMINAL_INTERFACE_MEMORY_ALLOCATION_ERROR);
    size_t* diffPath2 = (size_t*)calloc(pathLen2, sizeof(size_t));
    IF_NOT_COND_RETURN(diffPath2 != NULL,
                       TERMINAL_INTERFACE_MEMORY_ALLOCATION_ERROR);

    size_t commonPathLen = 0, lca = -1;
    for (size_t i = 0; i + 1 < pathLen2; ++i) {
        if (cntArr[path2[i + 1]] != 2) {
            lca = path2[i];
            // commonPath[commonPathLen] = path2[i];
            // ++commonPathLen;
            break;
        }

        LOG_DEBUG_VARS(i, path2[i], path1[i]);
        commonPath[commonPathLen] = path2[i];
        ++commonPathLen;
    }
    LOG_DEBUG_VARS(objName1, objName2, commonPathLen);

    printf("same properties of %s and %s:\n", objName1, objName2);
    char* sentence = NULL;
    IF_ERR_RETURN(getSentenceDefinitionFromPath(&akinator->tree, commonPathLen, commonPath, &sentence, false));
    LOG_DEBUG_VARS(sentence);
    printf("%s\n\n", sentence);
    FREE(sentence);












    size_t diffPath1_len = 0;
    size_t diffPath2_len = 0;
    for (size_t i = 0; i < pathLen1; ++i) {
        size_t nod = path1[i];
        if (cntArr[nod] == 1 || nod == lca)
            diffPath1[diffPath1_len++] = nod;
    }
    for (size_t i = 0; i < pathLen2; ++i) {
        size_t nod = path2[i];
        if (cntArr[nod] == 1 || nod == lca)
            diffPath2[diffPath2_len++] = nod;
    }

    printf("diff properties of %s and %s:\n", objName1, objName2);
    IF_ERR_RETURN(getSentenceDefinitionFromPath(&akinator->tree, diffPath1_len - 1, diffPath1, &sentence, false));
    LOG_DEBUG_VARS(sentence);
    printf("%s: %s\n", objName1, sentence);
    FREE(sentence);

    IF_ERR_RETURN(getSentenceDefinitionFromPath(&akinator->tree, diffPath2_len - 1, diffPath2, &sentence, false));
    LOG_DEBUG_VARS(sentence);
    printf("%s: %s\n", objName2, sentence);
    FREE(sentence);

    return TERMINAL_INTERFACE_STATUS_OK;
}

static TerminalInterfaceErrors terminalCmdShowCommonOf2Objects(Akinator* akinator) {
    IF_ERR_RETURN(commonOrDifferenceHelperFunc(&akinator->tree, true));
    //printf("not implemented yet\n");
    return TERMINAL_INTERFACE_STATUS_OK;
}

static TerminalInterfaceErrors terminalCmdShowDifferenceOf2Objects(Akinator* akinator) {
    IF_ERR_RETURN(commonOrDifferenceHelperFunc(&akinator->tree, false));
    //printf("not implemented yet\n");
    return TERMINAL_INTERFACE_STATUS_OK;
}

static TerminalInterfaceErrors terminalCmdShowImageOfAkinatorsDecisionTree(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    AKINATOR_ERR_CHECK(showImageOfAkinatorsDecisionTree(akinator));

    return TERMINAL_INTERFACE_STATUS_OK;
}

static TerminalInterfaceErrors terminalCmdTryToGuessObject(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    AKINATOR_ERR_CHECK(tryToGuessObject(akinator));

    return TERMINAL_INTERFACE_STATUS_OK;
}

static TerminalInterfaceErrors terminalCmdShowDifferenceImage(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    char* objName1 = printInputMessageAndReadString("name of object_1: ");
    char* objName2 = printInputMessageAndReadString("name of object_2: ");
    AKINATOR_ERR_CHECK(dumpCommonPathOf2Objects(akinator, objName1, objName2));
    FREE(objName1);
    FREE(objName2);

    return TERMINAL_INTERFACE_STATUS_OK;
}

typedef TerminalInterfaceErrors (*terminalCmdFuncPtr)(Akinator* akinator);

struct TerminalCmd {
    const char*        cmdName;
    terminalCmdFuncPtr funcPtr;
};

#define CMD(commandName, commandFuncPtr) \
    {commandName, commandFuncPtr},

static TerminalCmd arrayOfCommands[] = {
    #include "../include/plainTextTerminalCommandsForDecisionTree.in"
};

#undef CMD

static const size_t NUM_OF_COMMANDS = sizeof(arrayOfCommands) / sizeof(*arrayOfCommands);

static TerminalInterfaceErrors executeCommand(Akinator* akinator, const char* commandName, bool* isQuit) {
    IF_ARG_NULL_RETURN(akinator);
    IF_ARG_NULL_RETURN(commandName);
    IF_ARG_NULL_RETURN(isQuit);

    if (strcmp(commandName, QUIT_COMMAND) == 0) {
        *isQuit = true;
        return TERMINAL_INTERFACE_STATUS_OK;
    }

    for (size_t commandInd = 0; commandInd < NUM_OF_COMMANDS; ++commandInd) {
        TerminalCmd command = arrayOfCommands[commandInd];
        if (strcmp(command.cmdName, commandName) == 0) {
            TerminalInterfaceErrors error = (*command.funcPtr)(akinator);
            IF_ERR_RETURN(error);
        }
    }

    return TERMINAL_INTERFACE_STATUS_OK;
}

TerminalInterfaceErrors mainProgramWhileTrue(Akinator* akinator) {
    IF_ARG_NULL_RETURN(akinator);

    bool isQuit = false;
    IF_ERR_RETURN(terminalCmdShowHelpMessage(akinator));
    while (!isQuit) {
        // FIXME: uuuuhhh bruuuuh
        char* buffer = printInputMessageAndReadString("Print your command: ");
        executeCommand(akinator, buffer, &isQuit);
        FREE(buffer);
    }

    return TERMINAL_INTERFACE_STATUS_OK;
}
