
#include "../include/decisionTreeLib.hpp"

#define CMP(comparatorName, code) \
    bool comparatorName(const void* a, const void* b) {\
        return code;\
    }\

#include "../include/decisionTreeComparatorsPlainText.in"

#undef CMP

