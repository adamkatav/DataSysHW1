#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <memory>
#include <functional>
#include <exception>

template <class Key, class Value, template <class> class Ptr>
class AVLNode
{
public:
    Key key;
    Ptr<Value> value;
    int height;
    std::shared_ptr<AVLNode<Key, Value, Ptr>> left, right;
    std::weak_ptr<AVLNode<Key, Value, Ptr>> parent;

    AVLNode<Key, Value, Ptr>(Key key, Value value,
                             std::shared_ptr<AVLNode<Key, Value, Ptr>> left, std::shared_ptr<AVLNode<Key, Value, Ptr>> right) : key(key), value(Ptr<Value>(value)), height(1), left(left), right(right) {}

    int getBalanceFactor()
    {
        return getHeight(left) - getHeight(right);
    }
    static int getHeight(std::weak_ptr<AVLNode<Key, Value, Ptr>> node)
    {
        if (node == nullptr)
            return 0;
        int hleft = node->left != nullptr ? node->left->height : 0;
        int hright = node->right != nullptr ? node->right->height : 0;
        return (hleft > hright ? hleft : hright) + 1;
    }

    static AVLNode createAVLNode(Key key, Value &value,
                                 std::shared_ptr<AVLNode<Key, Value, Ptr>> left, std::shared_ptr<AVLNode<Key, Value, Ptr>> right,
                                 std::weak_ptr<AVLNode<Key, Value, Ptr>> parent)
    {
        AVLNode temp = AVLNode(key, value, left, right);
        temp.parent = std::weak_ptr<AVLNode<Key, Value, Ptr>>(temp);
        return temp;
    }
};

template <class Key, class Value, template <class> class Ptr>
class AVLTree
{
private:
    std::shared_ptr<AVLNode<Key, Value, Ptr>> root;
    std::weak_ptr<AVLNode<Key, Value, Ptr>> rotateLeft(std::weak_ptr<AVLNode<Key, Value, Ptr>> x)
    {
        auto y = x->right;
        auto T2 = y->left;
        y->parent = x->parent;
        if(x->key == root->key){
            root = y;
        }

        y->left = x;
        x->parent = y;
        x->right = T2;
        if(T2 != nullptr){
            T2->parent = x;
        }

        x->height = height(x->left) > height(x->right) + 1 ? height(x->left) : height(x->right) + 1;
        y->height = height(y->left) > height(y->right) + 1 ? height(y->left) : height(y->right) + 1;

        return y;
    }
    std::weak_ptr<AVLNode<Key, Value, Ptr>> rotateRight(std::weak_ptr<AVLNode<Key, Value, Ptr>> x)
    {
        auto y = x->left;
        auto T2 = y->right;
        y->parent = x->parent;
        if(x->key == root->key){
            root = y;
        }
        y->right = x;
        x->parent = y;
        x->left = T2;
        if(T2 != nullptr){
            T2->parent = x;
        }

        x->height = height(x->left) > height(x->right) + 1 ? height(x->left) : height(x->right) + 1;
        y->height = height(y->left) > height(y->right) + 1 ? height(y->left) : height(y->right) + 1;

        return y;
    }
    bool rotate(std::weak_ptr<AVLNode<Key, Value, Ptr>> parent)
    {
        if (parent->getBalanceFactor() == 2)
        {
            if (parent->left->getBalanceFactor() >= 0)
            { // LL
                return rotateRight(parent);
            }
            else if (parent->left->getBalanceFactor() == -1)
            { // LR
                parent->left = rotateLeft(parent->left);
                return rotateRight(parent->left);
            }
        }
        else if (parent->getBalanceFactor() == -2)
        {
            if (parent->right->getBalanceFactor() <= 0)
            { // RR
                return rotateLeft(parent);
            }
            else if (parent->right->getBalanceFactor() == 1)
            { // RL
                parent->right = rotateRight(parent->right);
                return rotateLeft(parent);
            }
        }
        if (parent->parent->key != parent->key)
            rotate(parent->parent);
        return false;
    }

