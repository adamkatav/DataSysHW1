#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <memory>
#include <functional>
#include <exception>

template<class Key, class Value, template<class> class Ptr>
class AVLNode{
    public:
    Key key;
    Ptr<Value> value;
    int height;
    std::shared_ptr<AVLNode<Key,Value,Ptr>> left, right;
    std::weak_ptr<AVLNode<Key,Value,Ptr>> parent;

    AVLNode<Key, Value, Ptr>(Key key, Value value,
        std::shared_ptr<AVLNode<Key,Value,Ptr>> left, std::shared_ptr<AVLNode<Key,Value,Ptr>> right):key(key), value(Ptr<Value>(value))
        ,height(1) , left(left), right(right){}

    int getBalanceFactor(){
        return getHeight(left) - getHeight(right);
     }
    static int getHeight(std::weak_ptr<AVLNode<Key,Value,Ptr>> node){
        if(node == nullptr) return 0;
        int hleft = node->left != nullptr ? node->left->height : 0;
        int hright = node->right != nullptr ? node->right->height : 0;
        return (hleft > hright ? hleft : hright) + 1;
    }

    static AVLNode createAVLNode(Key key, Value& value,
        std::shared_ptr<AVLNode<Key,Value,Ptr>> left, std::shared_ptr<AVLNode<Key,Value,Ptr>> right,
        std::weak_ptr<AVLNode<Key,Value,Ptr>> parent){
            AVLNode temp = AVLNode(key, value, left, right);
            temp.parent = std::weak_ptr<AVLNode<Key,Value,Ptr>>(temp);
            return temp;
        }
};


template<class Key, class Value, template<class> class Ptr>
class AVLTree
{
private:


std::shared_ptr<AVLNode<Key,Value,Ptr>> root;
bool rotateLeft(std::weak_ptr<AVLNode<Key,Value,Ptr>> x){
    auto y = x->right;
    auto T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = height(x->left) > height(x->right) + 1? height(x->left) : height(x->right) + 1;
    y->height = height(y->left) > height(y->right) + 1? height(y->left) : height(y->right) + 1;

    return true;
}
bool rotateRight(std::weak_ptr<AVLNode<Key,Value,Ptr>> x){
    auto y = x->left;
    auto T2 = y->right;

    y->right = x;
    x->left = T2;

    x->height = height(x->left) > height(x->right) + 1? height(x->left) : height(x->right) + 1;
    y->height = height(y->left) > height(y->right) + 1? height(y->left) : height(y->right) + 1;

    return true;
}
bool rotate(std::weak_ptr<AVLNode<Key,Value,Ptr>> parent){
    if(parent->getBalanceFactor() == 2){
        if(parent->left->getBalanceFactor() >= 0){ //LL
            return rotateRight(parent);
        }
        else if (parent->left->getBalanceFactor() == -1){//LR
            parent->left = rotateLeft(parent->left);
            return rotateRight(parent->left);
        }
    }
    else if (parent->getBalanceFactor() == -2){
        if(parent->right->getBalanceFactor() <= 0){ //RR
            return rotateLeft(parent);
        }
        else if (parent->right->getBalanceFactor() == 1){//RL
            parent->right = rotateRight(parent->right);
            return rotateLeft(parent);
        }
    }
    if(parent->parent->key != parent->key) rotate(parent->parent);
    return false;
}

int max(int a, int b){
    return a > b ? a : b;
}

int add_t(Key key, Ptr<Value> value_ptr, std::weak_ptr<AVLNode<Key,Value,Ptr>> root){
    if(key < root->key){
        if(root->left == nullptr){
            root->left = std::shared_ptr<AVLNode<Key,Value,Ptr>>(createAVLNode(key, value_ptr, nullptr, nullptr, root)); // Should pass weak_ptr
            return getHeight(root); 
        }
        root->height = max(add_t(key, value_ptr, root->left), getHeight(root->right)) + 1;
        rotate(root->parent);
    }
    else{
        if(root->right == nullptr){
            root->right = std::shared_ptr<AVLNode<Key,Value,Ptr>>(createAVLNode(key, value_ptr, nullptr, nullptr, root)); // Should pass weak_ptr
            return getHeight(root);
        }
        root->height = max(add_t(key, value_ptr, root->right), getHeight(root->left)) + 1;
        rotate(root->parent);
    }
    return getHeight(root);
}    

public:
AVLTree<Key, Value, Ptr>() : root(nullptr){}
void add(Key key, Ptr<Value> value_ptr){
    if(getValue(key) != nullptr) throw std::runtime_error("Key already exist");
    if(isEmpty()) root = std::shared_ptr<AVLNode<Key,Value,Ptr>>(createAVLNode(key, value_ptr, nullptr, nullptr, root));
    add_t(key, value_ptr, root);
}
void remove(Key key);
void addAVLTree(std::weak_ptr<AVLTree<Key,Value,Ptr>> other_tree);
std::weak_ptr<Value> getValue(Key key){
    std::weak_ptr<AVLNode<Key,Value,Ptr>> node = root;
    if(node == nullptr){
        return nullptr;
    }
    if (node->key == key){
        return std::weak_ptr<Value>(node->value);
    }
    if(node->key < key){
        return getValue(node->right);
    }
    return getValue(node->left);
}
template<typename ... Args>
void inOrder(std::function<void(Args...)> const & func, Args ... as);
bool isEmpty(){
    return root == nullptr;
}


};
#endif