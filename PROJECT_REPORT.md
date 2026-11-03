# Academic Project Report: Smart Media Player

**Course:** Data Structures and Algorithms (DSA) Laboratory  
**Language:** Plain C (ISO C99, `gcc`, Standard Library Only)  
**Project Title:** Smart Media Player Engine with Sliding Cooldown Shuffle  

---

## 1. Introduction & Motivation

Digital audio streaming services such as Spotify and Apple Music rely on sophisticated data structures to maintain responsive user interfaces, manage dynamic queues, track navigation history, and provide intelligent track recommendation algorithms.

The objective of this project is to implement a **Smart Media Player** from scratch in plain C. By avoiding external frameworks, libraries, or generic high-level wrappers, this project demonstrates direct memory management using pointers (`malloc`/`free`) and implements textbook Data Structures and Algorithms to solve real-world media playback challenges.

### Core Problems Addressed:
1. **Dynamic Track Navigation:** Moving forward and backward across songs without expensive array shifts.
2. **Cyclic Looping:** Seamlessly repeating an entire playlist when playback reaches the end.
3. **Priority Playback:** Allowing users to queue priority tracks ("Up Next") that temporarily override linear playlist order.
4. **Playback Reversal:** Supporting an undo/back history feature that tracks previously played songs regardless of how they were selected.
5. **Fast Catalog Search:** Rapidly searching songs alphabetically and listing the entire library in A-Z order.
6. **Smart Randomization (The Twist):** Overcoming the common drawback of naive random shuffle, where tracks frequently repeat within short intervals.

---

## 2. System Architecture & Component Mapping

The system is decoupled into modular components:

```
+-------------------------------------------------------------------+
|                           main.c (UI)                             |
|          Interactive Terminal Menu & Option 17 Live Demo          |
+---------------------------------+---------------------------------+
                                  |
                                  v
+-------------------------------------------------------------------+
|                           player.c                                |
|        Central Player State, Dispatcher & Priority Router         |
+-------------------+-------------------------------+---------------+
                    |                               |
                    v                               v
+-----------------------------------+   +---------------------------+
|        data_structures.c          |   |       algorithms.c        |
|  - Doubly Linked List (DLL)       |   |  - QuickSort (Lomuto)     |
|  - Circular Doubly List (CLL)     |   |  - MergeSort (Stable)     |
|  - Queue (FIFO Singly Linked)     |   |  - Binary Search (O(log n)|
|  - Stack (LIFO Singly Linked)     |   |  - Smart Shuffle Engine   |
|  - Binary Search Tree (BST)       |   +---------------------------+
+-----------------------------------+
```

---

## 3. Data Structures Used

### 3.1 Doubly Linked List (DLL)
- **Role:** Regular Playlist Navigation.
- **Node Definition:**
  ```c
  typedef struct DLLNode {
      Song song;
      struct DLLNode *prev;
      struct DLLNode *next;
  } DLLNode;
  ```
- **Why DLL?**
  A standard singly linked list only allows forward movement (`next`). A doubly linked list maintains both `next` and `prev` pointers, enabling constant-time $O(1)$ navigation for both "Next Track" and "Previous Track" actions. In addition, inserting or deleting songs in the middle or end of the playlist takes $O(1)$ once the node pointer is known, without shifting memory as required by contiguous arrays.

### 3.2 Circular Doubly Linked List (CLL)
- **Role:** "Repeat-All" Cyclic Playback Mode.
- **Structural Invariant:**
  - `head->prev == tail`
  - `tail->next == head`
- **Why CLL?**
  When Repeat-All mode is active, reaching the end of the playlist should naturally wrap around to the first song, and moving backward from the first song should wrap to the last song. A circular doubly linked list inherently satisfies this requirement without conditional boundary checks.

### 3.3 Queue (FIFO - First In, First Out)
- **Role:** "Up Next" Priority Playback Queue.
- **Node Definition:** Singly linked nodes with explicit `front` and `rear` pointers.
- **Operations:**
  - `enqueue(Queue *q, Song s)`: Inserts at `rear` ($O(1)$).
  - `dequeue(Queue *q, Song *outSong)`: Removes and returns from `front` ($O(1)$).
- **Why Queue?**
  Songs enqueued by the listener must be played in strict chronological order of addition. Furthermore, in `playNext()`, the player engine checks if `isQueueEmpty(&p->upNext)` is false. If true, the queue takes strict priority over regular DLL navigation.

