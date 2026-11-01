#ifndef NODE_H
#define NODE_H

template <typename Type, int Ord>
struct Node
{
    bool is_leaf;
    int num_keys;
    Type key[Ord]; 

    Node(bool leaf)
        : is_leaf(leaf), num_keys(0)
    {
    }
};

#endif