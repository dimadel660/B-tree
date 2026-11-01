//Eman Hosny Hassan   20240102
//Grace Emad Nabil    20240134
//Dima Adel Abdualla  20240183
//Rahma Yasser Moussa 20240
//Rawan Ahemd         202401

template <typename Type, int Ord>
void BTree<Type, Ord>::insert(Type value)
{
    if(root==nullptr)
    {
        root=new LeafNode<Type, Ord>();
        root->is_leaf=true;
        root->num_keys=1;
        root->key[0]=value;
        return;
    }
    insertRecursive(root, value);
    if(root->num_keys==Ord)
    {
        InternalNode<Type, Ord>* newRoot=new InternalNode<Type, Ord>();
        newRoot->child[0]=root;
        splitChild(newRoot, 0);
        root=newRoot;
    }
}

template <typename Type, int Ord>
void BTree<Type, Ord>::insertRecursive(Node<Type, Ord>* node, const Type& value)
{
    if(node->is_leaf)
    {
        int i=node->num_keys-1;
        while(i>=0)
        {
            if(value>=node->key[i]) break;
            node->key[i+1]=node->key[i];
            i--;
        }
        if(i>=0)
        {
            if(node->key[i]==value)
                return;
        }
        node->key[i+1]=value;
        node->num_keys++;
        return;
    }
    InternalNode<Type, Ord>* internal=static_cast<InternalNode<Type, Ord>*>(node);
    int childIdx=0;
    while(childIdx<node->num_keys)
    {
        if(value<node->key[childIdx]) break;
        childIdx++;
    }
    insertRecursive(internal->child[childIdx], value);
    if(internal->child[childIdx]->num_keys==Ord)
        splitChild(internal, childIdx);
}

template <typename Type, int Ord>
void BTree<Type, Ord>::splitChild(Node<Type, Ord>* parent, int childIdx)
{
    InternalNode<Type, Ord>* internalParent=static_cast<InternalNode<Type, Ord>*>(parent);
    Node<Type, Ord>* childCopy=internalParent->child[childIdx];
    const int middle=Ord/2;
    Node<Type, Ord>* upperHalf;
    if(childCopy->is_leaf)
        upperHalf=new LeafNode<Type, Ord>();
    else
        upperHalf=new InternalNode<Type, Ord>();
    upperHalf->is_leaf=childCopy->is_leaf;
    upperHalf->num_keys=Ord-middle-1;
    for(int i=0; i<upperHalf->num_keys; i++)
        upperHalf->key[i]=childCopy->key[middle+1+i];
    if(!childCopy->is_leaf)
    {
        InternalNode<Type, Ord>* oldInternal=static_cast<InternalNode<Type, Ord>*>(childCopy);
        InternalNode<Type, Ord>* newInternal=static_cast<InternalNode<Type, Ord>*>(upperHalf);
        for(int i=0; i<=upperHalf->num_keys; i++)
        {
            newInternal->child[i]=oldInternal->child[middle+1+i];
            oldInternal->child[middle+1+i]=nullptr;
        }
    }
    Type median=childCopy->key[middle];
    childCopy->num_keys=middle;
    for(int i=parent->num_keys; i>=childIdx+1; i--)
        internalParent->child[i+1]=internalParent->child[i];
    internalParent->child[childIdx+1]=upperHalf;
    for(int i=parent->num_keys-1; i>=childIdx; i--)
        parent->key[i+1]=parent->key[i];
    parent->key[childIdx]=median;
    parent->num_keys++;
}

template <typename Type, int Ord>
pair<Node<Type, Ord>*, int> BTree<Type, Ord>::search(Type key)
{
    Node<Type, Ord>* current=root;
    while(current!=nullptr)
    {
        int indx=0;
        while(indx<current->num_keys)
        {
            if(current->key[indx]>=key) break;
            indx++;
        }
        if(indx<current->num_keys)
        {
            if(current->key[indx]==key)
                return {current, indx};
        }
        if(current->is_leaf)
            return {nullptr, -1};
        current=static_cast<InternalNode<Type, Ord>*>(current)->child[indx];
    }
    return {nullptr, -1};
}

template <typename Type, int Ord>
void BTree<Type, Ord>::deleteNode(Node<Type, Ord>* node)
{
    if(node->is_leaf)
        delete static_cast<LeafNode<Type, Ord>*>(node);
    else
        delete static_cast<InternalNode<Type, Ord>*>(node);
}

