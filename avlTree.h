#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <memory>
#include <functional>

template<class Key, class Value, template<class> class Ptr>
class AVLNode{
    public:
    Key key;
    Value value;
    int balance_factor;
    Ptr<AVLNode<Key,Value,Ptr>> left, right, parent;
    AVLNode<Key, Value, Ptr>(Key key, Value value, int balance_factor,
     Ptr<AVLNode<Key,Value,Ptr>> left, Ptr<AVLNode<Key,Value,Ptr>> right,
     Ptr<AVLNode<Key,Value,Ptr>> parent):key(key), value(value),
     balance_factor(balance_factor),
     left(left), right(right), parent(parent){}

};


template<class Key, class Value, template<class> class Ptr>
class AVLTree
{
private:
Ptr<AVLNode<Key,Value,Ptr>> root;
    
public:
AVLTree<Key, Value, Ptr>() : root(nullptr){}
void add(Key key, Ptr<Value> value_ptr);
void remove(Key key);
void addAVLTree(std::weak_ptr<AVLTree<Key,Value,Ptr>> other_tree);
Value getValue(Key key);
template<typename ... Args>
void inOrder(std::function<void(Args...)> const & func, Args ... as);


};
#endif