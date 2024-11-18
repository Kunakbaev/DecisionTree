#ifndef INCLUDE_AKINATOR_LIB_HPP
#define INCLUDE_AKINATOR_LIB_HPP

#include "akinatorLibErrors.hpp"
#include "akinatorLibAkinatorStructure.hpp"

AkinatorErrors constructAkinator(Akinator* akinator, Dumper* dumper);
AkinatorErrors tryToGuessObject(Akinator* akinator);
AkinatorErrors dumpAkinator(Akinator* akinator);
AkinatorErrors dumpCommonPathOf2Objects(Akinator* akinator, const char* objName1, const char* objName2);

AkinatorErrors showDefinitionOfObject(Akinator* akinator, const char* objName);
AkinatorErrors readAkinatorsDecisionTreeFromFile(Akinator* akinator, const char* fileName);
AkinatorErrors saveAkinatorsDecisionTreeToFile(Akinator* akinator, const char* fileName);
AkinatorErrors showImageOfAkinatorsDecisionTree(Akinator* akinator);
AkinatorErrors destructAkinator(Akinator* akinator);

#endif
