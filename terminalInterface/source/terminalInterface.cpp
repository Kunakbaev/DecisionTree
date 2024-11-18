#include "../include/terminalInterface.hpp"
#include "../../akinatorLib/include/akinatorLib.hpp"

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

#define AKINATOR_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getAkinatorErrorMessage, AKINATOR_STATUS_OK, TERMINAL_INTERFACE_AKINATOR_ERROR);

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

// static TerminalInterfaceErrors commonOrDifferenceHelperFunc(Akinator* akinator, bool isShowSame) {
//     IF_ARG_NULL_RETURN(akinator);
//
//     // TODO: remove copypaste
//     char* objName1 = printInputMessageAndReadString("name of object_1: ");
//     char* objName2 = printInputMessageAndReadString("name of object_2: ");
//     assert(objName1 != NULL);
//     assert(objName2 != NULL);
//
//     IF_NOT_COND_RETURN(strcmp(objName1, objName2) != 0,
//                        TERMINAL_INTERFACE_SAME_OBJ_NAMES);
//
//     size_t*   path1 = NULL;
//     size_t pathLen1 = 0;
//     IF_ERR_RETURN(getPathToObjByName(akinator, objName1, &pathLen1, &path1));
//
//     size_t*   path2 = NULL;
//     size_t pathLen2 = 0;
//     IF_ERR_RETURN(getPathToObjByName(akinator, objName2, &pathLen2, &path2));
//
//     size_t* cntArr = NULL;
//
//     IF_ERR_RETURN(getCommonPathCntArray(akinator, objName1, objName2, &cntArr));
//
//     size_t* commonPath = (size_t*)calloc(pathLen1, sizeof(size_t));
//     IF_NOT_COND_RETURN(commonPath != NULL,
//                        DECISION_akinator_MEMORY_ALLOCATION_ERROR);
//
//     size_t commonPathLen = 0;
//     for (size_t i = 0; i + 1 < pathLen2; ++i) {
//         if (cntArr[path2[i + 1]] != 2) {
//             commonPath[commonPathLen] = path2[i];
//             ++commonPathLen;
//             break;
//         }
//         if (!isShowSame) continue;
//
//         LOG_DEBUG_VARS(i, path2[i], path1[i]);
//         commonPath[commonPathLen] = path2[i];
//         ++commonPathLen;
//     }
//     LOG_DEBUG_VARS(objName1, objName2, commonPathLen);
//
//     IF_ERR_RETURN(printPathArrayFromRootToNode(akinator, commonPathLen, commonPath, !isShowSame));
//
//     FREE(objName1);
//     FREE(objName2);
//     FREE(path1);
//     FREE(path2);
//     FREE(cntArr);
//     FREE(commonPath);
//
//     return TERMINAL_INTERFACE_STATUS_OK;
// }

static TerminalInterfaceErrors terminalCmdShowCommonOf2Objects(Akinator* akinator) {
    //IF_ERR_RETURN(commonOrDifferenceHelperFunc(akinator, true));
    printf("not implemented yet\n");
    return TERMINAL_INTERFACE_STATUS_OK;
}

static TerminalInterfaceErrors terminalCmdShowDifferenceOf2Objects(Akinator* akinator) {
    //IF_ERR_RETURN(commonOrDifferenceHelperFunc(akinator, false));
    printf("not implemented yet\n");
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
