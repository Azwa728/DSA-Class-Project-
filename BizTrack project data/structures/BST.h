#pragma once
// ---------------------------------------------------------------
// BST<K, V>  -  Binary Search Tree mapping a key to a value.
// Used for: indexing customers by ID (BST<int, Customer>) and for the
// Lab demo (BST<int, int>).
//
// Rule: every key in the LEFT subtree < node key < every key in the
// RIGHT subtree. Search / insert / delete are O(h), h = tree height
// (about log2 n for random insertion order, n for sorted insertion).
//
// RECURSION lives here: insert, remove, height and all the
// traversals are written recursively.
// ---------------------------------------------------------------
#include <iostream>
#include <string>
#include "structures/DynamicArray.h"

template <typename K, typename V>
class BST {
    struct Node {
        K key;
        V value;
        Node* left;
        Node* right;
        Node(const K& k, const V& v) : key(k), value(v), left(nullptr), right(nullptr) {}
    };

    Node* root;
    int count;

    // ---- recursive helpers ----
    Node* insertRec(Node* n, const K& k, const V& v, bool& inserted) {
        if (!n) { inserted = true; return new Node(k, v); }
        if (k < n->key)      n->left  = insertRec(n->left, k, v, inserted);
        else if (n->key < k) n->right = insertRec(n->right, k, v, inserted);
        else                 inserted = false;            // duplicate key
        return n;
    }

    Node* removeRec(Node* n, const K& k, bool& removed) {
        if (!n) return nullptr;
        if (k < n->key)      n->left  = removeRec(n->left, k, removed);
        else if (n->key < k) n->right = removeRec(n->right, k, removed);
        else {
            removed = true;
            if (!n->left)  { Node* r = n->right; delete n; return r; }   // 0 or 1 child
            if (!n->right) { Node* l = n->left;  delete n; return l; }
            Node* succ = n->right;                                       // 2 children:
            while (succ->left) succ = succ->left;                        // in-order successor
            K succKey = succ->key;
            n->key = succ->key;
            n->value = succ->value;
            bool dummy = false;
            n->right = removeRec(n->right, succKey, dummy);
        }
        return n;
    }

    int heightRec(Node* n) const {
        if (!n) return 0;
        int l = heightRec(n->left), r = heightRec(n->right);
        return 1 + (l > r ? l : r);
    }

    template <typename F> void inorderRec(Node* n, F& f) const {
        if (!n) return;
        inorderRec(n->left, f);
        f(n->key, n->value);
        inorderRec(n->right, f);
    }
    template <typename F> void preorderRec(Node* n, F& f) const {
        if (!n) return;
        f(n->key, n->value);
        preorderRec(n->left, f);
        preorderRec(n->right, f);
    }
    template <typename F> void postorderRec(Node* n, F& f) const {
        if (!n) return;
        postorderRec(n->left, f);
        postorderRec(n->right, f);
        f(n->key, n->value);
    }

    void traceRec(Node* n, int depth) const {
        std::string pad(depth * 3, ' ');
        if (!n) { std::cout << pad << "inorder(null)  -> return\n"; return; }
        std::cout << pad << "inorder(" << n->key << ")  -> go left\n";
        traceRec(n->left, depth + 1);
        std::cout << pad << "VISIT " << n->key << "\n";
        std::cout << pad << "inorder(" << n->key << ")  -> go right\n";
        traceRec(n->right, depth + 1);
    }

    void sidewaysRec(Node* n, int depth) const {
        if (!n) return;
        sidewaysRec(n->right, depth + 1);
        std::cout << std::string(depth * 6, ' ') << n->key << "\n";
        sidewaysRec(n->left, depth + 1);
    }

    void destroy(Node* n) {
        if (!n) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

public:
    BST() : root(nullptr), count(0) {}
    ~BST() { destroy(root); }
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    bool insert(const K& k, const V& v) {
        bool inserted = false;
        root = insertRec(root, k, v, inserted);
        if (inserted) count++;
        return inserted;
    }

    bool remove(const K& k) {
        bool removed = false;
        root = removeRec(root, k, removed);
        if (removed) count--;
        return removed;
    }

    // Returns a pointer to the stored value, or nullptr if the key is absent.
    // 'path' (optional) receives every key visited on the way down.
    V* find(const K& k, DynamicArray<K>* path = nullptr) const {
        Node* cur = root;
        while (cur) {
            if (path) path->push_back(cur->key);
            if (k < cur->key)      cur = cur->left;
            else if (cur->key < k) cur = cur->right;
            else return &cur->value;
        }
        return nullptr;
    }

    // f(key, value) is called once per node, in the traversal order.
    template <typename F> void inorder(F f) const   { inorderRec(root, f); }
    template <typename F> void preorder(F f) const  { preorderRec(root, f); }
    template <typename F> void postorder(F f) const { postorderRec(root, f); }

    int height() const { return heightRec(root); }
    int size() const { return count; }
    bool isEmpty() const { return count == 0; }

    void traceInorder() const { traceRec(root, 0); }
    void printSideways() const { sidewaysRec(root, 0); }

    void clear() { destroy(root); root = nullptr; count = 0; }
};
