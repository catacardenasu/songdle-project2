#include "BPlusTree.h"
#include <iostream>
#include <random>
#include <cstdlib>

// ORDER must be known at compile time, so it is a template parameter
template <int ORDER>
void runTest() {
    std::mt19937 rng(42);
    BPlusTree<int,ORDER>  tree;
    std::vector<int> keys;

    // 20000 songs but only 500 possible keys, so there are many duplicates.
    for (int i = 0; i < 20000; i++) {
        int k = rng() % 500;

        keys.push_back(k);
        tree.insert(k, i);
    }

    // compare 1000 random range queries against a brute-force loop.
    for (int t = 0; t < 1000; t++) {
        int lo = rng() % 500;


        int hi = lo + rng() % 100;

        std::vector<int> expected;
        for (int i= 0; i < (int)keys.size();i++)
            if (keys[i] >= lo && keys[i]<= hi) expected.push_back(i);

        std::vector<int> got =tree.rangeSearch(lo, hi);

        std::sort(got.begin(),got.end());

        if (got != expected) {
            std::cout << "FAILED rangeSearch at ORDER "<< ORDER << "\n";
            std::exit(1);
        }
    }

    // check exact search for every key
    for (int k = 0; k < 500; k++) {
        std::vector<int> expected;
        for (int i = 0; i <(int)keys.size();i++)
            if (keys[i]== k) expected.push_back(i);
        if (tree.search(k) !=expected) {
            std::cout << "FAILED search at ORDER " <<ORDER<< "\n";

            std::exit(1);
        }
    }

    std::cout << "ORDER " << ORDER << " passed\n";
}

int main() { //testing
    // small order forces many splits
    runTest<4>();
    runTest<16>();
    runTest<64>();

    // doubles are used for tempo
    BPlusTree<double,4> t;
    t.insert(120.5, 0);
    t.insert(99.0, 1);
    
    t.insert(120.5, 2);
    t.insert(150.25, 3); // 


    std::vector<int> r =t.rangeSearch(100.0, 130.0);
    if (r.size() != 2 || r[0] != 0 | r[1] != 2) {
        std::cout << "FAILED double test\n";
        return 1;
    }
    std::cout << "double test passed\n";

    std::cout << "all tests passed\n";
}