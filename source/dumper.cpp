#include <stdlib.h>

#include "../include/dumper.hpp"

#define IF_ARG_NULL_RETURN(arg) \
    COMMON_IF_ARG_NULL_RETURN(arg, DUMPER_ERROR_INVALID_ARGUMENT, getDumperErrorMessage)

#define IF_ERR_RETURN(error) \
    COMMON_IF_ERR_RETURN(error, getDumperErrorMessage, DUMPER_STATUS_OK)

#define IF_NOT_COND_RETURN(condition, error) \
    COMMON_IF_NOT_COND_RETURN(condition, error, getDumperErrorMessage)

#define LIST_ERR_CHECK(error) \
    COMMON_IF_SUBMODULE_ERR_RETURN(error, getLinkedListErrorMessage, LINKED_LIST_STATUS_OK, DUMPER_ERROR_LIST_SUBLIB_ERROR);

const size_t FILE_NAME_BUFFER_SIZE      = 100;
const size_t FULL_FILE_NAME_BUFFER_SIZE = 200;
const size_t BUFFER_SIZE                = 1 << 15;
const size_t TMP_BUFFER_SIZE            = 1 << 13;
const size_t FORMAT_SPEC_LEN            = 5;
const size_t COLOR_LEN                  = 10;

static char* fileFullNameBuffer                = NULL;
static char* fileNameBuffer                    = NULL;
static char* tmpBuffer                         = NULL; // ASK: how to get rid of it? Like, make strcat and sprintf at the same time
static char* buffer                            = NULL;

const char* getDumperErrorMessage(DumperErrors error) {
    switch (error) {
        case DUMPER_STATUS_OK:
            return "No errors in linked list, everything is valid.\n";
        case DUMPER_ERROR_INVALID_ARGUMENT:
            return "Dumper error: invalid argument (possibly set to NULL).\n";
        case DUMPER_ERROR_MEMORY_ALLOCATION_ERROR:
            return "Dumper error: memory allocation error.\n";
        case DUMPER_ERROR_COULD_OPEN_FILE:
            return "Dumper error: couldn't open file.\n";
        case DUMPER_ERROR_LIST_SUBLIB_ERROR:
            return "Dumper error: error occured in linked list sublib, possibly list is invalid.\n";

        default:
            return "Dumper error: unknown error.\n";
    }
}

DumperErrors dumperConstructor(Dumper* dumper,
                               size_t maxNumOfNodesToDraw,
                               const char* dirForLogsPath,
                               const char* outputFileFormat) {
    IF_ARG_NULL_RETURN(dumper);
    IF_ARG_NULL_RETURN(dirForLogsPath);
    IF_ARG_NULL_RETURN(outputFileFormat);

    *dumper = {};
    dumper->maxNumOfNodesToDraw = maxNumOfNodesToDraw;
    dumper->dirForLogsPath      = dirForLogsPath;
    dumper->outputFileFormat    = outputFileFormat;
    dumper->numberOfLogsBefore  = 0;

    char* allLogsFilePath = (char*)calloc(FULL_FILE_NAME_BUFFER_SIZE, sizeof(char));
    snprintf(allLogsFilePath, FULL_FILE_NAME_BUFFER_SIZE,
             "%s/allLogs.html", dumper->dirForLogsPath);
    IF_NOT_COND_RETURN(allLogsFilePath != NULL,
                       DUMPER_ERROR_MEMORY_ALLOCATION_ERROR);
    dumper->allLogsFile = fopen(allLogsFilePath, "w");
    IF_NOT_COND_RETURN(dumper->allLogsFile != NULL,
                       DUMPER_ERROR_COULD_OPEN_FILE);
    FREE(allLogsFilePath);
    setvbuf(dumper->allLogsFile, NULL, 0, _IONBF);

    fileFullNameBuffer = (char*)calloc(FULL_FILE_NAME_BUFFER_SIZE, sizeof(char));
    IF_NOT_COND_RETURN(fileFullNameBuffer != NULL,
                       DUMPER_ERROR_MEMORY_ALLOCATION_ERROR);
    fileNameBuffer     = (char*)calloc(FILE_NAME_BUFFER_SIZE,      sizeof(char));
    IF_NOT_COND_RETURN(fileNameBuffer != NULL,
                       DUMPER_ERROR_MEMORY_ALLOCATION_ERROR);
    tmpBuffer          = (char*)calloc(TMP_BUFFER_SIZE,            sizeof(char));
    IF_NOT_COND_RETURN(tmpBuffer != NULL,
                       DUMPER_ERROR_MEMORY_ALLOCATION_ERROR);
    buffer             = (char*)calloc(BUFFER_SIZE,                sizeof(char));
    IF_NOT_COND_RETURN(buffer != NULL,
                       DUMPER_ERROR_MEMORY_ALLOCATION_ERROR);

    // WARNING: whole root directory can be deleted, bruh, or some nasty command executed
    // memset(fileFullNameBuffer, 0, FULL_FILE_NAME_BUFFER_SIZE);
    // snprintf(fileFullNameBuffer, FULL_FILE_NAME_BUFFER_SIZE, "rm -rf %s", dirForLogsPath);
    // system(fileFullNameBuffer);
    memset(fileFullNameBuffer, 0, FULL_FILE_NAME_BUFFER_SIZE);
    snprintf(fileFullNameBuffer, FULL_FILE_NAME_BUFFER_SIZE, "mkdir -p %s", dirForLogsPath);
    system(fileFullNameBuffer);
    system("mkdir -p logs/images");
    system("mkdir -p logs/html");
    system("mkdir -p logs/dots");

    dumperAddDebugInfoToAllLogsFile(dumper, "<body style=\"background: black; overflow: scroll; margin: 15px\">\n");

    return DUMPER_STATUS_OK;
}

