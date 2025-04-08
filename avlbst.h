#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <cmath>

#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
  AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
    AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);

};


template <class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* leftChild = node->getLeft();
    if (!leftChild) return node;  // safety check
    
    // update child pointers
    node->setLeft(leftChild->getRight());
    if (leftChild->getRight() != nullptr)
        leftChild->getRight()->setParent(node);
    
    // update parent pointers
    AVLNode<Key, Value>* parent = node->getParent();
    leftChild->setParent(parent);
    if (parent != nullptr) {
        if (parent->getLeft() == node)
            parent->setLeft(leftChild);
        else
            parent->setRight(leftChild);
    }
    
    // set node as right child of leftChild
    leftChild->setRight(node);
    node->setParent(leftChild);
    
    
    int nodeNewBalance = node->getBalance() - 1 - std::max(0, static_cast<int>(leftChild->getBalance()));
    int leftChildNewBalance = leftChild->getBalance() - 1 + std::min(0, nodeNewBalance);
    node->setBalance(nodeNewBalance);
    leftChild->setBalance(leftChildNewBalance);
    
    return leftChild;
}

template <class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* rightChild = node->getRight();
    if (!rightChild) return node;  // safety check
    
    // update child pointers
    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft() != nullptr)
        rightChild->getLeft()->setParent(node);
    
    // update parent pointers
    AVLNode<Key, Value>* parent = node->getParent();
    rightChild->setParent(parent);
    if (parent != nullptr) {
        if (parent->getLeft() == node)
            parent->setLeft(rightChild);
        else
            parent->setRight(rightChild);
    }
    
    // set node as left child of rightChild
    rightChild->setLeft(node);
    node->setParent(rightChild);
    
    
    int nodeNewBalance = node->getBalance() + 1 - std::min(0, static_cast<int>(rightChild->getBalance()));
    int rightChildNewBalance = rightChild->getBalance() + 1 + std::max(0, nodeNewBalance);
    node->setBalance(nodeNewBalance);
    rightChild->setBalance(rightChildNewBalance);
    
    return rightChild;
}

