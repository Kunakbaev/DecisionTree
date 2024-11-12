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

static DumperErrors getNodesColorNormal(const Node* node, size_t highlightedNodeInd, char** result) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(result);

    memset(tmpBuffer, 0, TMP_BUFFER_SIZE);
    *result = (char*)calloc(COLOR_LEN, sizeof(char));
    IF_NOT_COND_RETURN(*result != NULL, DUMPER_ERROR_MEMORY_ALLOCATION_ERROR);
    const char* greenColor = "green";
    const char* whiteColor = "white";

    // TODO: maybe pass as parameter how many last nodes should be highlighted
    if (highlightedNodeInd - 1 <= node->memBuffIndex)
        memcpy(*result, greenColor, strlen(greenColor));
    else
        memcpy(*result, whiteColor, strlen(whiteColor));

    return DUMPER_STATUS_OK;
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

DumperErrors dumperDumpSingleTreeNode(Dumper* dumper, const Node* node, size_t highlightedNodeInd,
                                      const char* formatForNodeData) {
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

    char* color = NULL;
    getNodesColorNormal(node, highlightedNodeInd, &color);
    IF_ERR_RETURN(addNodeDumpStructToBuffer(dumper, node, color));
    FREE(color);
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








static DumperErrors getNodesColorForCommonPathFunc(const Node* node, size_t cntOccur, char** result) {
    IF_ARG_NULL_RETURN(node);
    IF_ARG_NULL_RETURN(result);

    memset(tmpBuffer, 0, TMP_BUFFER_SIZE);
    *result = (char*)calloc(COLOR_LEN, sizeof(char));
    IF_NOT_COND_RETURN(*result != NULL, DUMPER_ERROR_MEMORY_ALLOCATION_ERROR);
    const char* greenColor = "green";
    const char* whiteColor = "white";
    const char* redColor   = "red";

    // TODO: maybe pass as parameter how many last nodes should be highlighted
    //LOG_DEBUG_VARS(cntOccur);
    if (cntOccur == 0)
        memcpy(*result, whiteColor, strlen(whiteColor));
    if (cntOccur == 1)
        memcpy(*result, redColor, strlen(redColor));
    if (cntOccur == 2)
        memcpy(*result, greenColor, strlen(greenColor));

    return DUMPER_STATUS_OK;
}

static DumperErrors
superDuperPuperSlow_drawDecisionTreeRecursivelyCommonPathHighlight(
Dumper* dumper, const DecisionTree* tree,
size_t nodeInd, size_t parentInd,
size_t pathLen1, size_t* path1,
size_t pathLen2, size_t* path2) {
    IF_ARG_NULL_RETURN(dumper);
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path1);
    IF_ARG_NULL_RETURN(path2);

    if (nodeInd == 0) // subtree is empty
        return DUMPER_STATUS_OK;

    assert(nodeInd < tree->memBuffSize);
    Node node = tree->memBuff[nodeInd];

    // FIXME: bruuuuuuuuuuh, it works longer than your grandma comes too the second floor
    size_t cntOccur = 0;
    for (size_t i = 0; i < pathLen1; ++i)
        cntOccur += path1[i] == nodeInd;
    for (size_t i = 0; i < pathLen2; ++i)
        cntOccur += path2[i] == nodeInd;

    char* color = NULL;
    getNodesColorForCommonPathFunc(&node, cntOccur, &color);
    //LOG_DEBUG_VARS(nodeInd, cntOccur, color);
    IF_ERR_RETURN(addNodeDumpStructToBuffer(dumper, &node, color));
    FREE(color);

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

    IF_ERR_RETURN(superDuperPuperSlow_drawDecisionTreeRecursivelyCommonPathHighlight(dumper, tree, node.left , nodeInd, pathLen1, path1, pathLen2, path2));
    IF_ERR_RETURN(superDuperPuperSlow_drawDecisionTreeRecursivelyCommonPathHighlight(dumper, tree, node.right, nodeInd, pathLen1, path1, pathLen2, path2));

    return DUMPER_STATUS_OK;
}

static DumperErrors drawDecisionTreeRecursively(Dumper* dumper, const DecisionTree* tree,
                                                size_t nodeInd, size_t parentInd, size_t highlightedNodeInd) {
    IF_ARG_NULL_RETURN(dumper);
    IF_ARG_NULL_RETURN(tree);

    if (nodeInd == 0) // subtree is empty
        return DUMPER_STATUS_OK;

    assert(nodeInd < tree->memBuffSize);
    Node node = tree->memBuff[nodeInd];

    char* color = NULL;
    getNodesColorNormal(&node, highlightedNodeInd, &color);
    IF_ERR_RETURN(addNodeDumpStructToBuffer(dumper, &node, color));
    FREE(color);

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

    IF_ERR_RETURN(drawDecisionTreeRecursively(dumper, tree, node.left , nodeInd, highlightedNodeInd));
    IF_ERR_RETURN(drawDecisionTreeRecursively(dumper, tree, node.right, nodeInd, highlightedNodeInd));

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
                                    size_t highlightedNodeInd) {
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

    IF_ERR_RETURN(drawDecisionTreeRecursively(dumper, tree, tree->root, 0, highlightedNodeInd));

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

// FIXME:
// FIXME:
// FIXME: too much copypaste

DumperErrors dumperDumpDecisionTreeDrawCommonPathes(Dumper* dumper, const DecisionTree* tree,
                                    size_t pathLen1, size_t* path1,
                                    size_t pathLen2, size_t* path2) {
    IF_ARG_NULL_RETURN(dumper);
    IF_ARG_NULL_RETURN(tree);
    IF_ARG_NULL_RETURN(path1);
    IF_ARG_NULL_RETURN(path2);

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

    // FIXME: bruuh, super ugly
    IF_ERR_RETURN(superDuperPuperSlow_drawDecisionTreeRecursivelyCommonPathHighlight(dumper, tree, tree->root, 0,
        pathLen1, path1, pathLen2, path2));

    strncat(buffer, "}\n", BUFFER_SIZE);
    //LOG_DEBUG_VARS(buffer);
    fprintf(outputFile, buffer);
    fclose(outputFile);

    memset(fileNameBuffer, 0, FILE_NAME_BUFFER_SIZE);
    snprintf(fileNameBuffer, FILE_NAME_BUFFER_SIZE,
            "%zu_list.%s", dumper->numberOfLogsBefore, dumper->outputFileFormat);
    LOG_DEBUG_VARS(fileNameBuffer);

    // TODO: put assert for ;
    memset(fileFullNameBuffer, 0, FILE_NAME_BUFFER_SIZE);
    snprintf(fileFullNameBuffer, FULL_FILE_NAME_BUFFER_SIZE,
             "dot -Tpng logs/dots/%zu_list.dot -o %s/images/%s",
            dumper->numberOfLogsBefore, dumper->dirForLogsPath, fileNameBuffer);
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
