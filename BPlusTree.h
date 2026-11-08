#pragma once
#include <vector>
#include <algorithm>

template <typename K, int ORDER = 64>
class BPlusTree {
    struct Node {

        bool isLeaf;
        std::vector<K> keys;
        std::vector<Node*> children;
        std::vector<std::vector<int>>postings;

        Node* next = nullptr;
        Node(bool leaf) : isLeaf(leaf) {}
    };

    Node* root = nullptr;
    int nodeCount = 0;
    int numEntries = 0;

    int childIndex(Node* n,const K& key) const {
        return std::upper_bound(n->keys.begin(), n->keys.end(), key)-n->keys.begin();
    }

    Node* findLeaf(const K& key) const {
        Node* n = root;


        while (n != nullptr && !n->isLeaf)
            n = n->children[childIndex(n, key)];
        return n;

    }

    Node* insertRec(Node* node,const K& key, int idx , K& upKey) {
        if (node->isLeaf) {
            int pos= std::lower_bound(node->keys.begin(), node->keys.end(), key)
                      - node->keys.begin();

            if (pos <(int)node->keys.size() && node->keys[pos]==key) {
                node->postings[pos].push_back(idx);
                return nullptr;
            }

            node->keys.insert(node->keys.begin() + pos, key);

            node->postings.insert(node->postings.begin() + pos, std::vector<int>(1, idx));

            if ((int)node->keys.size()<=ORDER) return nullptr;

            int mid = node->keys.size() / 2;
            Node* right = new Node(true);
            nodeCount++;
            right->keys.assign(node->keys.begin() +mid, node->keys.end());
            right->postings.assign(node->postings.begin() + mid, node->postings.end()) ;
            node->keys.erase(node->keys.begin()+ mid, node->keys.end());

            node->postings.erase(node->postings.begin() + mid, node->postings.end());

            right->next =node->next;
            node->next =right;

            upKey =right->keys[0];
            return right;
        }

        int ci =childIndex(node, key);
        K childUp;
        Node* newChild = insertRec(node->children[ci], key, idx,childUp);
        if (newChild ==nullptr) return nullptr;

        node->keys.insert(node->keys.begin() + ci,childUp);
        node->children.insert(node->children.begin() +ci + 1,newChild);

        if ((int)node->keys.size()<=ORDER) return nullptr;

        int mid = node->keys.size()/2;
        upKey = node->keys[mid];
        Node* right =new Node(false);
        nodeCount++;
        right->keys.assign(node->keys.begin() + mid + 1, node->keys.end());

        right->children.assign(node->children.begin() + mid + 1, node->children.end());
        node->keys.erase(node->keys.begin() + mid, node->keys.end());

        node->children.erase(node->children.begin() + mid + 1, node->children.end());
        return right;
    }

    void destroy(Node* n) {
        if (n == nullptr) return;
        for (Node* c : n->children)  destroy(c);

        delete n;
    }

public:
    BPlusTree(){}
    BPlusTree(const BPlusTree&) =delete;
    BPlusTree& operator=(const BPlusTree&) = delete;
    ~BPlusTree() { destroy(root); }

    void insert(const K& key,  int idx) {
        if (root== nullptr ) {
            root =new Node(true);
            nodeCount++;
        }
        K upKey;
        Node* newRight =insertRec(root, key, idx, upKey);
        if (newRight !=nullptr) {
            Node* newRoot =new Node(false);
            nodeCount++;
            newRoot->keys.push_back(upKey);
            newRoot->children.push_back(root);
            newRoot->children.push_back(newRight);
            root =newRoot;
        }
        numEntries++;
    }

    
};