template <class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item) {
    // if tree is empty, create a new root and exit
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }
    
    // store nodes along the insertion path
    std::vector<AVLNode<Key, Value>*> ancestors;
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;
    
    // find the spot for insertion
    while (current != nullptr) {
        parent = current;
        ancestors.push_back(current);
        if (new_item.first < current->getKey())
            current = current->getLeft();
        else if (new_item.first > current->getKey())
            current = current->getRight();
        else {
            // key already exists, so update its value
            current->setValue(new_item.second);
            return;
        }
    }
    
    // create the new node and attach it to parent
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (new_item.first < parent->getKey())
        parent->setLeft(newNode);
    else
        parent->setRight(newNode);
    
    // walk back up and update balances, checking for rotations
    for (int i = static_cast<int>(ancestors.size()) - 1; i >= 0; i--) {
        AVLNode<Key, Value>* node = ancestors[i];
        if (new_item.first < node->getKey())
            node->setBalance(node->getBalance() + 1);  // left side grew
        else
            node->setBalance(node->getBalance() - 1);  // right side grew
        
        // if balance is now zero, height didn't increase further so stop
        if (node->getBalance() == 0)
            break;
        
        // check for imbalance and do rotations if needed
        if (node->getBalance() == 2) { // left heavy
            AVLNode<Key, Value>* leftChild = node->getLeft();
            if (leftChild != nullptr && leftChild->getBalance() >= 0) {
                // single right rotation
                AVLNode<Key, Value>* rotated = rotateRight(node);
                AVLNode<Key, Value>* p = node->getParent();
                if (p == nullptr)
                    this->root_ = rotated;
                else {
                    if (p->getLeft() == node)
                        p->setLeft(rotated);
                    else
                        p->setRight(rotated);
                }
            } else {
                // left-right rotation: rotate left on node's left child then right on node
                AVLNode<Key, Value>* newLeft = rotateLeft(node->getLeft());
                node->setLeft(newLeft);
                newLeft->setParent(node);
                AVLNode<Key, Value>* rotated = rotateRight(node);
                AVLNode<Key, Value>* p = node->getParent();
                if (p == nullptr)
                    this->root_ = rotated;
                else {
                    if (p->getLeft() == node)
                        p->setLeft(rotated);
                    else
                        p->setRight(rotated);
                }
            }
            break;
        } else if (node->getBalance() == -2) { // right heavy
            AVLNode<Key, Value>* rightChild = node->getRight();
            if (rightChild != nullptr && rightChild->getBalance() <= 0) {
                // single left rotation
                AVLNode<Key, Value>* rotated = rotateLeft(node);
                AVLNode<Key, Value>* p = node->getParent();
                if (p == nullptr)
                    this->root_ = rotated;
                else {
                    if (p->getLeft() == node)
                        p->setLeft(rotated);
                    else
                        p->setRight(rotated);
                }
            } else {
                // right-left rotation: rotate right on node's right child then left on node
                AVLNode<Key, Value>* newRight = rotateRight(node->getRight());
                node->setRight(newRight);
                newRight->setParent(node);
                AVLNode<Key, Value>* rotated = rotateLeft(node);
                AVLNode<Key, Value>* p = node->getParent();
                if (p == nullptr)
                    this->root_ = rotated;
                else {
                    if (p->getLeft() == node)
                        p->setLeft(rotated);
                    else
                        p->setRight(rotated);
                }
            }
            break;
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    // store the path to the node to remove
    std::vector<AVLNode<Key, Value>*> ancestors;
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;
    
    // find the node with the given key
    while (node != nullptr) {
        if (key < node->getKey()) {
            ancestors.push_back(node);
            node = node->getLeft();
        } else if (key > node->getKey()) {
            ancestors.push_back(node);
            node = node->getRight();
        } else {
            break;
        }
    }
    if (node == nullptr)
        throw KeyError();
    
    // if the node has two children, swap its contents with its in-order predecessor
    if (node->getLeft() != nullptr && node->getRight() != nullptr) {
        ancestors.push_back(node);
        AVLNode<Key, Value>* pred = node->getLeft();
        while (pred->getRight() != nullptr) {
            ancestors.push_back(pred);
            pred = pred->getRight();
        }
        // swap the nodes using nodeSwap from bst.h
        BinarySearchTree<Key, Value>::nodeSwap(node, pred);
        node = pred;
    }
    
    // now node has at most one child
    AVLNode<Key, Value>* child = nullptr;
    if (node->getLeft() != nullptr)
        child = node->getLeft();
    else if (node->getRight() != nullptr)
        child = node->getRight();
    
    parent = node->getParent();
    if (child != nullptr)
        child->setParent(parent);
    if (parent == nullptr)
        this->root_ = child;
    else if (parent->getLeft() == node)
        parent->setLeft(child);
    else
        parent->setRight(child);
    delete node;
    
    // update balance factors on the way up
    AVLNode<Key, Value>* current = parent;
    while (current != nullptr) {
        AVLNode<Key, Value>* p = current->getParent();
        if (p != nullptr) {
            if (p->getLeft() == current)
                current->setBalance(current->getBalance() - 1);
            else
                current->setBalance(current->getBalance() + 1);
        }
        if (current->getBalance() == 1 || current->getBalance() == -1)
            break;
        if (current->getBalance() == 2) {
            AVLNode<Key, Value>* leftChild = current->getLeft();
            if (leftChild != nullptr && leftChild->getBalance() >= 0) {
               AVLNode<Key, Value>* rotated = rotateRight(current);
               if (rotated->getParent() == nullptr)
                   this->root_ = rotated;
               else {
                   AVLNode<Key, Value>* par = rotated->getParent();
                   if (par->getLeft() == current)
                       par->setLeft(rotated);
                   else
                       par->setRight(rotated);
               }
            } else {
               AVLNode<Key, Value>* newLeft = rotateLeft(current->getLeft());
               current->setLeft(newLeft);
               newLeft->setParent(current);
               AVLNode<Key, Value>* rotated = rotateRight(current);
               if (rotated->getParent() == nullptr)
                   this->root_ = rotated;
               else {
                   AVLNode<Key, Value>* par = rotated->getParent();
                   if (par->getLeft() == current)
                       par->setLeft(rotated);
                   else
                       par->setRight(rotated);
               }
            }
        } else if (current->getBalance() == -2) {
            AVLNode<Key, Value>* rightChild = current->getRight();
            if (rightChild != nullptr && rightChild->getBalance() <= 0) {
               AVLNode<Key, Value>* rotated = rotateLeft(current);
               if (rotated->getParent() == nullptr)
                   this->root_ = rotated;
               else {
                   AVLNode<Key, Value>* par = rotated->getParent();
                   if (par->getLeft() == current)
                       par->setLeft(rotated);
                   else
                       par->setRight(rotated);
               }
            } else {
               AVLNode<Key, Value>* newRight = rotateRight(current->getRight());
               current->setRight(newRight);
               newRight->setParent(current);
               AVLNode<Key, Value>* rotated = rotateLeft(current);
               if (rotated->getParent() == nullptr)
                   this->root_ = rotated;
               else {
                   AVLNode<Key, Value>* par = rotated->getParent();
                   if (par->getLeft() == current)
                       par->setLeft(rotated);
                   else
                       par->setRight(rotated);
               }
            }
        }
        current = p;
    }

}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key, Value>* n1, AVLNode<Key, Value>* n2)
{
    // Call the base class version of nodeSwap
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    
    // Since this is an AVL tree, we also need to swap the balance factors
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

#endif