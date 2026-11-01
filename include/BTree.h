#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <utility>
#include "Node.h"
#include "InternalNode.h"
#include "LeafNode.h"

using namespace std;

template <typename Type, int Ord>
class BTree
{
public:
    Node<Type, Ord>* root;

    BTree()
        : root(nullptr)
    {
    }

    void splitChild(Node<Type, Ord>* parent, int childIdx);
    void insertRecursive(Node<Type, Ord>* node, const Type& value);
    void merge(Node<Type, Ord>* parent, int indx);
    void insert(Type key);
    void remove(Type key);
    pair<Node<Type, Ord>*, int> search(Type key);
    void display();
    void borrowLeft(Node<Type, Ord>* parent, int indx);
    void borrowRight(Node<Type, Ord>* parent, int indx);

    int minKeys() const
    {
        return (Ord+1)/2-1;
    }

    Type getPredecessor(Node<Type, Ord>* node, int indx);
    Type getSuccessor(Node<Type, Ord>* node, int indx);
    void removeFromLeaf(Node<Type, Ord>* node, int indx);
    void removeFromNonLeaf(InternalNode<Type, Ord>* node, int indx);
    void fill(InternalNode<Type, Ord>* parent, int idx);
    void removeRecursive(Node<Type, Ord>* node, const Type& key);
    void deleteNode(Node<Type, Ord>* node);
    void displayRecursive(Node<Type, Ord>* node);
};

#include "BTree.tpp"

#endif