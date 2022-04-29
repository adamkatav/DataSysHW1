#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <memory>
#include <functional>
#include <stdexcept>

template <class Key, class Value, template <class> class Ptr>
class AVLNode
{
public:
    Key key;
    Ptr<Value> value;
    int height;
    std::shared_ptr<AVLNode<Key, Value, Ptr>> left, right;
    std::weak_ptr<AVLNode<Key, Value, Ptr>> parent;

    AVLNode<Key, Value, Ptr>(Key key, Ptr<Value> value,
                             std::shared_ptr<AVLNode<Key, Value, Ptr>> left, std::shared_ptr<AVLNode<Key, Value, Ptr>> right) : key(key), value(value), height(1), left(left), right(right) {}

    int getBalanceFactor()
    {
        return getHeight(left) - getHeight(right);
    }
    static int getHeight(std::weak_ptr<AVLNode<Key, Value, Ptr>> node)
    {
        if (node.lock() == nullptr)
            return 0;
        int hleft = node.lock()->left != nullptr ? node.lock()->left->height : 0;
        int hright = node.lock()->right != nullptr ? node.lock()->right->height : 0;
        return (hleft > hright ? hleft : hright) + 1;
    }

    static std::shared_ptr<AVLNode<Key, Value, Ptr>> createAVLNode(Key key, Ptr<Value> value,
                                 std::shared_ptr<AVLNode<Key, Value, Ptr>> left, std::shared_ptr<AVLNode<Key, Value, Ptr>> right,
                                 std::weak_ptr<AVLNode<Key, Value, Ptr>> parent)
    {
        auto temp = std::make_shared(new AVLNode(key, value, left, right));
        temp->parent = std::weak_ptr<AVLNode<Key, Value, Ptr>>(temp);
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
        auto y = x.lock()->right;
        auto T2 = y->left;
        y->parent = x.lock()->parent;
        if(x.lock()->key == root->key){
            root = y;
        }

        y->left = x.lock();
        x.lock()->parent = y;
        x.lock()->right = T2;
        if(T2 != nullptr){
            T2->parent = x.lock();
        }

        x.lock()->height = AVLNode<Key, Value, Ptr>::getHeight(x.lock()->left) > AVLNode<Key, Value, Ptr>::getHeight(x.lock()->right) + 1 ? AVLNode<Key, Value, Ptr>::getHeight(x.lock()->left) : AVLNode<Key, Value, Ptr>::getHeight(x.lock()->right) + 1;
        y->height = AVLNode<Key, Value, Ptr>::getHeight(y->left) > AVLNode<Key, Value, Ptr>::getHeight(y->right) + 1 ? AVLNode<Key, Value, Ptr>::getHeight(y->left) : AVLNode<Key, Value, Ptr>::getHeight(y->right) + 1;

        return y;
    }
    std::weak_ptr<AVLNode<Key, Value, Ptr>> rotateRight(std::weak_ptr<AVLNode<Key, Value, Ptr>> x)
    {
        auto y = x.lock()->left;
        auto T2 = y->right;
        y->parent = x.lock()->parent;
        if(x.lock()->key == root->key){
            root = y;
        }
        y->right = x.lock();
        x.lock()->parent = y;
        x.lock()->left = T2;
        if(T2 != nullptr){
            T2->parent = x;
        }

        x.lock()->height = AVLNode<Key, Value, Ptr>::getHeight(x.lock()->left) > AVLNode<Key, Value, Ptr>::getHeight(x.lock()->right) + 1 ? AVLNode<Key, Value, Ptr>::getHeight(x.lock()->left) : AVLNode<Key, Value, Ptr>::getHeight(x.lock()->right) + 1;
        y->height = AVLNode<Key, Value, Ptr>::getHeight(y->left) > AVLNode<Key, Value, Ptr>::getHeight(y->right) + 1 ? AVLNode<Key, Value, Ptr>::getHeight(y->left) :AVLNode<Key, Value, Ptr>:: getHeight(y->right) + 1;

        return y;
    }
    std::weak_ptr<AVLNode<Key, Value, Ptr>> rotate(std::weak_ptr<AVLNode<Key, Value, Ptr>> parent)
    {
        if (parent.lock()->getBalanceFactor() == 2)
        {
            if (parent.lock()->left->getBalanceFactor() >= 0)
            { // LL
                return rotateRight(parent);
            }
            else if (parent.lock()->left->getBalanceFactor() == -1)
            { // LR
                parent.lock()->left = rotateLeft(parent.lock()->left).lock();
                return rotateRight(parent.lock()->left);
            }
        }
        else if (parent.lock()->getBalanceFactor() == -2)
        {
            if (parent.lock()->right->getBalanceFactor() <= 0)
            { // RR
                return rotateLeft(parent);
            }
            else if (parent.lock()->right->getBalanceFactor() == 1)
            { // RL
                parent.lock()->right = rotateRight(parent.lock()->right).lock();
                return rotateLeft(parent);
            }
        }
        if (parent.lock()->parent.lock()->key != parent.lock()->key)
            rotate(parent.lock()->parent);
        return parent;
    }

