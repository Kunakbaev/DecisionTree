#include "../include/terminalInterfaceErrors.hpp"

const char* getTerminalInterfaceErrorMessage(TerminalInterfaceErrors error) {
    #define ERROR_UNPACK(errName, errCode, errMessage)      \
        case errName:                                       \
            return "Terminal interface error: " errMessage ".\n";     \

    switch (error) {
        case TERMINAL_INTERFACE_STATUS_OK:
            return "Terminal interface error: no errors occured.\n";

        #include "../include/plainTextErrors.in"

        default:
            return "Terminal interface error: unknown error.\n";
    }

    #undef ERROR_UNPACK
}