void dumperAddDebugInfoToAllLogsFile(Dumper* dumper, const char* debugInfo) {
    assert(dumper    != NULL);
    assert(debugInfo != NULL);

    fprintf(dumper->allLogsFile, debugInfo);
}

void dumperAddImgToAllLogsFile(Dumper* dumper, const char* imagePath) {
    assert(dumper    != NULL);
    assert(imagePath != NULL);

    LOG_DEBUG_VARS(imagePath);
    fprintf(dumper->allLogsFile, "<img src=\"%s\"></img>\n", imagePath);
    LOG_DEBUG("---------------");
}

static DumperErrors addNodeDumpStructToBuffer(Dumper* dumper,
                                              const Node* node,
                                              const char* color) {
    IF_ARG_NULL_RETURN(dumper);
    IF_ARG_NULL_RETURN(buffer);

    // const size_t DATA_STRING_LEN = 50;
    // char* dataString = (char*)calloc(DATA_STRING_LEN, sizeof(char));
    // IF_NOT_COND_RETURN(dataString != NULL, DUMPER_ERROR_MEMORY_ALLOCATION_ERROR);
    // const char* format = varToFormat(node->data);
    // LOG_DEBUG_VARS(format);
    // sprintf(dataString, format, node->data);
    // LOG_DEBUG_VARS(dataString, node->data);

    memset(tmpBuffer, 0, TMP_BUFFER_SIZE);
    if (node != NULL) {
        snprintf(tmpBuffer, TMP_BUFFER_SIZE,
        "iamnode_id_%zu [shape=none, margin=0, fontcolor=white, color=%s, label=< \n"
            "<TABLE cellspacing=\"0\"> \n"
                "<TR><TD colspan=\"2\">data:  %s</TD></TR>\n"
                "<TR><TD colspan=\"2\">memIndex:  %zu</TD></TR>\n"
                "<TR><TD>no:  %zu</TD>\n"
                "<TD>yes: %zu</TD></TR>\n"
                "</TABLE> \n"
                " >];\n", node->memBuffIndex, color, node->data, node->memBuffIndex, node->left, node->right);
        // LOG_DEBUG_VARS(tmpBuffer, node->data);
    } else {
        snprintf(tmpBuffer, TMP_BUFFER_SIZE,
            "iamnode_id_%zu [shape=rect, margin=0, fontcolor=white, color=%s, label=<null>];\n",
            node->memBuffIndex, color);
    }

    strncat(buffer, tmpBuffer, BUFFER_SIZE);

    return DUMPER_STATUS_OK;
}