    int max(int a, int b)
    {
        return a > b ? a : b;
    }

    int add_t(Key key, Ptr<Value> value, std::weak_ptr<AVLNode<Key, Value, Ptr>> root)
    {
        if (key < root.lock()->key)
        {
            if (root.lock()->left == nullptr)
            {
                root.lock()->left = AVLNode<Key, Value, Ptr>::createAVLNode(key, value, std::shared_ptr<AVLNode<Key, Value, Ptr>>(nullptr), std::shared_ptr<AVLNode<Key, Value, Ptr>>(nullptr), root); // Should pass weak_ptr
                return AVLNode<Key, Value, Ptr>::getHeight(root);
            }
            add_t(key, value, root.lock()->left);
            root.lock()->height = max(AVLNode<Key, Value, Ptr>::getHeight(root.lock()->left), AVLNode<Key, Value, Ptr>::getHeight(root.lock()->right)) + 1;
            rotate(root);
        }
        else
        {
            if (root.lock()->right == nullptr)
            {
                root.lock()->right = AVLNode<Key, Value, Ptr>::createAVLNode(key, value, nullptr, nullptr, root);
                return AVLNode<Key, Value, Ptr>::getHeight(root);
            }
            add_t(key, value, root.lock()->right);
            root.lock()->height = max(AVLNode<Key, Value, Ptr>::getHeight(root.lock()->left), getHeight(root.lock()->right)) + 1;
            rotate(root);
        }
        return AVLNode<Key, Value, Ptr>::getHeight(root);
    }
    
    int sortedArrayOfThisAndAnotherTree(std::unique_ptr<AVLNode<Key, Value, Ptr>[]>& out_nodeArray, std::weak_ptr<AVLTree<Key, Value, Ptr>> other_tree){
        int total_size = size + other_tree.lock()->size;
        std::shared_ptr<AVLNode<Key, Value, Ptr>[]> this_flatten = flattenTree();
        std::shared_ptr<AVLNode<Key, Value, Ptr>[]> other_flatten = other_tree.lock()->flattenTree();
        int other_flatten_size = other_tree.lock()->size;
        int this_flatten_size = size;
        int this_it = 0;
        int other_it = 0;
        int i = 0;
        while (this_it < this_flatten_size && other_it < other_flatten_size)
        {
            if ( this_flatten[this_it]->key < other_flatten[other_it]->key)
            {
                *out_nodeArray[i] = *(this_flatten)[this_it];
                i++;
                this_it++;
            }
            else
            {
                
                *out_nodeArray[i] = *(other_flatten)[other_it];
                i++;
                other_it++;
            }
        }
        while ( other_it==other_flatten_size && this_it < this_flatten_size)
        {
            *out_nodeArray[i] = *(this_flatten)[this_it];
            i++;
            this_it++;
        }
        while ( this_it==this_flatten_size && other_it < other_flatten_size )
        {
            *out_nodeArray[i] = *(other_flatten)[other_it];
            i++;
            other_it++;
        }
        return total_size;
    }

    void flattenTree_t(std::weak_ptr<AVLNode<Key, Value, Ptr>[]> nodeArray, int& i, std::weak_ptr<AVLNode<Key, Value, Ptr>> root, int max_num){
        if(root.lock() == nullptr){
            return;
        }
        if (i == max_num)
        {
            return;
        }
        flattenTree_t(nodeArray,i,root.lock()->left,max_num);
        *(nodeArray)[i] = *root;
        i++;
        flattenTree_t(nodeArray,i,root.lock()->right,max_num);
    }

