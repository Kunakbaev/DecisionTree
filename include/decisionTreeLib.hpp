#ifndef DECISION_TREE_LIB_HPP
#define DECISION_TREE_LIB_HPP

#include "decisionTreeErrors.hpp"
#include "dumperStruct.hpp"

typedef bool (decisionTreeCompFuncPtr)(const void* a, const void* b);

struct Node {
    void*            data;
    size_t           left;
    size_t           right;
    size_t           memBuffIndex; // ASK: is this field necessary
};

struct DecisionTree {
    size_t                   root;
    Node*                    memBuff;
    size_t                   memBuffSize;
    size_t                   freeNodeIndex;
    Dumper                   dumper;
    decisionTreeCompFuncPtr* comparator;
    const char*              formatForNodeData; // ASK: how to make it const
};

DecisionTreeErrors constructDecisionTree(DecisionTree* tree, Dumper* dumper,
                                         decisionTreeCompFuncPtr comparator,
                                         const char* formatForNodeData);
DecisionTreeErrors addNewNodeToDecisionTree(DecisionTree* tree, const void* value);
DecisionTreeErrors dumpDecisionTree(DecisionTree* tree);
DecisionTreeErrors destructDecisionTree(DecisionTree* tree);

// comparators for decision tree

#define CMP(comparatorName, code) \
    bool comparatorName(const void* a, const void* b);

#include "../include/decisionTreeComparatorsPlainText.in"

#undef CMP

#endif
