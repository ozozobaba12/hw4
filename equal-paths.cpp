#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

//helper function that checks height of a given node (only used on the root)
int findHeight(Node * root);
//helper function that checks if height from findHeight() is equal to left/right paths of root
bool checkEqualPaths(Node * root, int height, int pathLength);

bool equalPaths(Node * root)
{
    // Add your code below
    //If there is no root, then all paths are the same length
    if (root == nullptr) {
        return true;
    }
  //If there is only a root, then all paths are the same length
    if (root->left == nullptr && root->right == nullptr) {
        return true;
    }

    //all paths' heights should be equal to depth
    int targetHeight = findHeight(root);

    return checkEqualPaths(root, targetHeight, 0);

}

//helper function that checks height of a given node (only used on the root)
int findHeight(Node * root)
{
    if (root == nullptr) {
        return -1;
    }

    if (root->left == nullptr && root->right == nullptr) {
        return 0;
    }

    if (root->left != nullptr) {
        return 1 + findHeight(root->left);
    }
    
    
    return 1 + findHeight(root->right);

    
}

//helper function that checks if height from findHeight() is equal to left/right paths of root
bool checkEqualPaths(Node * root, int height, int pathLength)
{
    if (root == nullptr) {
        return true;
    }

    if (root->left == nullptr && root->right == nullptr) {
        return pathLength == height;
    }

    return checkEqualPaths(root->left, height, pathLength + 1) &&
           checkEqualPaths(root->right, height, pathLength + 1);
}

