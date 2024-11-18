#ifndef INCLUDE_ULTIMATE_DUMPER_HPP
#define INCLUDE_ULTIMATE_DUMPER_HPP

// ASK: circular import?

enum DumperErrors {
    DUMPER_STATUS_OK                        = 0,
    DUMPER_ERROR_INVALID_ARGUMENT           = 1,
    DUMPER_ERROR_MEMORY_ALLOCATION_ERROR    = 2,
    DUMPER_ERROR_COULD_OPEN_FILE            = 3,
    DUMPER_ERROR_LIST_SUBLIB_ERROR          = 4,
};

#include "dumperStruct.hpp"
#include "../typicalBinaryTree/include/typicalBinaryTree.hpp"

const size_t MAX_COLORING_RULE_LEN                           = 20;
const size_t MAX_NUM_OF_NODES_IN_ONE_COLOR_WITH_NODES_STRUCT = 100;

struct NodesWithColor {
    const char* color;
    size_t      numOfNodes;
    size_t*     nodes;
};

DumperErrors dumperConstructor(Dumper* dumper,
                               size_t maxNumOfNodesToDraw,
                               const char* dirForLogsPath,
                               const char* outputFileFormat);
void dumperAddDebugInfoToAllLogsFile(Dumper* dumper, const char* debugInfo);
void dumperAddImgToAllLogsFile(Dumper* dumper, const char* imagePath);
DumperErrors dumperDumpSingleTreeNode(Dumper* dumper, const Node* node, const char* nodeColor);
DumperErrors dumperDumpBinaryTree(Dumper* dumper, const TypicalBinaryTree* tree,
                                      const NodesWithColor* coloringRule,
                                      size_t coloringRuleLen);
char* getLastImageFileName(const Dumper* dumper);
DumperErrors dumperDestructor(Dumper* dumper);

const char* getDumperErrorMessage(DumperErrors error);


#define DEBUG_VARS_TO_DUMPER_ALL_LOGS_FILE(dumper, ...) \
    do {\
        dumperAddDebugInfoToAllLogsFile(dumper, "<pre style=\"color: white\">\n");\
        stateLogFile((dumper)->allLogsFile);\
        LOG_DEBUG_VARS(__VA_ARGS__);\
        flushLogFile();\
        stateLogFile(NULL);\
        /*dumperAddDebugInfoToAllLogsFile(dumper, "</pre>\n");\*/\
    } while (0)

#define DEBUG_MESSAGE_TO_DUMPER_ALL_LOGS_FILE(dumper, message) \
    do {\
        dumperAddDebugInfoToAllLogsFile(dumper, "<pre style=\"color: white\">\n");\
        stateLogFile((dumper)->allLogsFile);\
        LOG_DEBUG(message);\
        flushLogFile();\
        stateLogFile(NULL);\
        /*dumperAddDebugInfoToAllLogsFile(dumper, "</pre>\n");\*/\
    } while (0)

#endif


