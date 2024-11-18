#ifndef INCLUDE_TYPICAL_BIN_TREE_ERRORS_HPP
#define INCLUDE_TYPICAL_BIN_TREE_ERRORS_HPP

#include "../../include/errorsHandlerDefines.hpp"

#define ERROR_UNPACK(errName, errCode, errMessage) \
    errName = errCode,

enum TypicalBinaryTreeErrors {
    TYPICAL_BIN_TREE_STATUS_OK             = 0,
    #include "typicalBinaryTreePlainText.in"
    //DECISION_TREE_MEMORY_ALLOCATION_ERROR = 1, // "couldn't allocate memory.\n")
    //DECISION_TREE_INVALID_ARGUMENT =        2, // "invalid argument, possibly set to NULL\n")
};

#undef ERROR_UNPACK

const char* getTypicalBinaryTreeErrorMessage(TypicalBinaryTreeErrors error);

#endif