### 3.4 Stack (LIFO - Last In, First Out)
- **Role:** Play History and "Back" Navigation.
- **Node Definition:** Singly linked nodes with explicit `top` pointer.
- **Operations:**
  - `pushStack(Stack *st, Song s)`: Pushes song onto `top` ($O(1)$).
  - `popStack(Stack *st, Song *outSong)`: Pops most recent song from `top` ($O(1)$).
  - `peekStack(const Stack *st, Song *outSong)`: Inspects active song ($O(1)$).
- **Why Stack?**
  A user may trigger playback via next, previous, direct search, or smart shuffle. Regardless of how a song was started, pressing "Back" must return to the song played immediately before it. The LIFO property naturally models this undo behavior.

### 3.5 Binary Search Tree (BST)
- **Role:** Alphabetical Song Library Catalog & Title Search.
- **Node Definition:**
  ```c
  typedef struct BSTNode {
      Song song;
      struct BSTNode *left;
      struct BSTNode *right;
  } BSTNode;
  ```
- **Key Property:**
  For any node $P$, all songs in $P\text{->left}$ have `title < P->song.title`, and all songs in $P\text{->right}$ have `title > P->song.title`.
- **In-Order Traversal ($O(n)$):**
  Visiting `Left -> Root -> Right` automatically outputs songs sorted alphabetically (A-Z) without executing an external sorting routine.
- **Lookup ($O(\log n)$ average):**
  Allows rapid case-insensitive title lookup.

---

## 4. Algorithms Used

### 4.1 QuickSort
- **Mechanism:** Lomuto partitioning scheme.
- **Partition Selection:** Last element in current subarray chosen as pivot.
- **Flexibility:** Supports sorting the song array by Title (choice 1), Artist (choice 2), or Duration (choice 3).
- **Time Complexity:** Average $O(n \log n)$, Worst-case $O(n^2)$.
- **Space Complexity:** $O(\log n)$ call-stack space.

### 4.2 MergeSort
- **Mechanism:** Divide-and-conquer splitting the array into halves until single-element subarrays are reached, followed by a linear merge pass.
- **Key Advantage:** MergeSort is **stable**. When sorting by Artist or Duration, songs with identical values retain their relative initial order.
- **Time Complexity:** Guaranteed $O(n \log n)$ across all cases.
- **Space Complexity:** $O(n)$ auxiliary array storage.

### 4.3 Binary Search
- **Mechanism:** Repeatedly halves the search space on a title-sorted array.
- **Time Complexity:** $O(\log n)$.
- **Comparison Count:** For a library of $n=1000$ songs, finds any track in at most $\lceil \log_2(1000) \rceil = 10$ comparisons.

### 4.4 The Algorithmic Twist: Smart Shuffle with Sliding Cooldown Window
- **The Problem:** Standard pseudo-random selection (`rand() % n`) exhibits Poisson clustering, resulting in identical or recent songs replaying after very short intervals.
- **The Algorithm:**
  1. **Window Size:** Fixed parameter $N = 4$ (`SMART_SHUFFLE_WINDOW`).
  2. **Step 1 (Window Extraction):** Inspect the top $N$ elements of the `history` stack without popping them. Extract their unique track IDs into `windowIds[N]`.
  3. **Step 2 (Candidate Filtering):** Scan the candidate song library and compile an array of indices whose `id` does **not** exist in `windowIds`.
  4. **Step 3 (Selection):** Pick uniformly at random (`rand() % candidateCount`) from the filtered candidates only.
  5. **Step 4 (Relaxation):** If total songs $M \le N$, the algorithm dynamically clamps the window size to $M - 1$, preventing deadlock or infinite loops.

---

## 5. Feature-to-Structure Mapping Table

| Application Feature | Primary Data Structure / Algorithm | Secondary Data Structure | Time Complexity | Space Complexity |
| :--- | :--- | :--- | :--- | :--- |
| **Playlist Step (Next/Prev)** | Doubly Linked List (DLL) | `currentTrack` pointer | $O(1)$ | $O(1)$ |
| **Repeat-All Looping** | Circular Doubly Linked List (CLL) | `repeatAll` flag | $O(1)$ | $O(1)$ |
| **Priority Queue ("Up Next")** | Queue (FIFO linked) | `front` & `rear` pointers | $O(1)$ | $O(1)$ |
| **Back Button Navigation** | Stack (LIFO linked) | `top` pointer | $O(1)$ | $O(1)$ |
| **Catalog Listing (A-Z)** | Binary Search Tree (BST) | In-Order Traversal | $O(n)$ | $O(h)$ |
| **Search by Title** | Binary Search Tree (BST) | Case-insensitive compare | $O(\log n)$ avg | $O(h)$ |
| **Multi-field Sorting** | QuickSort & MergeSort | Temporary merge arrays | $O(n \log n)$ | $O(n)$ |
| **Exact Array Search** | Binary Search | Sorted array | $O(\log n)$ | $O(1)$ |
| **Non-repeating Shuffle** | Sliding Cooldown Window | History Stack lookup | $O(n)$ | $O(N)$ |

