#ifndef DECISION_TREE_LIB_HPP
#define DECISION_TREE_LIB_HPP

#include "decisionTreeErrors.hpp"

typedef void* node_data_type;

struct Node {
    node_data_type   data;
    Node*            left;
    Node*            right;
};

DecisionTreeErrors addNewNodeToDecisionTree(Node** startNode, node_data_type value);
DecisionTreeErrors dumpDecisionTree(const Node* tree);

#endif
