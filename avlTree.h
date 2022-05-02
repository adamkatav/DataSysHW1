#ifndef AVLTREE_H_
#define AVLTREE_H_

#include <memory>
#include <functional>
#include <stdexcept>
#include <iostream>

template <class Key, class Value, template <class> class Ptr>
class AVLNode
{
public:
    Key key;
    std::shared_ptr<Value> value;
    int height;
    std::shared_ptr<AVLNode<Key, Value, Ptr>> left, right;
    std::weak_ptr<AVLNode<Key, Value, Ptr>> parent;

    AVLNode<Key, Value, Ptr>(Key key, std::shared_ptr<Value> value,
                             std::shared_ptr<AVLNode<Key, Value, Ptr>> left, std::shared_ptr<AVLNode<Key, Value, Ptr>> right) : key(key), value(value), height(1), left(left), right(right) {}

    AVLNode<Key, Value, Ptr>(){};

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

    static std::shared_ptr<AVLNode<Key, Value, Ptr>> createAVLNode(Key key, std::shared_ptr<Value> value,
                                 std::shared_ptr<AVLNode<Key, Value, Ptr>> left, std::shared_ptr<AVLNode<Key, Value, Ptr>> right,
                                 std::shared_ptr<AVLNode<Key, Value, Ptr>> parent)
    {
        std::shared_ptr<AVLNode<Key, Value, Ptr>> temp = std::make_shared<AVLNode<Key, Value, Ptr>>();
        temp->key = key;
        temp->value = value;
        temp->left = left;
        temp->right = right;
        temp->height = 1;
        if(parent == nullptr)
            temp->parent = std::weak_ptr<AVLNode<Key, Value, Ptr>>(temp);
        else
            temp->parent = parent;

        return temp;
    }
};

