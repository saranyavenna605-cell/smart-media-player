# 🎵 Smart Media Player - Data Structures Project in Plain C

A terminal-based **Smart Media Player** written in **pure, beginner-friendly C** (`gcc`, standard library only). 
Designed specifically for a college **Data Structures and Algorithms (DSA)** coursework presentation and viva voce examination.

Every function is short (~20 lines max), clearly commented in plain English, and avoids complex macros, function pointers, or obscure C tricks.

---

## 📋 Table of Contents
1. [Core Features & DSA Mapping](#-core-features--dsa-mapping)
2. [File-by-File Plain English Explanation](#-file-by-file-plain-english-explanation)
3. [How to Compile and Run](#-how-to-compile-and-run)
4. [How to Run Unit Tests](#-how-to-run-unit-tests)
5. [Terminal Menu Options (0–17)](#-terminal-menu-options-017)
6. [The Algorithmic Twist: Smart Shuffle](#-the-algorithmic-twist-smart-shuffle)
7. [Viva Voce & Presentation Cheat Sheet](#-viva-voce--presentation-cheat-sheet)

---

## 🚀 Core Features & DSA Mapping

| Feature | Data Structure / Algorithm | File | Complexity |
| :--- | :--- | :--- | :--- |
| **Playlist (Next & Prev)** | **Doubly Linked List (DLL)** | `data_structures.c` | $O(1)$ step |
| **Repeat-All Mode** | **Circular Doubly Linked List (CLL)** | `data_structures.c` | $O(1)$ loop wrap |
| **"Up Next" Priority Queue** | **Queue (FIFO)** | `data_structures.c` | $O(1)$ enqueue/dequeue |
| **Play History & Back Button** | **Stack (LIFO)** | `data_structures.c` | $O(1)$ push/pop |
| **Library Catalog & Search** | **Binary Search Tree (BST)** | `data_structures.c` | $O(\log n)$ search / $O(n)$ in-order |
| **Multi-field Sorting** | **QuickSort & MergeSort** | `algorithms.c` | $O(n \log n)$ |
| **Exact Title Lookup** | **Binary Search** | `algorithms.c` | $O(\log n)$ |
| **Cooldown Randomizer** | **Smart Shuffle Engine** | `algorithms.c` | $O(N)$ window verification |

---

## 📂 File-by-File Plain English Explanation

### 1. `models.h`
- Defines the `Song` structure: `id`, `title[50]`, `artist[50]`, and `duration` (in seconds).
- Defines the node and header structures for:
  - `DLLNode` and `DLL` (Doubly Linked List)
  - `CLLNode` and `CLL` (Circular Doubly Linked List)
  - `QueueNode` and `Queue` (FIFO Queue)
  - `StackNode` and `Stack` (LIFO Stack)
  - `BSTNode` (Binary Search Tree)
- Declares the `Player` engine state structure and all function prototypes across the project.

### 2. `data_structures.c`
Contains the implementation of all 5 data structures built completely from scratch using standard pointers and `malloc`/`free`:
- **Doubly Linked List (DLL):** Maintains `head`, `tail`, `prev`, and `next` pointers. Used for linear playlist navigation (Next/Previous).
- **Circular Doubly Linked List (CLL):** The `head->prev` points to `tail` and `tail->next` points to `head`. Used for infinite Repeat-All looping.
- **Queue (FIFO):** Singly linked list with `front` and `rear` pointers. Song added at rear (`enqueue`) and played first from front (`dequeue`).
- **Stack (LIFO):** Singly linked list with `top` pointer. When a song plays, it is pushed to top. Pressing "Back" pops the stack to return to the previous track.
- **Binary Search Tree (BST):** Nodes organized alphabetically by song title (`left` < `root` < `right`). In-order traversal (`Left -> Root -> Right`) automatically prints songs in A-Z order.

### 3. `algorithms.c`
Implements core sorting, searching, and randomization algorithms:
- **QuickSort:** Recursive divide-and-conquer using Lomuto partitioning. Can sort library array by Title, Artist, or Duration.
- **MergeSort:** Stable divide-and-conquer algorithm with $O(n \log n)$ guaranteed time complexity.
- **Binary Search:** Searches a sorted array for a song title in $O(\log n)$ time by repeatedly halving the search space.
- **Smart Shuffle:** Solves the playlist repetition problem using a cooldown window of size $N=4$ tracks cross-referenced against the History Stack.

### 4. `player.c`
The central player engine that connects user commands to the data structures:
- Manages priority routing: if songs exist in the **Up Next Queue**, they are played before continuing the regular playlist.
- Handles Repeat-All toggling, history tracking on every play event, and freeing all allocated memory.

### 5. `main.c`
The user-facing terminal application:
- Provides a clean, numbered menu (Options 0 to 17).
- Contains input validation to avoid infinite loops on invalid input.
- Includes **Option 17 (LIVE DEMO)**: an automated presentation run that showcases every single data structure and algorithm with explanatory output.

### 6. `test_suite.c`
A standalone automated test file using standard C `assert()`:
- 8 individual test suites verifying DLL, CLL, Queue, Stack, BST, QuickSort, MergeSort, Binary Search, and Smart Shuffle.

### 7. `smart_media_player.c` (Standalone Single-File Version)
Contains the **entire pure C project in one self-contained file** (all structs, DLL, CLL, Queue, Stack, BST, QuickSort, MergeSort, Binary Search, Smart Shuffle, menu, and Live Demo).
- Perfect for single-file submission requirements or online C compilers.

### 8. `Makefile`
- `make`: Compiles modular files (`main.c`, `player.c`, `algorithms.c`, `data_structures.c`) into `./smart_media_player`.
- `make single`: Compiles the standalone single-file `smart_media_player.c` directly.
- `make test`: Builds and runs the automated unit tests `./test_suite`.
- `make clean`: Removes all compiled binaries and object files.

### 9. `PROJECT_REPORT.md`
A complete academic project report covering problem background, architecture, algorithm trace, complexity tables, and 20+ viva questions.

---

## 🛠️ How to Compile and Run

### Option A: Using `make` (Recommended)
```bash
# Build the smart media player executable
make

# Run the program (Linux / macOS)
./smart_media_player

# Run the program (Windows Command Prompt / PowerShell)
.\smart_media_player.exe
```

### Option B: Direct GCC Compilation (One-liner)
If `make` is not installed on your system, compile directly with GCC:
```bash
gcc -Wall -Wextra -std=c99 -o smart_media_player main.c player.c algorithms.c data_structures.c
.\smart_media_player.exe
```

---

## 🧪 How to Run Unit Tests

### Option A: Using `make test`
```bash
make test
```

### Option B: Direct Compilation
```bash
gcc -Wall -Wextra -std=c99 -o test_suite test_suite.c algorithms.c data_structures.c
.\test_suite.exe
```

**Expected Output:**
```
=======================================================
   RUNNING SMART MEDIA PLAYER DATA STRUCTURE TEST SUITE
=======================================================

  [PASS] Doubly Linked List (DLL) tests passed!
  [PASS] Circular Doubly Linked List (CLL) tests passed!
  [PASS] Queue (FIFO - Up Next) tests passed!
  [PASS] Stack (LIFO - History) tests passed!
  [PASS] Binary Search Tree (BST) tests passed!
  [PASS] QuickSort & MergeSort tests passed!
  [PASS] Binary Search (O(log n)) tests passed!
  [PASS] Smart Shuffle Cooldown Algorithm tests passed!

-------------------------------------------------------
  ALL 8 TEST SUITES COMPLETED SUCCESSFULLY! (8/8 PASS)
=======================================================
```

---

## 🔊 Live Audio Output Through Laptop Speakers

The Smart Media Player features a built-in hardware audio engine with zero external dependencies:
1. **Acoustic Melody Synthesizer:** When any song is chosen, the player generates and plays the recognizable signature melody of that song live through your laptop speakers using standard Windows PCM audio (`winmm`).
2. **Real MP3/WAV Support:** You can drop any real audio file into the `music/` folder (e.g. `music/1.mp3`, `music/2.mp3`, or `music/Blinding Lights.mp3`), and the player will stream the actual full song using Windows MCI!
3. **Real-Time Visualizer:** Displays live sound waveform indicators while songs play.

---

## 🎮 Terminal Menu & Direct Song Selection

```
================================================================================
                  SMART MEDIA PLAYER (Pure C Data Structures)                   
================================================================================
  -------------------------------------------------------------
  [<< PREV TRACK]: (#1) 'Bohemian Rhapsody' by Queen
  [>>> NOW PLAYING]: (#2) 'Blinding Lights' by The Weeknd (03:20)
  [>> NEXT TRACK]: (#3) 'Hotel California' by Eagles
  -------------------------------------------------------------

  [CURRENT PLAYLIST (Doubly Linked List)]:
   [ 1] Bohemian Rhapsody         | Queen              | 05:54
  >[ 2] Blinding Lights           | The Weeknd         | 03:20  <-- [PLAYING NOW]
   [ 3] Hotel California          | Eagles             | 06:31
   [ 4] Shape of You              | Ed Sheeran         | 03:53
   [ 5] Billie Jean               | Michael Jackson    | 04:54
   [ 6] Starboy                   | The Weeknd         | 03:50
   [ 7] Smells Like Teen Spirit   | Nirvana            | 05:01
   [ 8] Stay                      | The Kid LAROI      | 02:21
   [ 9] As It Was                 | Harry Styles       | 02:47
   [10] Rolling in the Deep       | Adele              | 03:48
  [Repeat-All: OFF | Play History: 2 songs | Up Next Queue: 0 songs]
--------------------------------------------------------------------------------
  1. Add Song to Playlist         10. Toggle Repeat-All (CLL)
  2. Remove Song from Playlist    11. Search Title in BST
  3. Refresh Playlist View        12. Display A-Z Catalog (BST)
  4. Play / Select Song by ID     13. QuickSort Library
  5. Next Song (Queue / DLL)      14. MergeSort Library
  6. Previous Song (DLL)          15. Binary Search (O(log n))
  7. Back Button (Stack - LIFO)   16. Smart Shuffle (Cooldown 4)
  8. Add to Up Next Queue (FIFO)  17. RUN FULL LIVE DEMO
  9. Play from Up Next Queue       0. Exit
================================================================================
Select option (0-17) or type song number (e.g. #1 to #10) to play: 
```

- **Direct Song Selection:** You can directly type `#1` through `#10` (or `play 2`) from the main prompt to immediately play that track through your speakers!
- **Option 17 (LIVE DEMO):** Designed for lab evaluation and viva presentations! Selecting `17` automatically runs an end-to-end walkthrough of all 8 components without needing manual input.

---

## 🔀 The Algorithmic Twist: Smart Shuffle

A common flaw with naive random shuffle (`rand() % N`) is that tracks often repeat shortly after being played, ruining the user experience.

The **Smart Shuffle Engine** solves this with a **3-step Sliding Cooldown Window** ($N = 4$):
1. **Window Extraction:** Reads the last $N$ songs from the top of the **History Stack** without popping them.
2. **Eligibility Filtering:** Iterates through the song library array and filters out any song whose ID appears in the cooldown window.
3. **Random Selection:** Chooses uniformly at random (`rand()`) **only from the eligible pool outside the window**.
4. **Window Relaxation (Deadlock Prevention):** If the total song pool has fewer than $N+1$ songs, the window size dynamically shrinks to `poolSize - 1` so the player never gets stuck.

---

## 🎓 Viva Voce & Presentation Cheat Sheet

1. **Why use a Doubly Linked List for a playlist instead of an array?**
   - *Answer:* Inserting or deleting a song in an array requires shifting elements ($O(n)$). In a Doubly Linked List, insertion and deletion are $O(1)$ pointer reassignments once the node is reached. Moving Next and Previous is also an instant $O(1)$ step.

2. **What is the difference between DLL and CLL?**
   - *Answer:* In a DLL, `head->prev = NULL` and `tail->next = NULL`. In a CLL (Circular Doubly Linked List), `head->prev = tail` and `tail->next = head`. This makes implementing Repeat-All natural without index bounds checks.

3. **Why is FIFO Queue used for "Up Next"?**
   - *Answer:* Users expect songs they queue first to play first (First-In, First-Out).

4. **Why is LIFO Stack used for the "Back" button?**
   - *Answer:* The most recently played track must be returned to first (Last-In, First-Out).

5. **Why use In-order Traversal on the BST for the catalogue?**
   - *Answer:* In a Binary Search Tree, an in-order traversal (`Left -> Root -> Right`) visits keys in strictly ascending alphabetical order, producing a sorted A-Z catalog in $O(n)$ time without needing a separate sorting step.

6. **What is the difference between QuickSort and MergeSort?**
   - *Answer:* QuickSort is an in-place sorting algorithm with average $O(n \log n)$ time but worst case $O(n^2)$. MergeSort has a guaranteed $O(n \log n)$ time and is **stable** (preserves the relative order of duplicate elements), but requires auxiliary temporary memory.
