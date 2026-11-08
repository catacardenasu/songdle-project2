#include "DataLoader.h"
#include "GameUI.h"
#include "Game.h"

#if __has_include("BPlusTree.h")
#include "BPlusTree.h"
#define HAVE_BPLUS 1
#endif
#if __has_include("RedBlackTree.h")
#include "RedBlackTree.h"
#define HAVE_RB 1
#endif

#include <iostream>

int main(int argc, char** argv) {
    const std::string path = argc > 1 ? argv[1] : "data/songs_clean.tsv";

    std::cout << "Loading " << path << "...\n";
    LoadStats stats;
    std::vector<Song> songs;
    try {
        songs = DataLoader::load(path, &stats);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    std::cout << "Loaded " << stats.loaded << " songs (" << stats.skipped
              << " rows skipped) in " << stats.time << " ms\n";
    if (songs.empty()) return 1;

    std::vector<CategorySets> sets;
#ifdef HAVE_BPLUS
    sets.push_back(buildCategorySet<BPlusTree>("B+ Tree", songs));
#endif
#ifdef HAVE_RB
    sets.push_back(buildCategorySet<RedBlackTree>("Red-black tree", songs));
#endif
    for (const CategorySets& s : sets)
        std::cout << "Built " << s.name << " in " << s.buildTime << " ms\n";

    try {
        GameUI ui(songs, sets);
        ui.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}