#pragma once
#include <vector>
#include <algorithm>

// ORDER = max number of keys a node can hold before it splits.
template <typename K, int ORDER = 64>
class BPlusTree {
    struct Node { // One struct is used for both leaf and internal nodes.

        bool isLeaf;
        std::vector<K> keys;
        std::vector<Node*> children;
        std::vector<std::vector<int>>postings; // Leaf only: postings[i] holds the song indices for keys[i].

        Node* next = nullptr; // Leaf only: link to the next leaf, used by rangeSearch.
        Node(bool leaf) : isLeaf(leaf) {}
    };

    Node* root = nullptr;
    int nodeCount = 0;
    int numEntries = 0;

    
    // which child to go down into; a key equal to a signpost goes right.
    int childIndex(Node* n,const K& key) const {
        return std::upper_bound(n->keys.begin(), n->keys.end(), key)-n->keys.begin();
    }
     // walk down from the root to the leaf where key belong
    Node* findLeaf(const K& key) const {
        Node* n = root;


        while (n != nullptr && !n->isLeaf)
            n = n->children[childIndex(n, key)];
        return n;

    }
    // insert into this subtree
    Node* insertRec(Node* node,const K& key, int idx , K& upKey) {
        if (node->isLeaf) {
            int pos= std::lower_bound(node->keys.begin(), node->keys.end(), key)
                      - node->keys.begin();

            if (pos <(int)node->keys.size() && node->keys[pos]==key) {  //just add the song index to its list
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


            // leaf split;copy the first key of the right leaf up
            upKey =right->keys[0];
            return right;
        }

        int ci =childIndex(node, key);
        K childUp;
        Node* newChild = insertRec(node->children[ci], key, idx,childUp);
        if (newChild ==nullptr) return nullptr;

        node->keys.insert(node->keys.begin() + ci,childUp); // child split; add its key and new node to this node.
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
    // free every node (children first)
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

    // add (key -> song index).
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

    // all song indices with exactyl this key
    std::vector<int> search(const K& key) const {
        Node* leaf = findLeaf(key);
        if (leaf ==nullptr) return {};
        int pos= std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key)
                  - leaf->keys.begin();
        if (pos < (int)leaf->keys.size() &&leaf->keys[pos] == key)
            return leaf->postings[pos];
        return {};
    }

    
    //go to the leaf with lo, then walk along the linked leaves until a key > hi.
    std::vector<int> rangeSearch(const K& lo, const K& hi) const {
        std::vector<int> result;
        Node* leaf =findLeaf(lo);
        if (leaf == nullptr) return result;
        int i = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), lo)
                - leaf->keys.begin();
        while (leaf != nullptr) {
            for (; i < (int)leaf->keys.size(); i++) {
                if (leaf->keys[i] > hi)return result;
                result.insert(result.end(),leaf->postings[i].begin(),
                              leaf->postings[i].end());
            }
            leaf = leaf->next;
            i = 0;
        }
        return result;
    }
    //rough memory estimate
    size_t memoryBytes() const {
        return nodeCount * sizeof(Node) + numEntries * (sizeof(int) + sizeof(K));
    }

   

    
};