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
    if (!leftChild) return node;
    
    node->setLeft(leftChild->getRight());
    if (leftChild->getRight() != nullptr)
        leftChild->getRight()->setParent(node);
    
    AVLNode<Key, Value>* parent = node->getParent();
    leftChild->setParent(parent);
    if (parent != nullptr) {
        if (parent->getLeft() == node)
            parent->setLeft(leftChild);
        else
            parent->setRight(leftChild);
    } else {
        this->root_ = leftChild;
    }
    
    leftChild->setRight(node);
    node->setParent(leftChild);
    
    node->setBalance(node->getBalance() - 1 - std::max(0, static_cast<int>(leftChild->getBalance())));
    leftChild->setBalance(leftChild->getBalance() - 1 + std::min(0, static_cast<int>(node->getBalance())));
    
    return leftChild;
}

template <class Key, class Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node) {
    AVLNode<Key, Value>* rightChild = node->getRight();
    if (!rightChild) return node;
    
    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft() != nullptr)
        rightChild->getLeft()->setParent(node);
    
    AVLNode<Key, Value>* parent = node->getParent();
    rightChild->setParent(parent);
    if (parent != nullptr) {
        if (parent->getLeft() == node)
            parent->setLeft(rightChild);
        else
            parent->setRight(rightChild);
    } else {
        this->root_ = rightChild;
    }
    
    rightChild->setLeft(node);
    node->setParent(rightChild);
    
    node->setBalance(node->getBalance() + 1 - std::min(0, static_cast<int>(rightChild->getBalance())));
    rightChild->setBalance(rightChild->getBalance() + 1 + std::max(0, static_cast<int>(node->getBalance())));
    
    return rightChild;
}

template <class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value>& new_item) {
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }
    
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;
    bool wentLeft = false;
    std::vector<std::pair<AVLNode<Key, Value>*, bool>> path;
    
    while (current != nullptr) {
        parent = current;
        if (new_item.first < current->getKey()) {
            current = current->getLeft();
            wentLeft = true;
        } else if (new_item.first > current->getKey()) {
            current = current->getRight();
            wentLeft = false;
        } else {
            current->setValue(new_item.second);
            return;
        }
        path.push_back({parent, wentLeft});
    }
    
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if (wentLeft)
        parent->setLeft(newNode);
    else
        parent->setRight(newNode);
    
    for (int i = static_cast<int>(path.size()) - 1; i >= 0; i--) {
        AVLNode<Key, Value>* node = path[i].first;
        bool childWentLeft = path[i].second;
        
        if (childWentLeft)
            node->updateBalance(1);
        else
            node->updateBalance(-1);
        
        if (node->getBalance() == 0) {
            break;
        }
        else if (node->getBalance() == 2) {
            AVLNode<Key, Value>* leftChild = node->getLeft();
            if (leftChild->getBalance() == -1) {
                rotateLeft(leftChild);
            }
            rotateRight(node);
            break;
        }
        else if (node->getBalance() == -2) {
            AVLNode<Key, Value>* rightChild = node->getRight();
            if (rightChild->getBalance() == 1) {
                rotateRight(rightChild);
            }
            rotateLeft(node);
            break;
        }
    }
}

template <class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key) {
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->root_);
    
    while (node != nullptr && node->getKey() != key) {
        if (key < node->getKey())
            node = node->getLeft();
        else
            node = node->getRight();
    }
    
    if (node == nullptr)
        return;
    
    if (node->getLeft() != nullptr && node->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = node->getLeft();
        while (pred->getRight() != nullptr) {
            pred = pred->getRight();
        }
        
        nodeSwap(node, pred);
    }
    
    AVLNode<Key, Value>* parent = node->getParent();
    bool isLeftChild = (parent != nullptr && parent->getLeft() == node);
    
    AVLNode<Key, Value>* child = nullptr;
    if (node->getLeft() != nullptr) {
        child = node->getLeft();
    } else if (node->getRight() != nullptr) {
        child = node->getRight();
    }
    
    if (parent == nullptr) {
        this->root_ = child;
    } else {
        if (isLeftChild) {
            parent->setLeft(child);
        } else {
            parent->setRight(child);
        }
    }
    
    if (child != nullptr) {
        child->setParent(parent);
    }
    
    AVLNode<Key, Value>* current = parent;
    int8_t heightDiff = isLeftChild ? -1 : 1;
    
    delete node;
    
    while (current != nullptr) {
        current->updateBalance(heightDiff);
        
        if (current->getBalance() == 2) {
            AVLNode<Key, Value>* leftChild = current->getLeft();
            if (leftChild != nullptr) {
                if (leftChild->getBalance() == -1) {
                    rotateLeft(leftChild);
                }
                current = rotateRight(current);
            }
        } 
        else if (current->getBalance() == -2) {
            AVLNode<Key, Value>* rightChild = current->getRight();
            if (rightChild != nullptr) {
                if (rightChild->getBalance() == 1) {
                    rotateRight(rightChild);
                }
                current = rotateLeft(current);
            }
        }
        
        if (current->getBalance() == 0) {
            AVLNode<Key, Value>* parent = current->getParent();
            if (parent != nullptr) {
                heightDiff = (parent->getLeft() == current) ? -1 : 1;
            }
            current = parent;
        } else {
            break;
        }
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