    int max(int a, int b)
    {
        return a > b ? a : b;
    }

    int add_t(Key key, Ptr<Value> value_ptr, std::weak_ptr<AVLNode<Key, Value, Ptr>> root)
    {
        if (key < root->key)
        {
            if (root->left == nullptr)
            {
                root->left = std::shared_ptr<AVLNode<Key, Value, Ptr>>(createAVLNode(key, value_ptr, nullptr, nullptr, root)); // Should pass weak_ptr
                return getHeight(root);
            }
            add_t(key, value_ptr, root->left);
            root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
            rotate(root);
        }
        else
        {
            if (root->right == nullptr)
            {
                root->right = std::shared_ptr<AVLNode<Key, Value, Ptr>>(createAVLNode(key, value_ptr, nullptr, nullptr, root)); // Should pass weak_ptr
                return getHeight(root);
            }
            add_t(key, value_ptr, root->right);
            root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
            rotate(root);
        }
        return getHeight(root);
    }
public:
    AVLTree<Key, Value, Ptr>() : root(nullptr) {}
    void add(Key key, Ptr<Value> value_ptr)
    {
        if (getValue(key) != nullptr)
            throw std::runtime_error("Key already exist");
        if (isEmpty())
            root = std::shared_ptr<AVLNode<Key, Value, Ptr>>(createAVLNode(key, value_ptr, nullptr, nullptr, root));
        add_t(key, value_ptr, root);
    }
    int remove(Key key){
            auto root = getValue(key);
            if(root == nullptr){
                return -1;
            }
            auto new_root = getMin(root->right);
            auto new_root_old_parent = root->parent;
            if(new_root == nullptr){
                if(root->parent->key < root->key){
                    root->parent->right = root->left;
                }
                else{
                    root->parent->left = root->left;
                }
            }
            else{
                
                new_root_old_parent = new_root->parent;
                //update root's parent
                if(root->parent->key < root->key){
                    root->parent->left = new_root;
                }
                else{
                    root->parent->right = new_root;
                }

                //update new_root left and right
                new_root->left = root->left;
                root->left->parent = new_root;


                //removing new_root from its parent
                new_root->parent->left = new_root->right;

                //Update new_root right
                if(root->right->key != new_root ->key){
                    new_root->right = root->right;
                }
            }
            if(root->key == root->parent->key){
                new_root->parent = new_root;
            }
            while (new_root_old_parent->key != new_root_old_parent->parent->key){
                auto new_root_old_parent_parent = new_root_old_parent->parent;
                rotate(new_root_old_parent);
                new_root_old_parent = new_root_old_parent_parent;
            }
        return 0;
    }
    void addAVLTree(std::weak_ptr<AVLTree<Key, Value, Ptr>> other_tree);
    std::weak_ptr<AVLTree<Key, Value, Ptr>> getMin(std::shared_ptr<AVLTree<Key, Value, Ptr>> root){
        while(root->left != nullptr){
            root = root->left;
        }
        return root;
    }

    std::weak_ptr<AVLTree<Key, Value, Ptr>> getMax(){
        auto temp = root;
        while(temp->right != nullptr){
            temp = temp->right;
        }
        return temp;
    }


    std::weak_ptr<Value> getValue(Key key)
    {
        std::weak_ptr<AVLNode<Key, Value, Ptr>> node = root;
        if (node == nullptr)
        {
            return nullptr;
        }
        if (node->key == key)
        {
            return std::weak_ptr<Value>(node->value);
        }
        if (node->key < key)
        {
            return getValue(node->right);
        }
        return getValue(node->left);
    }
    template <typename... Args>
    void inOrder(std::function<void(Args...)> const &func, Args... as);
    bool isEmpty()
    {
        return root == nullptr;
    }
};
#endif