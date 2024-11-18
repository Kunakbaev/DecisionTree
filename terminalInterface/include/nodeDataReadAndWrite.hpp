#ifndef TERMINAL_INTERFACE_INCLUDE_NODE_DATA_READ_AND_WRITE_HPP
#define TERMINAL_INTERFACE_INCLUDE_NODE_DATA_READ_AND_WRITE_HPP

const char* getStringFromVoidPtr(const void* voidPtr);
const char* nodeDataPrinter(const void* num);
void nodeDataReader(void** num, const char* line);

#endif