    void addAVLTree_t(std::unique_ptr<AVLNode<Key, Value, Ptr>[]>& node_arr, int s, int e, AVLNode<Key, Value, Ptr> &new_node, AVLNode<Key, Value, Ptr> parent){
        if(s > e){
            return;
        }
        int m = (s + e)/2;
        new_node = std::make_shared(createAVLNode(*(node_arr)[m]->key, *(node_arr)[m]->value, nullptr, nullptr, parent));
        addAVLTree_t(node_arr, s, m-1, new_node->left, new_node);
        addAVLTree_t(node_arr, m+1, e, new_node->right, new_node);
    }
    std::shared_ptr<AVLNode<Key, Value, Ptr>[]> flattenTree(){
        std::shared_ptr<AVLNode<Key, Value, Ptr>[]> nodeArray = std::make_shared(new AVLNode<Key, Value, Ptr>[size]);
        flattenTree_t(nodeArray,0,root,size);
        return nodeArray;
    }
public:
    int size;
    AVLTree<Key, Value, Ptr>() : root(nullptr), size(0) {}
    void add(Key key, Ptr<Value> value)
    {
        if (isEmpty()){
            root = AVLNode<Key, Value, Ptr>::createAVLNode(key, value, nullptr, nullptr, root);
            root->parent = root;
        }
        if (getValue(key).lock() != nullptr){
            throw std::runtime_error("Key already exist");
        }
        size++;
        add_t(key, value, root);
    }
    int remove(Key key){
        auto root = getNode(key).lock();
        if(root == nullptr){
            return -1;
        }
        auto new_root = getMin_t(root->right);
        auto new_root_old_parent = root->parent;
        if(new_root.lock() == nullptr){
            if(root->parent.lock()->key < root->key){
                root->parent.lock()->right = root->left;
            }
            else{
                root->parent.lock()->left = root->left;
            }
        }
        else{
            
            new_root_old_parent = new_root.lock()->parent;
            //update root's parent
            if(root->parent.lock()->key < root->key){
                root->parent.lock()->left = new_root.lock();
            }
            else{
                root->parent.lock()->right = new_root.lock();
            }

            //update new_root left and right
            new_root.lock()->left = root->left;
            root->left->parent = new_root;


            //removing new_root from its parent
            new_root.lock()->parent.lock()->left = new_root.lock()->right;

            //Update new_root right
            if(root->right->key != new_root.lock()->key){
                new_root.lock()->right = root->right;
            }
        }
        if(root->key == root->parent.lock()->key){
            new_root.lock()->parent = new_root;
        }
        while (new_root_old_parent.lock()->key != new_root_old_parent.lock()->parent.lock()->key){
            auto new_root_old_parent_parent = new_root_old_parent.lock()->parent;
            rotate(new_root_old_parent);
            new_root_old_parent = new_root_old_parent_parent;
        }
        size--;
        return 0;
    }
    void addAVLTree(std::weak_ptr<AVLTree<Key, Value, Ptr>> other_tree){
        AVLNode<Key, Value, std::weak_ptr> a;
        std::unique_ptr<AVLNode<Key, Value, Ptr>[]> nodeArray(new AVLNode<Key, Value, Ptr>[size + other_tree.lock()->size]);
        int nodeArray_size = sortedArrayOfThisAndAnotherTree(nodeArray, other_tree);
        addAVLTree_t(nodeArray, 0, nodeArray_size-1, root, root);
        size += other_tree.lock()->size;
    }

    std::weak_ptr<AVLNode<Key, Value, Ptr>> getMin_t(std::shared_ptr<AVLNode<Key, Value, Ptr>> root){
        while(root->left != nullptr){
            root = root->left;
        }
        return root;
    }

    std::weak_ptr<AVLTree<Key, Value, Ptr>> getMin(){
        return getMin_t(root);
    }

    std::weak_ptr<Value> getMax_t(std::shared_ptr<AVLNode<Key, Value, Ptr>> root){
        auto temp = root;
        while(temp->right != nullptr){
            temp = temp->right;
        }
        return temp;
    }
    
    std::weak_ptr<Value> getMax(){
        return getMax_t(root);
    }

std::shared_ptr<Key[]> flattenKeysArray(){
    auto nodeArray = flattenTree();
    std::shared_ptr<Key[]> keyArray = std::make_shared(new Key[size]);
    for(int i = 0; i < size; i++){
        *(keyArray)[i] = *(nodeArray)[i];
    }
    return keyArray;
}

std::shared_ptr<Value[]> flattenvaluesArray(){
    auto nodeArray = flattenTree();
    std::shared_ptr<Value[]> valueArray = std::make_shared(new Value[size]);
    for(int i = 0; i < size; i++){
        *(valueArray)[i] = *(nodeArray)[i];
    }
    return valueArray;
}

    std::weak_ptr<Value> getValue_t(Key key, std::weak_ptr<AVLNode<Key, Value, Ptr>> root){
        auto p = root.lock();
        if(p == nullptr)
            return std::weak_ptr<Value>();
        if(p->key == key)
            return std::weak_ptr<Value>(p->value);
        if(p->key < key)
            return getValue_t(key, p->right);
        return getValue_t(key, p->left);
    }

    std::weak_ptr<Value> getValue(Key key)
    {
        return getValue_t(key, root);
    }

    std::weak_ptr<AVLNode<Key, Value, Ptr>> getNode_t(Key key, std::weak_ptr<AVLNode<Key, Value, Ptr>> root){
        auto p = root.lock();
        if(p == nullptr)
            return std::weak_ptr<AVLNode<Key, Value, Ptr>>();
        if(p->key == key)
            return p;
        if(p->key < key)
            return getNode_t(key, p->right);
        return getNode_t(key, p->left);
    }

    std::weak_ptr<AVLNode<Key, Value, Ptr>> getNode(Key key)
    {
        return getNode_t(key, root);
    }

    bool isEmpty()
    {
        return root == nullptr;
    }

void clear(){
    root = std::make_shared<std::shared_ptr<AVLNode<Key, Value, Ptr>>>(nullptr);
    size = 0;
}

};
#endif