#include "../include/typicalBinaryTree.hpp"

const char* getTypicalBinaryTreeErrorMessage(TypicalBinaryTreeErrors error) {
    #define ERROR_UNPACK(errName, errCode, errMessage)          \
        case errName:                                           \
            return "Typical binary tree error: " errMessage ".\n";    \

    switch (error) {
        case TYPICAL_BIN_TREE_STATUS_OK:
            return "Typical binary tree: no errors occured.\n";

        // ASK: code generation
        #include "../include/typicalBinaryTreePlainText.in"

        default:
            return "Typical binary tree error: unknown error.\n";
    }

    #undef UNPACK
}
