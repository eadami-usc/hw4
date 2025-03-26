#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
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
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
    void removeFix(AVLNode<Key, Value>* node, int diff);

};


template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* rightChild = node->getRight();

    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft() != nullptr) {
        rightChild->getLeft()->setParent(node);
    }
    rightChild->setParent(node->getParent());
    if (node->getParent() == nullptr) {
        this->root_ = rightChild;
    } else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(rightChild);
    } else {
        node->getParent()->setRight(rightChild);
    }
    rightChild->setLeft(node);
    node->setParent(rightChild);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* leftChild = node->getLeft();

    node->setLeft(leftChild->getRight());
    if (leftChild->getRight() != nullptr) {
        leftChild->getRight()->setParent(node);
    }
    leftChild->setParent(node->getParent());
    if (node->getParent() == nullptr) {
        this->root_ = leftChild;
    } else if (node == node->getParent()->getRight()) {
        node->getParent()->setRight(leftChild);
    } else {
        node->getParent()->setLeft(leftChild);
    }
    leftChild->setRight(node);
    node->setParent(leftChild);
}


/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    Key key = new_item.first;
    Value value = new_item.second;

    // if empty tree, set new node, n, as root, b(n) = 0
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(key, value, nullptr);
        return;
    }

    // else insert n, set b(n) = 0, and look at its parent p
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    while (curr != nullptr) {
        if (key == curr->getKey()) {
            curr->setValue(value);
            return;
        } else if (key < curr->getKey()) {
            if (curr->getLeft() == nullptr) {
                curr->setLeft(new AVLNode<Key, Value>(key, value, curr));
                // if b(p) = 1, then b(p) = 0, and we are done
                if (curr->getBalance() == 1) {
                    curr->setBalance(0);
                // if b(p) = 0, then update b(p) and call insertFix(p, n)
                } else {
                    curr->updateBalance(-1);
                    insertFix(curr, curr->getLeft());
                }
                return;
            }
            curr = curr->getLeft();
        } else {
            if (curr->getRight() == nullptr) {
                curr->setRight(new AVLNode<Key, Value>(key, value, curr));
                // if b(p) = -1, then b(p) = 0, and we are done
                if (curr->getBalance() == -1) {
                    curr->setBalance(0);
                // if b(p) = 0, then update b(p) and call insertFix(p, n)
                } else {
                    curr->updateBalance(1);
                    insertFix(curr, curr->getRight());
                }
                return;
            }
            curr = curr->getRight();
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child)
{
    // if p is null or parent(p) is null, return
    if (parent == nullptr || parent->getParent() == nullptr) {
        return;
    }

    AVLNode<Key, Value>* grandparent = parent->getParent();

    // if p is the left child of g
    if (parent == grandparent->getLeft()) {
        grandparent->updateBalance(-1);
        if (grandparent->getBalance() == 0) {
            return;
        } else if (grandparent->getBalance() == -1) {
            insertFix(grandparent, parent);
        } else {
            if (child == parent->getLeft()) {
                // zig-zig
                rotateRight(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            } else {
                // zig-zag
                rotateLeft(parent);
                rotateRight(grandparent);
                if (child->getBalance() == -1) {
                    parent->setBalance(0);
                    grandparent->setBalance(1);
                } else if (child->getBalance() == 0) {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                } else if (child->getBalance() == 1) {
                    parent->setBalance(-1);
                    grandparent->setBalance(0);
                }
                child->setBalance(0);
            }
        }
    // if p is the right child of g
    } else {
        grandparent->updateBalance(1);
        if (grandparent->getBalance() == 0) {
            return;
        } else if (grandparent->getBalance() == 1) {
            insertFix(grandparent, parent);
        } else {
            if (child == parent->getRight()) {
                // zig-zig
                rotateLeft(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            } else {
                // zig-zag
                rotateRight(parent);
                rotateLeft(grandparent);
                if (child->getBalance() == 1) {
                    parent->setBalance(0);
                    grandparent->setBalance(-1);
                } else if (child->getBalance() == 0) {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                } else if (child->getBalance() == -1) {
                    parent->setBalance(1);
                    grandparent->setBalance(0);
                }
                child->setBalance(0);
            }
        }
    }
}


/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // find node to remove, n
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if (nodeToRemove == nullptr) {
        return;
    }

    // if n has two children, swap with predecessor
    if (nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(nodeToRemove));
        nodeSwap(nodeToRemove, pred);
    }

    AVLNode<Key, Value>* parent = nodeToRemove->getParent();
    AVLNode<Key, Value>* child = nullptr;
    int diff = 0;

    // delete n from tree
    // if n has one child, get ready to promote child
    if (nodeToRemove->getLeft() != nullptr) {
        child = nodeToRemove->getLeft();
    } else {
        child = nodeToRemove->getRight();
    }

    // replace n with child (promotion)
    if (parent == nullptr) {
        this->root_ = child;
    } else if (nodeToRemove == parent->getLeft()) {
        parent->setLeft(child);
        // if n is a left child of p, diff = 1
        diff = 1;
    } else {
        parent->setRight(child);
        // if n is a right child of p, diff = -1
        diff = -1;
    }

    // update child's parent
    if (child != nullptr) {
        child->setParent(parent);
    }

    delete nodeToRemove;

    // patch tree after deletion
    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
    // if n is null, return
    if (node == nullptr) {
        return;
    }

    // compute next recursive call's arguments now before altering the tree
    AVLNode<Key, Value>* parent = node->getParent();
    int nextDiff = 0;

    if (parent != nullptr) {
        if (node == parent->getLeft()) {
            nextDiff = 1;
        } else {
            nextDiff = -1;
        }
    }

    // if diff = -1
    if (diff == -1) {
        // case 1: b(n) + diff == -2
        if (node->getBalance() + diff == -2) {
            AVLNode<Key, Value>* child = node->getLeft();
            if (child->getBalance() == -1) {
                // zig-zig
                rotateRight(node);
                node->setBalance(0);
                child->setBalance(0);
                removeFix(parent, nextDiff);
            } else if (child->getBalance() == 0) {
                // zig-zig
                rotateRight(node);
                node->setBalance(-1);
                child->setBalance(1);
            } else if (child->getBalance() == 1) {
                // zig-zag
                AVLNode<Key, Value>* grandchild = child->getRight();
                rotateLeft(child);
                rotateRight(node);
                if (grandchild->getBalance() == 1) {
                    node->setBalance(0);
                    child->setBalance(-1);
                } else if (grandchild->getBalance() == 0) {
                    node->setBalance(0);
                    child->setBalance(0);
                } else if (grandchild->getBalance() == -1) {
                    node->setBalance(1);
                    child->setBalance(0);
                }
                grandchild->setBalance(0);
                removeFix(parent, nextDiff);
            }
        }
        // case 2: b(n) + diff == -1
        else if (node->getBalance() + diff == -1) {
            node->setBalance(-1);
        }
        // case 3: b(n) + diff == 0
        else if (node->getBalance() + diff == 0) {
            node->setBalance(0);
            removeFix(parent, nextDiff);
        }
    // if diff = 1
    } else {
        // case 1: b(n) + diff == 2
        if (node->getBalance() + diff == 2) {
            AVLNode<Key, Value>* child = node->getRight();
            if (child->getBalance() == 1) {
                // zig-zig
                rotateLeft(node);
                node->setBalance(0);
                child->setBalance(0);
                removeFix(parent, nextDiff);
            } else if (child->getBalance() == 0) {
                // zig-zig
                rotateLeft(node);
                node->setBalance(1);
                child->setBalance(-1);
            } else if (child->getBalance() == -1) {
                // zig-zag
                AVLNode<Key, Value>* grandchild = child->getLeft();
                rotateRight(child);
                rotateLeft(node);
                if (grandchild->getBalance() == -1) {
                    node->setBalance(0);
                    child->setBalance(1);
                } else if (grandchild->getBalance() == 0) {
                    node->setBalance(0);
                    child->setBalance(0);
                } else if (grandchild->getBalance() == 1) {
                    node->setBalance(-1);
                    child->setBalance(0);
                }
                grandchild->setBalance(0);
                removeFix(parent, nextDiff);
            }
        }
        // case 2: b(n) + diff == 1
        else if (node->getBalance() + diff == 1) {
            node->setBalance(1);
        }
        // case 3: b(n) + diff == 0
        else if (node->getBalance() + diff == 0) {
            node->setBalance(0);
            removeFix(parent, nextDiff);
        }
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
