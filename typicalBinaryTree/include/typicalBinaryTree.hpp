#ifndef TYPICAL_BIN_TREE_LIB_HPP
#define TYPICAL_BIN_TREE_LIB_HPP

#include "typicalBinaryTreeErrors.hpp"
#include "../../include/dumperStruct.hpp"

// NOTE:
// this functions should be safe, otherwise any errors occuring inside will not be caught

typedef const char* (*nodeDataToStringFuncPtr)(const void* value);
typedef void        (*stringToNodeDataFuncPtr)(void** value, const char* line);
typedef int         (*cmpNodesByValueFuncPtr )(const void* obj1 , const void* obj2);

struct Node {
    void*      data;         // node's data
    size_t     left;         // left son
    size_t     right;        // right son
    size_t     memBuffIndex; // ASK: is this field necessary
    size_t     parent;       // index in mem buff array of node's parent
};

struct TypicalBinaryTree {
    size_t                       root;
    Node*                        memBuff;
    size_t                       memBuffSize;
    size_t                       freeNodeIndex;
    Dumper                       dumper;
    nodeDataToStringFuncPtr      nodeDataToString;
    stringToNodeDataFuncPtr      stringToNodeData;
    cmpNodesByValueFuncPtr       cmpNodesByValue;
};

TypicalBinaryTreeErrors constructTypicalBinaryTree(TypicalBinaryTree* tree, Dumper* dumper,
                                                   nodeDataToStringFuncPtr nodeDataPrinter,
                                                   stringToNodeDataFuncPtr nodeDataReader,
                                                   cmpNodesByValueFuncPtr  nodesComparator);

TypicalBinaryTreeErrors addNewObjectToTypicalBinaryTree(TypicalBinaryTree* tree, size_t parentInd,
                                                        const void* value, bool isToLeftSon,
                                                        size_t* newNodeIndex);
TypicalBinaryTreeErrors printPathToObjByValue(const TypicalBinaryTree* tree, const void* value);
TypicalBinaryTreeErrors getPathToObjByVertInd(const TypicalBinaryTree* tree, size_t vertInd,
                                              size_t* pathLen, size_t** path);
TypicalBinaryTreeErrors getPathToObjByValue(const TypicalBinaryTree* tree, const void* value,
                                            size_t* pathLen, size_t** path);

//TypicalBinaryTreeErrors getPathToTypicalBinaryTreeNode(const TypicalBinaryTree* tree, const Node* node, size_t** path);
TypicalBinaryTreeErrors readTypicalBinaryTreeFromFile(TypicalBinaryTree* tree, const char* fileName);
TypicalBinaryTreeErrors saveTypicalBinaryTreeToFile(TypicalBinaryTree* tree, const char* fileName);

TypicalBinaryTreeErrors dumpTypicalBinaryTree(TypicalBinaryTree* tree);
TypicalBinaryTreeErrors openImageOfCurrentStateTypicalBinaryTree(TypicalBinaryTree* tree);
TypicalBinaryTreeErrors dumpCommonPathOf2Objects(TypicalBinaryTree* tree, size_t obj1, size_t obj2);
TypicalBinaryTreeErrors dumpCommonPathOf2ObjectsByValue(TypicalBinaryTree* tree,
                                                        const void* objValue_1, const void* objValue_2);

TypicalBinaryTreeErrors getBinTreeNodeByVertIndex(TypicalBinaryTree* tree, size_t vertInd, Node* result);
TypicalBinaryTreeErrors isNodeAleftSonOfParent(TypicalBinaryTree* tree, size_t parentInd, size_t vertInd, bool* is);
TypicalBinaryTreeErrors isValueInBinTree(const TypicalBinaryTree* tree, const void* value, bool* isIn);
TypicalBinaryTreeErrors isBinTreeEmpty(const TypicalBinaryTree* tree, bool* is);
TypicalBinaryTreeErrors isBinTreeNodeLeaf(const TypicalBinaryTree* tree, size_t currentNodeInd, bool* is);

TypicalBinaryTreeErrors destructTypicalBinaryTree(TypicalBinaryTree* tree);

#endif
