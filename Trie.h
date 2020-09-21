#pragma once

#pragma GCC optimize("-O3")

#include <unordered_map>
#include <vector>
#include <utility>
#include <iostream>

template <typename KeyType, typename ValueType>
class Trie {
 public:
    typedef typename KeyType::value_type NodeKeyType;
    class Node {
     public:
        ValueType *value = nullptr;
        std::unordered_map<NodeKeyType, Node*> nxt;
        Node() = default;
        explicit Node(ValueType val) {
            *value = std::move(val);
        }
        explicit Node(ValueType &&val) {
            *value = val;
        }
        ~Node() {
            if (value != nullptr) {
                delete value;
            }
            for (auto &[key, nxtNode] : nxt) {
                delete nxtNode;
            }
            nxt.clear();
        }
    };

    Trie() : rootNode(new Node()) {}

    Trie(Trie &other) = delete;

    Trie(Trie &&other) = delete;

    void Clear() {
        delete rootNode;
        rootNode = new Node();
    }

    ValueType &operator[](const KeyType &key) {
        Node *v = rootNode;
        for (auto &i : key) {
            auto &nv = v->nxt[i];
            if (nv == nullptr) {
                nv = new Node();
            }
            v = nv;
        }
        if (v->value == nullptr) {
            v->value = new ValueType();
        }
        return *v->value;
    }

    bool Contains(const KeyType &key) {
        Node *v = rootNode;
        for (NodeKeyType &i : key) {
            auto &nv = v->nxt[i];
            if (nv == nullptr) {
                return false;
            }
            v = nv;
        }
        return v->value != nullptr;
    }

    std::optional<ValueType*> GetIfContains(const KeyType &key) {
        Node *v = rootNode;
        for (auto &i : key) {
            auto &nv = v->nxt[i];
            if (nv == nullptr) {
                return std::nullopt;
            }
            v = nv;
        }
        if (v->value == nullptr) {
            return std::nullopt;
        }
        return {v->value};
    }

    std::vector<std::pair<KeyType, ValueType>> Collapse() {
        std::vector<std::pair<KeyType, ValueType>> ans;
        std::string prefix;
        CollapseDfs(ans, prefix, rootNode);
        return ans;
    }

    ~Trie() {
        delete rootNode;
    }
 private:
    Node *rootNode = nullptr;
    void CollapseDfs(std::vector<std::pair<KeyType, ValueType>> &ans, KeyType &prefix, Node *v) {
        if (v->value != nullptr) {
            ans.emplace_back(prefix, v->value);
        }
        for (auto &i : v->nxt) {
            prefix.push_back(i.first);
            CollapseDfs(ans, prefix, i.second);
            prefix.pop_back();
        }
    }
};