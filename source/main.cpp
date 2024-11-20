#include <iostream>
//#include <espeak/speak_lib.h>

#include "logLib.hpp"
//#include "../typicalBinaryTree/include/typicalBinaryTree.hpp"
#include "../akinatorLib/include/akinatorLib.hpp"
#include "../terminalInterface/include/terminalInterface.hpp"
#include "../include/dumper.hpp"

#define IF_MAIN_BIN_TREE_ERR_RETURN(error)                                   \
    do {                                                            \
        TypicalBinaryTreeErrors tmp = error;                             \
        if (tmp != TYPICAL_BIN_TREE_STATUS_OK) {                       \
            LOG_ERROR(getTypicalBinaryTreeErrorMessage(tmp));            \
            /* if error occurs in destructor, then we are screwed*/ \
            destructTypicalBinaryTree(&tree);                            \
            assert(tmp != TYPICAL_BIN_TREE_STATUS_OK);                 \
            return tmp;                                             \
        }                                                           \
    } while(0)

#define IF_MAIN_AKINATOR_ERR_RETURN(error)                                   \
    do {                                                            \
        AkinatorErrors tmp = error;                             \
        if (tmp != AKINATOR_STATUS_OK) {                       \
            LOG_ERROR(getAkinatorErrorMessage(tmp));            \
            /* if error occurs in destructor, then we are screwed*/ \
            destructAkinator(&akinator);                            \
            assert(tmp != AKINATOR_STATUS_OK);                 \
            return tmp;                                             \
        }                                                           \
    } while(0)

// DecisionTreeErrors addValueAndDump(DecisionTree* tree, const void* value);
//
// const char* getStringFromVoidPtr(const void* voidPtr) {
//     //assert(voidPtr != NULL);
//     // char* buffer = (char*)calloc(1000, sizeof(char));
//     // assert(buffer != NULL);
//
//     const char* ptr = (const char*)(voidPtr);
//     if (ptr == NULL)
//         return "";
//     return (const char*)(voidPtr);
//
// //     return (const char*)voidPtr;
// //     LOG_DEBUG_VARS(voidPtr, ptr, ptr[0]);
// //     for (size_t i = 0; ptr[i] != '\0'; ++i) {
// //         LOG_DEBUG_VARS(i, ptr[i]);
// //         buffer[i] = ptr[i];
// //     }
// //
// //     return buffer;
// }

// const char* nodeDataPrinter(const void* num) {
//     // LOG_DEBUG("node data printer");
//     //int x = num == NULL ? 0 : *(const int*)num;
//     const char* buffer = getStringFromVoidPtr(num);
//     //LOG_DEBUG_VARS(buffer);
//     //LOG_ERROR("-----------------------");
//
//     return buffer;
// }
//
// void nodeDataReader(void** num, const char* line) {
//     // LOG_DEBUG("node data reader");
//     if (line == NULL) return;
//
//     *num = calloc(strlen(line) + 1, sizeof(char));
//     assert(*num != NULL);
//
//     LOG_DEBUG_VARS(line, num);
//     sscanf(line, "%[^\n]s", *num);
//     // LOG_ERROR("---------------------");
//     // LOG_DEBUG_VARS(num, line, (const char*)(*num));
// }
//
// int cmpNodesByValue(const void* num, const void* num2) {
//     //LOG_DEBUG_VARS("cmp of nodes", num, num2);
//     const char* one = getStringFromVoidPtr(num);
//     const char* two = getStringFromVoidPtr(num2);
//     return strcmp(one, two);
// }

int main() {
    //setLoggingLevel(DEBUG);

    Dumper dumper = {};
    dumperConstructor(&dumper, 10, "logs", "png");

    Akinator akinator = {};

    IF_MAIN_AKINATOR_ERR_RETURN(constructAkinator(&akinator, &dumper));

    LOG_DEBUG("does it work? :)");
    //IF_MAIN_AKINATOR_ERR_RETURN(readAkinatorsDecisionTreeFromFile(&akinator, "fruitsAndVegetablesDB.txt"));
    // IF_MAIN_AKINATOR_ERR_RETURN(showImageOfAkinatorsDecisionTree(&akinator));
    // IF_MAIN_AKINATOR_ERR_RETURN(showDefinitionOfObject(&akinator, "potato"));
    // IF_MAIN_AKINATOR_ERR_RETURN(dumpCommonPathOf2Objects(&akinator, "potato", "cabbage"));

    mainProgramWhileTrue(&akinator);

    IF_MAIN_AKINATOR_ERR_RETURN(destructAkinator(&akinator));

//     TypicalBinaryTree tree = {};
//     IF_MAIN_ERR_RETURN(constructTypicalBinaryTree(&tree, &dumper,
//                                                   nodeDataPrinter,
//                                                   nodeDataReader,
//                                                   cmpNodesByValue));
//
//     IF_MAIN_ERR_RETURN(readTypicalBinaryTreeFromFile(&tree, "fruitsAndVegetablesDB.txt"));
//     IF_MAIN_ERR_RETURN(openImageOfCurrentStateTypicalBinaryTree(&tree));
//     //IF_MAIN_ERR_RETURN(dumpCommonPathOf2ObjectsByValue(&tree, "potato", "cabbage"));
//
//     IF_MAIN_ERR_RETURN(destructTypicalBinaryTree(&tree));

    return 0;
}

// DecisionTreeErrors addValueAndDump(DecisionTree* tree, const void* value) {
//     if (tree == NULL) {
//         LOG_ERROR(getDecisionTreeErrorMessage(DECISION_TREE_INVALID_ARGUMENT));
//         return DECISION_TREE_INVALID_ARGUMENT;
//     }
//
//     IF_MAIN_ERR_RETURN(addNewNodeToDecisionTree(tree, (void*)value));
//     IF_MAIN_ERR_RETURN(dumpDecisionTree(tree));
//
//     return DECISION_TREE_STATUS_OK;
// }
