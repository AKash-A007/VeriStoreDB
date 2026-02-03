#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <functional>

namespace vsdb {

template<typename Key, typename Value>
class BTreeNode {
public:
    std::vector<Key> keys;
    std::vector<Value> values;
    std::vector<std::shared_ptr<BTreeNode>> children;
    bool is_leaf;
    
    BTreeNode(bool leaf = true) : is_leaf(leaf) {}
    
    size_t size() const { return keys.size(); }
};

template<typename Key, typename Value>
class BTree {
public:
    BTree(int order = 3) : order_(order), root_(nullptr) {}
    
    void insert(const Key& key, const Value& value);
    std::optional<Value> search(const Key& key) const;
    bool remove(const Key& key);
    void traverse(std::function<void(const Key&, const Value&)> callback) const;
    
    bool empty() const { return root_ == nullptr; }
    
private:
    int order_; // Maximum number of children
    std::shared_ptr<BTreeNode<Key, Value>> root_;
    
    void insert_non_full(std::shared_ptr<BTreeNode<Key, Value>> node, const Key& key, const Value& value);
    void split_child(std::shared_ptr<BTreeNode<Key, Value>> parent, int index);
    std::optional<Value> search_node(std::shared_ptr<BTreeNode<Key, Value>> node, const Key& key) const;
    void traverse_node(std::shared_ptr<BTreeNode<Key, Value>> node, std::function<void(const Key&, const Value&)> callback) const;
};

// Implementation
template<typename Key, typename Value>
void BTree<Key, Value>::insert(const Key& key, const Value& value) {
    if (!root_) {
        root_ = std::make_shared<BTreeNode<Key, Value>>(true);
        root_->keys.push_back(key);
        root_->values.push_back(value);
        return;
    }
    
    if (root_->size() == 2 * order_ - 1) {
        auto new_root = std::make_shared<BTreeNode<Key, Value>>(false);
        new_root->children.push_back(root_);
        split_child(new_root, 0);
        root_ = new_root;
    }
    
    insert_non_full(root_, key, value);
}

template<typename Key, typename Value>
void BTree<Key, Value>::insert_non_full(std::shared_ptr<BTreeNode<Key, Value>> node, const Key& key, const Value& value) {
    int i = node->size() - 1;
    
    if (node->is_leaf) {
        node->keys.push_back(Key());
        node->values.push_back(Value());
        
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            node->values[i + 1] = node->values[i];
            i--;
        }
        
        node->keys[i + 1] = key;
        node->values[i + 1] = value;
    } else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        
        if (node->children[i]->size() == 2 * order_ - 1) {
            split_child(node, i);
            if (key > node->keys[i]) {
                i++;
            }
        }
        
        insert_non_full(node->children[i], key, value);
    }
}

template<typename Key, typename Value>
void BTree<Key, Value>::split_child(std::shared_ptr<BTreeNode<Key, Value>> parent, int index) {
    auto full_child = parent->children[index];
    auto new_child = std::make_shared<BTreeNode<Key, Value>>(full_child->is_leaf);
    
    int mid = order_ - 1;
    
    // Move second half of keys/values to new child
    for (int i = mid + 1; i < full_child->size(); i++) {
        new_child->keys.push_back(full_child->keys[i]);
        new_child->values.push_back(full_child->values[i]);
    }
    
    if (!full_child->is_leaf) {
        for (int i = mid + 1; i <= full_child->children.size() - 1; i++) {
            new_child->children.push_back(full_child->children[i]);
        }
        full_child->children.resize(mid + 1);
    }
    
    // Move middle key up to parent
    parent->keys.insert(parent->keys.begin() + index, full_child->keys[mid]);
    parent->values.insert(parent->values.begin() + index, full_child->values[mid]);
    parent->children.insert(parent->children.begin() + index + 1, new_child);
    
    // Shrink full_child
    full_child->keys.resize(mid);
    full_child->values.resize(mid);
}

template<typename Key, typename Value>
std::optional<Value> BTree<Key, Value>::search(const Key& key) const {
    if (!root_) return std::nullopt;
    return search_node(root_, key);
}

template<typename Key, typename Value>
std::optional<Value> BTree<Key, Value>::search_node(std::shared_ptr<BTreeNode<Key, Value>> node, const Key& key) const {
    int i = 0;
    while (i < node->size() && key > node->keys[i]) {
        i++;
    }
    
    if (i < node->size() && key == node->keys[i]) {
        return node->values[i];
    }
    
    if (node->is_leaf) {
        return std::nullopt;
    }
    
    return search_node(node->children[i], key);
}

template<typename Key, typename Value>
void BTree<Key, Value>::traverse(std::function<void(const Key&, const Value&)> callback) const {
    if (root_) {
        traverse_node(root_, callback);
    }
}

template<typename Key, typename Value>
void BTree<Key, Value>::traverse_node(std::shared_ptr<BTreeNode<Key, Value>> node, std::function<void(const Key&, const Value&)> callback) const {
    int i;
    for (i = 0; i < node->size(); i++) {
        if (!node->is_leaf) {
            traverse_node(node->children[i], callback);
        }
        callback(node->keys[i], node->values[i]);
    }
    
    if (!node->is_leaf) {
        traverse_node(node->children[i], callback);
    }
}

template<typename Key, typename Value>
bool BTree<Key, Value>::remove(const Key& key) {
    // Simplified: not implemented in basic version
    return false;
}

} // namespace vsdb