#ifndef INCLUDE_DUMPER_STRUCT
#define INCLUDE_DUMPER_STRUCT

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

// TODO: move to logger lib

struct Dumper {
    const char* dirForLogsPath;
    const char* outputFileFormat;
    size_t      maxNumOfNodesToDraw;
    size_t      numberOfLogsBefore; // ASK: we want every file to have unique name, file with bigger number was outputed later in program, maybe it will be automaticly sorted by time of creation
    FILE*       allLogsFile;
};

#endif
