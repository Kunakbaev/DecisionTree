#ifndef TERMINAL_INTEFACE_INCLUDE_TERMINAL_INTERFACE_ERRORS_HPP
#define TERMINAL_INTEFACE_INCLUDE_TERMINAL_INTERFACE_ERRORS_HPP

#define ERROR_UNPACK(errName, errCode, errMessage) \
    errName = errCode,

enum TerminalInterfaceErrors {
    TERMINAL_INTERFACE_STATUS_OK = 0,
    #include "plainTextErrors.in"
};

#undef ERROR_UNPACK

const char* getTerminalInterfaceErrorMessage(TerminalInterfaceErrors error);

#endif
