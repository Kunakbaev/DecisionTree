#ifndef COMMON_ERRORS_HANDLER_DEFINES_HPP
#define COMMON_ERRORS_HANDLER_DEFINES_HPP

#include <assert.h>

#include "logLib.hpp"
#include "utils.hpp"

// ASK: how to check if error == OK if all enums are different type
// log and return happens only if error realy occured (error != STATUS_OK)

// FIXME: bruh, auto is C++ feature, can pass error type to define, no big deal
#define COMMON_IF_ERR_RETURN(errorTmp, getErrorMessageFunc, OK_STATUS)                                        \
    do {                                                            \
        auto bruh = errorTmp;                                           \
        if (bruh != OK_STATUS) {                                   \
            LOG_ERROR(getErrorMessageFunc(bruh));                  \
            assert(bruh != OK_STATUS);                             \
            return bruh;                                           \
        }                                                           \
    } while(0)

#define COMMON_IF_NOT_COND_RETURN(condition, error, getErrorMessageFunc)                        \
    do {                                                            \
        bool tmpCondition = (condition);                            \
        if (!tmpCondition) {                                        \
            LOG_ERROR(getErrorMessageFunc(error));                      \
            LOG_DEBUG(#condition);\
            assert(#condition && tmpCondition);                                   \
            return error;                                           \
        }                                                           \
    } while(0)

// ASK: should I create tmp variable for this case too?
#define COMMON_IF_ARG_NULL_RETURN(arg, error, getErrorMessageFunc)                       \
    do {                                                            \
        if (arg == NULL) {                                          \
            LOG_ERROR(getErrorMessageFunc(error));                      \
            assert(arg != NULL);                                    \
            return error;                                           \
        }                                                           \
    } while (0)





#define COMMON_IF_SUBMODULE_ERR_RETURN(errorTmp, getErrorMessageFunc, OK_STATUS, returnError)                                        \
    do {                                                            \
        auto bruh = errorTmp;                                           \
        if (bruh != OK_STATUS) {                                   \
            LOG_ERROR(getErrorMessageFunc(bruh));                  \
            assert(bruh != OK_STATUS);                             \
            return returnError;                                           \
        }                                                           \
    } while(0)


#endif
