# Songdle

A Wordle-style song guessing game. The player has 8 guesses to find a mystery song from
real Spotify songs. After each guess the game says whether the artist matches, whether the
mystery song's year, tempo and popularity are higher or lower, and how many songs still match.
Filtering is done with two structures written from scratch, a **B+ tree** and a
**red-black tree**, and their speeds are compared.

## Team: Debugging Since Always
| Member | GitHub | Main work |
|---|---|---|
| Mila Bond | bondmila | B+ tree and its tests |
| Natalie Pascut | natp0 | Red-black tree |
| Catalina Cardenas | catacardenasu | Data cleaning, game logic, menu, benchmark |

Report and video: done by all members. Video demo: [link](https://youtu.be/IRcZro-Sjko)

## Requirements
- A C++17 compiler (g++ 9+ or clang++ 10+)
- SFML (graphics library used for the game window). On macOS: `brew install sfml`
- Python 3 (optional: only for re-cleaning the data and graphing benchmark results)

## Setup
1. Clone the repo:
   ```
   git clone https://github.com/catacardenasu/songdle-project2.git
   cd songdle-project2
   ```
2. The cleaned dataset is already included at `data/songs_clean.tsv` (tab-separated),
   so no download is needed to run the game.

### (Optional) Regenerating the cleaned data
`songs_clean.tsv` was produced from the original Kaggle file
[Spotify Dataset 1921-2020, 600k+ Tracks](https://www.kaggle.com/datasets/yamaerenay/spotify-dataset-19212020-600k-tracks)
using `scripts/clean_data.py`. To redo it, download the Kaggle file, put it in `data/`,
install the Python requirements (`pip install -r scripts/req`) and run:
```
python3 scripts/clean_data.py
```
(`scripts/explore_data.py` is a helper for inspecting the dataset.)

## Build
The C++ source is in the `src/` folder. From the repo root:
```
cd src
g++ -std=c++17 -O2 -Wall *.cpp -o songdle -I$(brew --prefix sfml)/include -L$(brew --prefix sfml)/lib -lsfml-graphics -lsfml-window -lsfml-system
cd ..
```
(macOS with Homebrew. On Linux, install SFML with your package manager and use
`g++ -std=c++17 -O2 -Wall *.cpp -o songdle -lsfml-graphics -lsfml-window -lsfml-system`.)

## Run
Run from the repo root so the program can find `data/songs_clean.tsv`:
```
./src/songdle
```
Menu options:
- **Play:** guess songs; after each guess you get artist match, higher/lower for year, tempo
  and popularity, the number of songs still matching, and the filtering time of each tree.
-  **Run benchmark:** plays many random games automatically and shows the average filtering
  time of the B+ tree and the red-black tree.
- **Quit:** exits the program.

## Project layout
All C++ source files are in `src/`.
| File | Purpose |
|---|---|
| `src/main.cpp` | Program entry point |
| `Game.h` / `Game.cpp` | Game logic and clue feedback |
| `GameUI.h` / `GameUI.cpp` | Menu and graph interface (SFML) |
| `Benchmark.h` / `Benchmark.cpp` | Benchmark mode (B+ tree vs red-black tree) |
| `DataLoader.h` / `DataLoader.cpp` | Loads `data/songs_clean.tsv` |
| `Song.h` | Song record (name, artist, year, popularity, tempo) |
| `TreeBase.h` | Shared interface for both trees |
| `BPlusTree.h` | B+ tree index (Mila) |
| `RedBlackTree.h` / `RedBlackTree.cpp` | Red-black tree index (Natalie) |
| `data/songs_clean.tsv` | Cleaned Spotify dataset |
| `scripts/` | `clean_data.py` (cleaning), `explore_data.py`, `req` (Python requirements) |

## Notes
- Both trees expose `insert(key, songIndex)`, `search(key)`, `rangeSearch(lo, hi)` (inclusive)
  and `memoryBytes()`, so they are interchangeable in the game.
- The full dataset has 500,000+ songs, so loading takes a few seconds.