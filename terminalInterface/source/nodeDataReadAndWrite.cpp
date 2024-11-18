#include <stdlib.h>
#include <string.h>

#include "logLib.hpp"
#include "../include/nodeDataReadAndWrite.hpp"

const char* getStringFromVoidPtr(const void* voidPtr) {
    //assert(voidPtr != NULL);
    const char* ptr = (const char*)(voidPtr);
    if (ptr == NULL)
        return "";
    return (const char*)(voidPtr);
}

const char* nodeDataPrinter(const void* num) {
    // LOG_DEBUG("node data printer");
    const char* buffer = getStringFromVoidPtr(num);
    return buffer;
}

void nodeDataReader(void** num, const char* line) {
    // LOG_DEBUG("node data reader");
    if (line == NULL) return;

    *num = calloc(strlen(line) + 1, sizeof(char));
    assert(*num != NULL);

    LOG_DEBUG_VARS(line, num);
    sscanf(line, "%[^\n]s", *num);
    // LOG_ERROR("---------------------");
    // LOG_DEBUG_VARS(num, line, (const char*)(*num));
}