template <class Key, class Value, template <class> class Ptr>
class AVLTree
{
private:
    std::shared_ptr<AVLNode<Key, Value, Ptr>> root;
    std::shared_ptr<AVLNode<Key, Value, Ptr>> rotateLeft(std::shared_ptr<AVLNode<Key, Value, Ptr>> x)
    {
        auto y = x->right;
        auto T2 = y->left;
        y->parent = x->parent;
        if(x->key == root->key){
            root = y;
            root->parent = y;
        }
        else{
            if(x->parent.lock()->right != nullptr && x->parent.lock()->right->key == x->key){
                x->parent.lock()->right = y;
            }
            else{
                x->parent.lock()->left = y;
            }
        }
        y->left = x;
        x->parent = y;
        x->right = T2;
        if(T2 != nullptr){
            T2->parent = x;
        }

        x->height = AVLNode<Key, Value, Ptr>::getHeight(x->left) > AVLNode<Key, Value, Ptr>::getHeight(x->right)  ? AVLNode<Key, Value, Ptr>::getHeight(x->left) + 1 : AVLNode<Key, Value, Ptr>::getHeight(x->right) + 1;
        y->height = AVLNode<Key, Value, Ptr>::getHeight(y->left) > AVLNode<Key, Value, Ptr>::getHeight(y->right)  ? AVLNode<Key, Value, Ptr>::getHeight(y->left) + 1 : AVLNode<Key, Value, Ptr>::getHeight(y->right) + 1;

        return y;
    }
    std::shared_ptr<AVLNode<Key, Value, Ptr>> rotateRight(std::shared_ptr<AVLNode<Key, Value, Ptr>> x)
    {
        auto y = x->left;
        auto T2 = y->right;
        y->parent = x->parent;
        if(x->key == root->key){
            root = y;
            root->parent = y;
        }
        else{
            if(x->parent.lock()->right != nullptr && x->parent.lock()->right->key == x->key){
                x->parent.lock()->right = y;
            }
            else{
                x->parent.lock()->left = y;
            }
        }
        y->right = x;
        x->parent = y;
        x->left = T2;
        if(T2 != nullptr){
            T2->parent = x;
        }

        x->height = AVLNode<Key, Value, Ptr>::getHeight(x->left) > AVLNode<Key, Value, Ptr>::getHeight(x->right) ? AVLNode<Key, Value, Ptr>::getHeight(x->left) + 1 : AVLNode<Key, Value, Ptr>::getHeight(x->right) + 1;
        y->height = AVLNode<Key, Value, Ptr>::getHeight(y->left) > AVLNode<Key, Value, Ptr>::getHeight(y->right) ? AVLNode<Key, Value, Ptr>::getHeight(y->left) + 1 :AVLNode<Key, Value, Ptr>:: getHeight(y->right) + 1;

        return y;
    }
    std::weak_ptr<AVLNode<Key, Value, Ptr>> rotate(std::weak_ptr<AVLNode<Key, Value, Ptr>> parent)
    {
        if (parent.lock()->getBalanceFactor() == 2)
        {
            if (parent.lock()->left->getBalanceFactor() >= 0)
            { // LL
                return rotateRight(parent.lock());
            }
            else if (parent.lock()->left->getBalanceFactor() == -1)
            { // LR
                parent.lock()->left = rotateLeft(parent.lock()->left);
                return rotateRight(parent.lock()->left);
            }
        }
        else if (parent.lock()->getBalanceFactor() == -2)
        {
            if (parent.lock()->right->getBalanceFactor() <= 0)
            { // RR
                return rotateLeft(parent.lock());
            }
            else if (parent.lock()->right->getBalanceFactor() == 1)
            { // RL
                auto REMOVE_ON_SIGHT = rotateRight(parent.lock()->right);
                parent.lock()->right = REMOVE_ON_SIGHT;
                return rotateLeft(parent.lock());
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

    int add_t(Key key, std::shared_ptr<Value> value, std::weak_ptr<AVLNode<Key, Value, Ptr>> root)
    {
        if (key < root.lock()->key)
        {
            if (root.lock()->left == nullptr)
            {
                root.lock()->left = AVLNode<Key, Value, Ptr>::createAVLNode(key, value, std::shared_ptr<AVLNode<Key, Value, Ptr>>(nullptr), std::shared_ptr<AVLNode<Key, Value, Ptr>>(nullptr), root.lock()); // Should pass weak_ptr
                root.lock()->height = max(AVLNode<Key, Value, Ptr>::getHeight(root.lock()->left), AVLNode<Key, Value, Ptr>::getHeight(root.lock()->right)) + 1;
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
                root.lock()->right = AVLNode<Key, Value, Ptr>::createAVLNode(key, value, nullptr, nullptr, root.lock());
                root.lock()->height = max(AVLNode<Key, Value, Ptr>::getHeight(root.lock()->left), AVLNode<Key, Value, Ptr>::getHeight(root.lock()->right)) + 1;
                return AVLNode<Key, Value, Ptr>::getHeight(root);
            }
            add_t(key, value, root.lock()->right);
            root.lock()->height = max(AVLNode<Key, Value, Ptr>::getHeight(root.lock()->left), AVLNode<Key, Value, Ptr>::getHeight(root.lock()->right)) + 1;
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
            if ( this_flatten[this_it].key < other_flatten[other_it].key)
            {
                out_nodeArray[i] = (this_flatten)[this_it];
                i++;
                this_it++;
            }
            else
            {
                
                out_nodeArray[i] = (other_flatten)[other_it];
                i++;
                other_it++;
            }
        }
        while ( other_it==other_flatten_size && this_it < this_flatten_size)
        {
            out_nodeArray[i] = (this_flatten)[this_it];
            i++;
            this_it++;
        }
        while ( this_it==this_flatten_size && other_it < other_flatten_size )
        {
            out_nodeArray[i] = (other_flatten)[other_it];
            i++;
            other_it++;
        }
        return total_size;
    }

    void flattenTree_t(AVLNode<Key, Value, Ptr>* nodeArray, int& i, std::weak_ptr<AVLNode<Key, Value, Ptr>> root, int max_num, Key min, Key max){
        if(root.lock() == nullptr){
            return;
        }
        if (i == max_num)
        {
            return;
        }
        if (root.lock()->key < min)
        {
            flattenTree_t(nodeArray,i,root.lock()->right,max_num, min, max);
            return;
        }
        if (max < root.lock()->key)
        {
            flattenTree_t(nodeArray,i,root.lock()->left,max_num, min, max);
            return;
        }

        flattenTree_t(nodeArray,i,root.lock()->left,max_num, min, max);
        nodeArray[i] = *(root.lock());
        i++;
        flattenTree_t(nodeArray,i,root.lock()->right,max_num, min, max);
    }

    void addAVLTree_t(std::unique_ptr<AVLNode<Key, Value, Ptr>[]>& node_arr, int s, int e, std::shared_ptr<AVLNode<Key, Value, Ptr>> new_node, std::shared_ptr<AVLNode<Key, Value, Ptr>> parent){
        if(s > e){
            return;
        }
        int m = (s + e)/2;
        new_node = AVLNode<Key, Value, Ptr>::createAVLNode((node_arr)[m].key, (node_arr)[m].value, nullptr, nullptr, parent);
        addAVLTree_t(node_arr, s, m-1, new_node->left, new_node);
        addAVLTree_t(node_arr, m+1, e, new_node->right, new_node);
    }
    std::unique_ptr<AVLNode<Key, Value, Ptr>[]> flattenTree(){
        std::unique_ptr<AVLNode<Key, Value, Ptr>[]> nodeArray(new AVLNode<Key, Value, Ptr>[size]);
        int count = 0;
        flattenTree_t(nodeArray.get(),count,root,size,getMin_t(root).lock()->key, getMax_t(root).lock()->key);

        return nodeArray;
    }
public:
    int size;
    AVLTree<Key, Value, Ptr>() : root(nullptr), size(0) {}
    void add(Key key, std::shared_ptr<Value> value)
    {
        auto REMOVE_ON_SIGHT = getValue(key).lock();
        if (REMOVE_ON_SIGHT != nullptr){
            throw std::runtime_error("Key already exist");
        }
        if (isEmpty()){
            root = AVLNode<Key, Value, Ptr>::createAVLNode(key, value, nullptr, nullptr, root);
            root->parent = root;
        }
        else{
            add_t(key, value, root);
        }
        size++;
    }
    int remove(Key key){
        auto old_root = getNode(key).lock();
        if(old_root == nullptr){
            throw std::runtime_error("key doesn't exist in remove");
        }
        auto new_root = getMin_t(old_root->right);
        auto new_root_old_parent = old_root->parent;
        if(new_root.lock() == nullptr){
            if(old_root->parent.lock()->key < old_root->key){
                old_root->parent.lock()->right = old_root->left;
            }
            else{
                old_root->parent.lock()->left = old_root->left;
            }
        }
        else{
            
            new_root_old_parent = new_root.lock()->parent;
            if(old_root->right->key == new_root.lock()->key)
            {
                //if the new root is the old root's right son
                //we will not use new_root_old_parent in this case, only in the end when updating heights
                new_root_old_parent = new_root.lock();
            }
            
            //update root's parent
            new_root.lock()->parent = old_root->parent;
            if(old_root->parent.lock()->key < old_root->key){
                old_root->parent.lock()->right = new_root.lock();
            }
            else{
                old_root->parent.lock()->left = new_root.lock();
            }
            
            if(old_root->key == old_root->parent.lock()->key){
            this->root = new_root.lock();
            new_root.lock()->parent = new_root;
            }

            //removing new_root from its parent
            if(old_root->right->key != new_root.lock()->key)
            {
                new_root_old_parent.lock()->left = new_root.lock()->right;
            }
            
            //update new_root left and right
            new_root.lock()->left = old_root->left;
            if(old_root->left != nullptr)
                old_root->left->parent = new_root;

            //Update new_root right
            if(old_root->right->key != new_root.lock()->key){
                new_root.lock()->right = old_root->right;
                if(old_root->right == nullptr)
                    throw std::runtime_error("right is null in remove");
                old_root->right->parent = new_root;
            }
        }

        //update the heights in the מסלול הכנסה
        auto temp = new_root_old_parent.lock();
        while (temp->key != temp->parent.lock()->key){
            temp->height = max(AVLNode<Key, Value, Ptr>::getHeight(temp->left), AVLNode<Key, Value, Ptr>::getHeight(temp->right)) + 1;
            temp = temp->parent.lock();
        }
        //update root's height
        temp->height = max(AVLNode<Key, Value, Ptr>::getHeight(temp->left), AVLNode<Key, Value, Ptr>::getHeight(temp->right)) + 1;
        //rotate 
        while (new_root_old_parent.lock()->key != new_root_old_parent.lock()->parent.lock()->key){
            auto new_root_old_parent_parent = new_root_old_parent.lock()->parent;
            rotate(new_root_old_parent);
            new_root_old_parent = new_root_old_parent_parent;
        }
        size--;
        print();
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
        if(root == nullptr)
            return std::shared_ptr<AVLNode<Key, Value, Ptr>>();
        auto temp = root;
        //auto prev = temp;
        while(temp->left != nullptr){
        //    prev = temp;
            temp = temp->left;
        }
        return temp;
    }

    std::shared_ptr<Value> getMin(){
        if(isEmpty())
            return std::shared_ptr<Value>();
        return std::shared_ptr<Value>(getMax_t(root).lock()->value);
    }

    std::weak_ptr<AVLNode<Key, Value, Ptr>> getMax_t(std::shared_ptr<AVLNode<Key, Value, Ptr>> root){
        if(root == nullptr)
            return std::shared_ptr<AVLNode<Key, Value, Ptr>>();
        auto temp = root;
        //auto prev = temp;
        while(temp->right != nullptr){
        //    prev = temp;
            temp = temp->right;
        }
        return temp;
    }
    
    std::shared_ptr<Value> getMax(){
        //return std::make_shared<Value>(getMax_t(root).lock()->value);
        if(isEmpty())
            return std::shared_ptr<Value>();
        auto REMOVE_ON_SIGHT = getMax_t(root).lock()->value;

        return std::shared_ptr<Value>(REMOVE_ON_SIGHT);
    }

    std::unique_ptr<Key[]> flattenKeysArray(){
        auto nodeArray = flattenTree();
        std::unique_ptr<Key[]> keyArray = std::unique_ptr<Key[]>(new Key[size]);
        for(int i = 0; i < size; i++){
            (keyArray)[i] = (nodeArray)[i].key;
        }
        return keyArray;
    }



    std::unique_ptr<std::shared_ptr<Value>[]> flattenvaluesArray(int desired_size, Key min, Key max){
        auto nodeArray = std::unique_ptr<AVLNode<Key,Value,Ptr>[]>(new AVLNode<Key,Value,Ptr>[desired_size]);
        int count = 0;
        flattenTree_t(nodeArray.get(),count,root,desired_size,min,max);
        auto valueArray = std::unique_ptr<std::shared_ptr<Value>[]>(new std::shared_ptr<Value>[desired_size]);
        for(int i = 0; i < desired_size; i++){
            *(valueArray[i]) = *(std::shared_ptr<Value>(nodeArray[i].value));
        }
        return valueArray;
    }

    std::weak_ptr<Value> getValue_t(Key key, std::weak_ptr<AVLNode<Key, Value, Ptr>> root){
        auto p = root.lock();
        if(p == nullptr)
            return std::shared_ptr<Value>();
        if(p->key == key)
            return p->value;
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
    root = nullptr;
    size = 0;
}

void print_t(std::weak_ptr<AVLNode<Key,Value,Ptr>> node,int& counter){
    if(node.lock()==nullptr) return;
    print_t(node.lock()->left, counter);
    std::cout << node.lock()->key << std::endl; 
    counter++;
    print_t(node.lock()->right, counter);
}

void print(){
    int counter = 0;
    print_t(root,counter);
    std::cout << "size = " << counter << std::endl;
}


};
#endif