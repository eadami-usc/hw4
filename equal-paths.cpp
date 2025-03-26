#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool equalPathsHelper(Node* node, int depth, int& leafDepth);

bool equalPaths(Node* root) {
    if (root == nullptr) {
        return true;
    }
    
    int leafDepth = -1;
    return equalPathsHelper(root, 1, leafDepth);
}

bool equalPathsHelper(Node* node, int depth, int& leafDepth) {
    if (node == nullptr) {
        return true;
    }
    
    if (node->left == nullptr && node->right == nullptr) {
        // if this is the first leaf found, set the reference depth
        if (leafDepth == -1) {
            leafDepth = depth;
            return true;
        }
        // else, compare this leaf's depth with the reference
        return depth == leafDepth;
    }
    
    // check left and right subtrees    
    if (node->left != nullptr) {
        return equalPathsHelper(node->left, depth + 1, leafDepth);
    }
    
    if (node->right != nullptr) {
        return equalPathsHelper(node->right, depth + 1, leafDepth);
    }
    
    return equalPathsHelper(node->left, depth + 1, leafDepth) && equalPathsHelper(node->right, depth + 1, leafDepth);
}