---

## 6. Verification and Unit Testing

The project includes an exhaustive unit test suite (`test_suite.c`) using standard C `assert()` statements. Every data structure and algorithm is verified independently:

| Test Case | Module | Verification Performed | Status |
| :---: | :--- | :--- | :---: |
| **TC-1** | Doubly Linked List | Insertion, pointer linkage, deletion from head/mid/tail | **PASS** |
| **TC-2** | Circular Doubly List | Circular wrapping (`head->prev` and `tail->next`) | **PASS** |
| **TC-3** | Queue (FIFO) | Order preservation, empty/underflow checks | **PASS** |
| **TC-4** | Stack (LIFO) | Push, pop, peek, LIFO ordering | **PASS** |
| **TC-5** | Binary Search Tree | Alphabetical ordering, search hit, search miss | **PASS** |
| **TC-6** | Sorting Algorithms | QuickSort and MergeSort correctness across fields | **PASS** |
| **TC-7** | Binary Search | $O(\log n)$ index detection and missing element check | **PASS** |
| **TC-8** | Smart Shuffle | Zero occurrences of last 4 played songs across 30 runs | **PASS** |

---

## 7. Viva Voce Questions & Model Answers

### Q1: Why did you build custom data structures instead of using an array for everything?
**Answer:** While arrays offer $O(1)$ index access, insertions and deletions require $O(n)$ element shifting. In a media player, playlists are dynamic (songs are added, removed, and rearranged frequently). A Doubly Linked List provides $O(1)$ pointer manipulations for insertions and deletions. Furthermore, Queue (FIFO) and Stack (LIFO) model the exact access patterns required for "Up Next" and "Back Button" without risking array buffer overflow.

### Q2: What happens if `dequeue()` is called on an empty queue?
**Answer:** The function checks if `q->front == NULL`. If so, it returns `0` (false), preventing pointer dereferencing on null memory (queue underflow protection).

### Q3: How is memory managed in this project?
**Answer:** Every node allocated dynamically with `malloc()` is paired with a corresponding `free()` operation. Dedicated destruction functions (`freeDLL`, `freeCLL`, `freeQueue`, `freeStack`, `freeBST`, and `freePlayer`) ensure that when songs are removed or the program terminates, there are zero memory leaks.

### Q4: Why is In-Order traversal used on the BST for the catalogue?
**Answer:** An in-order traversal follows the recursive sequence: `Left Subtree -> Root -> Right Subtree`. Because a Binary Search Tree stores smaller keys on the left and larger keys on the right, visiting nodes in this sequence automatically extracts keys in non-decreasing (A-Z) alphabetical order in $O(n)$ time.

### Q5: How does the Smart Shuffle prevent infinite loops if the library has only 2 songs?
**Answer:** The algorithm incorporates a relaxation check: if the total candidate pool size is less than or equal to the window size ($M \le N$), the window size is dynamically relaxed to $M - 1$. For example, with 2 songs, the window becomes 1, so the player toggles between them rather than freezing in an infinite loop.

---

## 8. Future Scope

1. **Persistent Storage:** Integrating lightweight binary file I/O (`fread`/`fwrite`) or JSON loading to save playlists between sessions.
2. **Audio Decoding:** Interfacing with a lightweight audio decoding library (e.g., `miniaudio.h`) to stream actual MP3/WAV waveforms.
3. **Self-Balancing Trees:** Upgrading the BST to an AVL or Red-Black Tree to guarantee $O(\log n)$ worst-case search times during skewed sequential insertions.
4. **Volume Normalization:** Storing audio gain metrics inside the `Song` struct to auto-level playback loudness across tracks.

---

## 9. Conclusion

The Smart Media Player successfully demonstrates the practical utility of fundamental Data Structures and Algorithms. By directly mapping Doubly Linked Lists, Circular Lists, Queues, Stacks, Binary Search Trees, QuickSort, MergeSort, Binary Search, and a Sliding Cooldown Shuffle into an intuitive terminal application, the project delivers a clear, robust, and academically rigorous software engineering design.
