#ifndef DECISION_TREE_LIB_HPP
#define DECISION_TREE_LIB_HPP

#include "decisionTreeErrors.hpp"

typedef void* node_data_type;

struct Node {
    node_data_type   data;
    size_t           left;
    size_t           right;
    size_t           memBuffIndex; // ASK: is this field necessary
};

struct DecisionTree {
    size_t root;
    Node*  memBuff;
    size_t memBuffSize;
    size_t freeNodeIndex;
};

DecisionTreeErrors constructDecisionTree(DecisionTree* tree);
DecisionTreeErrors addNewNodeToDecisionTree(DecisionTree* tree, node_data_type value);
DecisionTreeErrors dumpDecisionTree(DecisionTree* tree);

#endif
