#ifndef INTERNAL_NODE_H
#define INTERNAL_NODE_H

#include "Node.h"

template <typename Type, int Ord>
struct InternalNode : public Node<Type, Ord>
{
    Node<Type, Ord>* child[Ord+1];

    InternalNode()
        : Node<Type, Ord>(false)
    {
        for(int i=0; i<Ord+1; ++i)
            child[i]=nullptr;
    }
};

#endif