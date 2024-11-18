#ifndef AKINATOR_LIB_ERRORS_HPP
#define AKINATOR_LIB_ERRORS_HPP

#define ERROR_UNPACK(errName, errCode, errMessage) \
    errName = errCode,

enum AkinatorErrors {
    AKINATOR_STATUS_OK = 0,
    #include "akinatorLibErrorsPlainText.in"
};

#undef ERROR_UNPACK

const char* getAkinatorErrorMessage(AkinatorErrors error);

#endif