template <typename Type, int Ord>
void BTree<Type, Ord>::removeFromLeaf(Node<Type, Ord>* node, int idx)
{
    for(int i=idx+1; i<node->num_keys; i++)
        node->key[i-1]=node->key[i];
    node->num_keys--;
}

template <typename Type, int Ord>
void BTree<Type, Ord>::removeFromNonLeaf(InternalNode<Type, Ord>* node, int idx)
{
    Node<Type, Ord>* left=node->child[idx];
    Node<Type, Ord>* right=node->child[idx+1];
    if(left->num_keys>minKeys() || right->num_keys<=minKeys())
    {
        Type predecessor=getPredecessor(node, idx);
        node->key[idx]=predecessor;
        removeRecursive(node->child[idx], predecessor);
        if(node->child[idx]->num_keys<minKeys())
            fill(node, idx);
    }
    else
    {
        Type successor=getSuccessor(node, idx);
        node->key[idx]=successor;
        removeRecursive(node->child[idx+1], successor);
        if(node->child[idx+1]->num_keys<minKeys())
            fill(node, idx+1);
    }
}

template <typename Type, int Ord>
void BTree<Type, Ord>::merge(Node<Type, Ord>* parent, int indx)
{
    InternalNode<Type, Ord>* internalParent=static_cast<InternalNode<Type, Ord>*>(parent);
    Node<Type, Ord>* left=internalParent->child[indx];
    Node<Type, Ord>* right=internalParent->child[indx+1];
    left->key[left->num_keys]=internalParent->key[indx];
    left->num_keys++;
    int oldLeftKeys=left->num_keys;
    for(int i=0; i<right->num_keys; i++)
        left->key[oldLeftKeys+i]=right->key[i];
    if(!right->is_leaf)
    {
        InternalNode<Type, Ord>* leftNode=static_cast<InternalNode<Type, Ord>*>(left);
        InternalNode<Type, Ord>* rightNode=static_cast<InternalNode<Type, Ord>*>(right);
        for(int i=0; i<=right->num_keys; i++)
            leftNode->child[oldLeftKeys+i]=rightNode->child[i];
    }
    left->num_keys=oldLeftKeys+right->num_keys;
    for(int i=indx; i<internalParent->num_keys-1; i++)
        internalParent->key[i]=internalParent->key[i+1];
    for(int i=indx+1; i<internalParent->num_keys; i++)
        internalParent->child[i]=internalParent->child[i+1];
    internalParent->child[internalParent->num_keys]=nullptr;
    internalParent->num_keys--;
    deleteNode(right);
}

template <typename Type, int Ord>
void BTree<Type, Ord>::borrowLeft(Node<Type, Ord>* parent, int indx)
{
    InternalNode<Type, Ord>* internalParent=static_cast<InternalNode<Type, Ord>*>(parent);
    Node<Type, Ord>* left=internalParent->child[indx];
    Node<Type, Ord>* current=internalParent->child[indx+1];
    for(int i=current->num_keys-1; i>=0; i--)
        current->key[i+1]=current->key[i];
    if(!current->is_leaf)
    {
        InternalNode<Type, Ord>* leftNode=static_cast<InternalNode<Type, Ord>*>(left);
        InternalNode<Type, Ord>* currentNode=static_cast<InternalNode<Type, Ord>*>(current);
        for(int i=current->num_keys; i>=0; i--)
            currentNode->child[i+1]=currentNode->child[i];
        currentNode->child[0]=leftNode->child[left->num_keys];
        leftNode->child[left->num_keys]=nullptr;
    }
    current->key[0]=internalParent->key[indx];
    internalParent->key[indx]=left->key[left->num_keys-1];
    current->num_keys++;
    left->num_keys--;
}

