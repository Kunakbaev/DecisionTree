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

    return TERMINAL_INTERFACE_STATUS_OK;
}

// if no, than to left son, otherwise (yes) to right son
TerminalInterfaceErrors askQuestionFromNodesData(const Node* node, bool *isToLeftSon) {
    IF_ARG_NULL_RETURN(node);

    // TODO: check that it's not leaf
    (*nodeDataPrinter)(node->data);
    IF_ERR_RETURN(isAnswerOnQuestionYes(isToLeftSon));

    return TERMINAL_INTERFACE_STATUS_OK;
}

TerminalInterfaceErrors isObjectGuessedCorrectly(const Node* node, bool* isCorrectGuess) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(isCorrectGuess);

    printf("Decision tree's guess is: \"%s\", is it your object?\n", node->data);
    IF_ERR_RETURN(isAnswerOnQuestionYes(isCorrectGuess));

    return TERMINAL_INTERFACE_STATUS_OK;
}

TerminalInterfaceErrors getGoodSentencesFromNodesData(const TypicalBinaryTree* tree, size_t pathLen, size_t* path,
                                                      char*** result) {
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path);
    IF_ARG_NULL_RETURN(result);

    const char** nodesData = (const char**)calloc(pathLen, sizeof(const char*));
                   *result = (char**)      calloc(pathLen, sizeof(const char*));
    IF_NOT_COND_RETURN(nodesData   != NULL, TERMINAL_INTERFACE_MEMORY_ALLOCATION_ERROR);

    for (size_t vertInd = 0; vertInd < pathLen; ++vertInd) {
        Node node = {};
        BIN_TREE_ERR_CHECK(getBinTreeNodeByVertIndex(tree, path[vertInd], &node));
        nodesData  [vertInd] = (*nodeDataPrinter)(node.data);
        LOG_DEBUG_VARS(vertInd, nodesData[vertInd]);
    }

    size_t prevNodeInd = path[0];
    for (size_t vertInd = 1; vertInd < pathLen + 1; ++vertInd) {
        const size_t TMP_BUFF_SIZE = 300;
        char tmpBuff[TMP_BUFF_SIZE] = {};
        if (vertInd < pathLen) {
            size_t curNodeInd = path[vertInd];
            bool isLeftSon = false;
            BIN_TREE_ERR_CHECK(isNodeAleftSonOfParent(tree, prevNodeInd, curNodeInd, &isLeftSon));
            const char* ans = isLeftSon ? "no" : "yes";
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