DumperErrors dumperDumpSingleTreeNode(Dumper* dumper, const Node* node, const char* nodeColor) {
    IF_ARG_NULL_RETURN(dumper);

    LOG_DEBUG("single node dumping ---------------------");
    ++dumper->numberOfLogsBefore;
    memset(fileNameBuffer, 0, FILE_NAME_BUFFER_SIZE);
    snprintf(fileNameBuffer, FILE_NAME_BUFFER_SIZE,
             "dots/%zu_node.dot", dumper->numberOfLogsBefore);

    memset(fileFullNameBuffer, 0, FULL_FILE_NAME_BUFFER_SIZE);
    snprintf(fileFullNameBuffer, FULL_FILE_NAME_BUFFER_SIZE,
             "%s/%s", dumper->dirForLogsPath, fileNameBuffer);

    FILE* outputFile = fopen(fileFullNameBuffer, "w");
    IF_NOT_COND_RETURN(outputFile != NULL,
                       DUMPER_ERROR_COULD_OPEN_FILE);

    memset(buffer, 0, BUFFER_SIZE);
    strncat(buffer, "digraph html {\n\
        overlap=false\n\
        splines=ortho\n\
        bgcolor=\"black\"\n\
        rankdir=TB\n\
        pad=0.2\n\
    ", BUFFER_SIZE);

    IF_ERR_RETURN(addNodeDumpStructToBuffer(dumper, node, nodeColor));
    strncat(buffer, "}\n", BUFFER_SIZE);
    fprintf(outputFile, buffer);
    fclose(outputFile);

    memset(fileNameBuffer, 0, FILE_NAME_BUFFER_SIZE);
    snprintf(fileNameBuffer, FILE_NAME_BUFFER_SIZE, "%zu_node.%s",
            dumper->numberOfLogsBefore, dumper->outputFileFormat);

    memset(fileFullNameBuffer, 0, FILE_NAME_BUFFER_SIZE);
    snprintf(fileFullNameBuffer, FULL_FILE_NAME_BUFFER_SIZE,
            "dot -Tpng logs/dots/%zu_node.dot -o  %s/images/%s",
            dumper->numberOfLogsBefore, dumper->dirForLogsPath, fileNameBuffer);
    system(fileFullNameBuffer);
    return DUMPER_STATUS_OK;
}







static const char*  DEFAULT_COLOR = "white";

static const char* getNodeColor(const Node* node, const NodesWithColor* coloringRule, size_t coloringRuleLen) {
    assert(node            != NULL);
    assert(coloringRule    != NULL);
    assert(coloringRuleLen  < MAX_COLORING_RULE_LEN);

    for (size_t arrInd = 0; arrInd < coloringRuleLen; ++arrInd) {
        const char* color = coloringRule[arrInd].color;
        size_t*     nodes = coloringRule[arrInd].nodes;
        size_t   nodesLen = coloringRule[arrInd].numOfNodes;

        for (size_t nodeArrInd = 0; nodeArrInd < nodesLen; ++nodeArrInd) {
            size_t nodeInd = nodes[nodeArrInd];
            if (nodeInd == node->memBuffIndex) {
                return color;
            }
        }
    }

    return DEFAULT_COLOR; // maybe log error
}

static DumperErrors drawDecisionTreeRecursively(Dumper* dumper, const DecisionTree* tree,
                                                size_t nodeInd, size_t parentInd,
                                                const NodesWithColor* coloringRule,
                                                size_t coloringRuleLen) {
    IF_ARG_NULL_RETURN(dumper);
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(coloringRule);

    if (nodeInd == 0) // subtree is empty
        return DUMPER_STATUS_OK;

    assert(nodeInd < tree->memBuffSize);
    Node node = tree->memBuff[nodeInd];

    const char* color = getNodeColor(&node, coloringRule, coloringRuleLen);
    IF_ERR_RETURN(addNodeDumpStructToBuffer(dumper, &node, color));

    if (parentInd != 0) {
        memset(tmpBuffer, 0, TMP_BUFFER_SIZE);

        assert(parentInd < tree->memBuffSize);
        Node parent = tree->memBuff[parentInd];
        if (nodeInd == parent.left) {
            snprintf(tmpBuffer, TMP_BUFFER_SIZE, "iamnode_id_%zu -> iamnode_id_%zu [color=orange, fontcolor=white, weight=1]\n",
                parentInd, nodeInd);
        } else {
            snprintf(tmpBuffer, TMP_BUFFER_SIZE, "iamnode_id_%zu -> iamnode_id_%zu [color=lightblue, fontcolor=white, weight=1]\n",
                parentInd, nodeInd);
        }

        size_t tmpBuffLen = strlen(tmpBuffer);
        snprintf(tmpBuffer + tmpBuffLen, TMP_BUFFER_SIZE - tmpBuffLen, "iamnode_id_%zu -> iamnode_id_%zu [color=purple, fontcolor=white]\n",
                nodeInd, tree->memBuff[nodeInd].parent);
        strncat(buffer, tmpBuffer, BUFFER_SIZE);
    }

    IF_ERR_RETURN(drawDecisionTreeRecursively(dumper, tree, node.left , nodeInd, coloringRule, coloringRuleLen));
    IF_ERR_RETURN(drawDecisionTreeRecursively(dumper, tree, node.right, nodeInd, coloringRule, coloringRuleLen));

    return DUMPER_STATUS_OK;
}

