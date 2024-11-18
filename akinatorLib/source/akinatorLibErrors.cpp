#include "../include/akinatorLibErrors.hpp"

const char* getAkinatorErrorMessage(AkinatorErrors error) {
    #define ERROR_UNPACK(errName, errCode, errMessage)      \
        case errName:                                       \
            return "Akinator error: " errMessage ".\n";     \

    switch (error) {
        case AKINATOR_STATUS_OK:
            return "Akinator error: no errors occured.\n";

        #include "../include/akinatorLibErrorsPlainText.in"

        default:
            return "Akinator error: unknown error.\n";
    }

    #undef ERROR_UNPACK
}
