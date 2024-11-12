#ifndef DECISION_TREE_LIB_HPP
#define DECISION_TREE_LIB_HPP

#include "decisionTreeErrors.hpp"
#include "dumperStruct.hpp"

struct Node {
    char*      data; // question (if node is not leaf), object name (if node is leaf)
    size_t     left; // no (0)
    size_t     right; // yes (1)
    size_t     memBuffIndex; // ASK: is this field necessary
    size_t     parent;
};

struct DecisionTree {
    size_t                   root;
    Node*                    memBuff;
    size_t                   memBuffSize;
    size_t                   freeNodeIndex;
    Dumper                   dumper;
};

DecisionTreeErrors constructDecisionTree(DecisionTree* tree, Dumper* dumper);
DecisionTreeErrors printPathToObjByName(const DecisionTree* tree, const char* objName);
//DecisionTreeErrors getPathToDecisionTreeNode(const DecisionTree* tree, const Node* node, size_t** path);
DecisionTreeErrors tryToGuessObject(DecisionTree* tree);
DecisionTreeErrors readDecisionTreeFromFile(DecisionTree* tree, const char* fileName);
DecisionTreeErrors saveDecisionTreeToFile(DecisionTree* tree, const char* fileName);
DecisionTreeErrors dumpDecisionTree(DecisionTree* tree);
DecisionTreeErrors dumpCommonPathOf2Objects(DecisionTree* tree, const char* objName1, const char* objName2);
DecisionTreeErrors destructDecisionTree(DecisionTree* tree);

#endif