char* getLastImageFileName(const Dumper* dumper) {
    assert(dumper != NULL);

    memset(fileNameBuffer, 0, FILE_NAME_BUFFER_SIZE);
    snprintf(fileNameBuffer, FILE_NAME_BUFFER_SIZE,
            "%zu_list.%s", dumper->numberOfLogsBefore, dumper->outputFileFormat);
    LOG_DEBUG_VARS(fileNameBuffer);

    snprintf(tmpBuffer, TMP_BUFFER_SIZE,
             "%s/images/%s", dumper->dirForLogsPath, fileNameBuffer);
    return tmpBuffer;
}

DumperErrors dumperDumpDecisionTree(Dumper* dumper, const DecisionTree* tree,
                                    const NodesWithColor* coloringRule,
                                    size_t coloringRuleLen) {
    IF_ARG_NULL_RETURN(dumper);
    IF_ARG_NULL_RETURN(tree);

    LOG_DEBUG("decision tree dumping ---------------------");
    ++dumper->numberOfLogsBefore;
    memset(fileNameBuffer, 0, FILE_NAME_BUFFER_SIZE);
    // TODO: rewrite with snprintf
    snprintf(fileNameBuffer, FILE_NAME_BUFFER_SIZE, "dots/%zu_list.dot",
             dumper->numberOfLogsBefore);

    memset(fileFullNameBuffer, 0, FULL_FILE_NAME_BUFFER_SIZE);
    snprintf(fileFullNameBuffer, FULL_FILE_NAME_BUFFER_SIZE,
             "%s/%s", dumper->dirForLogsPath, fileNameBuffer);

    LOG_DEBUG_VARS(fileFullNameBuffer, fileNameBuffer);
    FILE* outputFile = fopen(fileFullNameBuffer, "w");
    IF_NOT_COND_RETURN(outputFile != NULL,
                       DUMPER_ERROR_COULD_OPEN_FILE);

    memset(buffer, 0, BUFFER_SIZE);
    // FIXME: add errors check
    strncat(buffer, "digraph html {\n\
        overlap=false\n\
        bgcolor=\"black\"\n\
        rankdir=TB\n\
        pad=0.2\n\
    ", BUFFER_SIZE);

    IF_ERR_RETURN(drawDecisionTreeRecursively(dumper, tree, tree->root, 0, coloringRule, coloringRuleLen));

    strncat(buffer, "}\n", BUFFER_SIZE);
    //LOG_DEBUG_VARS(buffer);
    fprintf(outputFile, buffer);
    fclose(outputFile);

    tmpBuffer = getLastImageFileName(dumper);

    // TODO: put assert for ;
    memset(fileFullNameBuffer, 0, FILE_NAME_BUFFER_SIZE);
    snprintf(fileFullNameBuffer, FULL_FILE_NAME_BUFFER_SIZE,
             "dot -Tpng logs/dots/%zu_list.dot -o %s",
             dumper->numberOfLogsBefore, tmpBuffer);
    LOG_DEBUG_VARS(fileFullNameBuffer);
    // WARNING: some nasty command can be substituted
    system(fileFullNameBuffer);

    memset(fileFullNameBuffer, 0, FILE_NAME_BUFFER_SIZE);
    snprintf(fileFullNameBuffer, FULL_FILE_NAME_BUFFER_SIZE,
             "images/%s", fileNameBuffer);
    LOG_DEBUG_VARS(fileFullNameBuffer);
    dumperAddImgToAllLogsFile(dumper, fileFullNameBuffer);

    return DUMPER_STATUS_OK;
}

DumperErrors dumperDestructor(Dumper* dumper) {
    IF_ARG_NULL_RETURN(dumper);

    dumperAddDebugInfoToAllLogsFile(dumper, "</body>\n");

    FREE(fileNameBuffer);
    FREE(tmpBuffer);
    FREE(buffer);
    FREE(fileFullNameBuffer);

    // ASK: why if free than memory leak?
    fclose(dumper->allLogsFile);
    *dumper = {};

    return DUMPER_STATUS_OK;
}
