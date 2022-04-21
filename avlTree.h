#ifndef AVLTREE_H_
#define AVLTREE_H_

template<class Key, class Value, template<class> class Ptr>
class AVLNode{
    public:
    Key key;
    Value value;
    int balance_factor;
    Ptr<AVLNode<Key,Value,Ptr>> left, right, parent;
    AVLNode<Key, Value, Ptr>(Key key, Value value, int balance_factor,
     Ptr<AVLNode<Key,Value,Ptr>> left, right, parent):key(key), value(value),
     balance_factor(balance_factor),
     left(left), right(right), parent(parent){}

};


template<class Key, class Value, class Ptr>
class AVLTree
{
private:
    
public:
AVLTree<Key, Value, Ptr>(/* args */)
{
}


};
#endif