template <typename Type, int Ord>
void BTree<Type, Ord>::borrowRight(Node<Type, Ord>* parent, int indx)
{
    InternalNode<Type, Ord>* internalParent=static_cast<InternalNode<Type, Ord>*>(parent);
    Node<Type, Ord>* current=internalParent->child[indx];
    Node<Type, Ord>* right=internalParent->child[indx+1];
    current->key[current->num_keys]=internalParent->key[indx];
    internalParent->key[indx]=right->key[0];
    for(int i=0; i<right->num_keys-1; i++)
        right->key[i]=right->key[i+1];
    if(!right->is_leaf)
    {
        InternalNode<Type, Ord>* rightNode=static_cast<InternalNode<Type, Ord>*>(right);
        InternalNode<Type, Ord>* currentNode=static_cast<InternalNode<Type, Ord>*>(current);
        currentNode->child[current->num_keys+1]=rightNode->child[0];
        for(int i=0; i<right->num_keys; i++)
            rightNode->child[i]=rightNode->child[i+1];
        rightNode->child[right->num_keys]=nullptr;
    }
    right->num_keys--;
    current->num_keys++;
}

template <typename Type, int Ord>
Type BTree<Type, Ord>::getPredecessor(Node<Type, Ord>* node, int indx)
{
    Node<Type, Ord>* current=static_cast<InternalNode<Type, Ord>*>(node)->child[indx];
    while(!current->is_leaf)
        current=static_cast<InternalNode<Type, Ord>*>(current)->child[current->num_keys];
    return current->key[current->num_keys-1];
}

template <typename Type, int Ord>
Type BTree<Type, Ord>::getSuccessor(Node<Type, Ord>* node, int indx)
{
    Node<Type, Ord>* current=static_cast<InternalNode<Type, Ord>*>(node)->child[indx+1];
    while(!current->is_leaf)
        current=static_cast<InternalNode<Type, Ord>*>(current)->child[0];
    return current->key[0];
}

template <typename Type, int Ord>
void BTree<Type, Ord>::fill(InternalNode<Type, Ord>* parent, int idx)
{
    if(idx>0)
    {
        if(parent->child[idx-1]->num_keys>minKeys())
        {
            borrowLeft(parent, idx-1);
            return;
        }
    }
    if(idx<parent->num_keys)
    {
        if(parent->child[idx+1]->num_keys>minKeys())
        {
            borrowRight(parent, idx);
            return;
        }
        merge(parent, idx);
        return;
    }
    merge(parent, idx-1);
}

template <typename Type, int Ord>
void BTree<Type, Ord>::removeRecursive(Node<Type, Ord>* node, const Type& key)
{
    int idx=0;
    while(idx<node->num_keys)
    {
        if(node->key[idx]>=key) break;
        idx++;
    }
    if(idx<node->num_keys)
    {
        if(node->key[idx]==key)
        {
            if(node->is_leaf)
                removeFromLeaf(node, idx);
            else
                removeFromNonLeaf(static_cast<InternalNode<Type, Ord>*>(node), idx);
            return;
        }
    }
    if(node->is_leaf)
    {
        cout<<"The key "<<key<<" is not present in the tree\n";
        return;
    }
    InternalNode<Type, Ord>* internal=static_cast<InternalNode<Type, Ord>*>(node);
    removeRecursive(internal->child[idx], key);
    if(internal->child[idx]->num_keys<minKeys())
        fill(internal, idx);
}

template <typename Type, int Ord>
void BTree<Type, Ord>::remove(Type key)
{
    if(root==nullptr)
    {
        cout<<"The tree is empty\n";
        return;
    }
    removeRecursive(root, key);
    if(root->num_keys==0)
    {
        Node<Type, Ord>* old=root;
        if(root->is_leaf)
            root=nullptr;
        else
            root=static_cast<InternalNode<Type, Ord>*>(root)->child[0];
        deleteNode(old);
    }
}

template <typename Type, int Ord>
void BTree<Type, Ord>::display()
{
    if(root==nullptr)
    {
        cout<<"The tree is empty\n";
        return;
    }
    displayRecursive(root);
    cout<<endl;
}

template <typename Type, int Ord>
void BTree<Type, Ord>::displayRecursive(Node<Type, Ord>* node)
{
    if(node->is_leaf)
    {
        for(int i=0; i<node->num_keys; i++)
            cout<<node->key[i]<<" ";
        return;
    }
    InternalNode<Type, Ord>* internal=static_cast<InternalNode<Type, Ord>*>(node);
    for(int i=0; i<node->num_keys; i++)
    {
        displayRecursive(internal->child[i]);
        cout<<node->key[i]<<" ";
    }
    displayRecursive(internal->child[node->num_keys]);
}