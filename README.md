# 🎵 Smart Media Player — Advanced DSA Project

A complete, high-performance **Smart Media Player** built from scratch using custom, node-based Data Structures and Algorithms in **Python** and **C++**.

Designed to satisfy academic project requirements with a clean, defensible architecture that avoids generic management clichés.

---

## 🚀 Key Features & DSA Mapping

| Feature | Data Structure / Algorithm | File |
| :--- | :--- | :--- |
| **Playlist (Next & Previous)** | **Doubly Linked List (DLL)** | [data_structures.py](data_structures.py) |
| **Repeat-All Mode** | **Circular Doubly Linked List (CLL)** | [data_structures.py](data_structures.py) |
| **"Up Next" Priority Queue** | **Queue (FIFO)** | [data_structures.py](data_structures.py) |
| **Play History & Back Button** | **Stack (LIFO)** | [data_structures.py](data_structures.py) |
| **Library Search by Title** | **Binary Search Tree (BST)** | [data_structures.py](data_structures.py) |
| **A-Z Catalogue Listing** | **BST In-order Traversal** | [data_structures.py](data_structures.py) |
| **Sorting by Title/Artist/Duration** | **QuickSort & MergeSort** | [algorithms.py](algorithms.py) |
| **Logarithmic Library Lookup** | **Binary Search ($O(\log n)$)** | [algorithms.py](algorithms.py) |
| **The Twist: Smart Shuffle** | **Sliding Cooldown Randomizer** | [algorithms.py](algorithms.py) |

> [!TIP]
> **The Algorithmic Twist:**
> Standard random shuffle often clusters and repeats recently played songs. The **Smart Shuffle Engine** maintains a sliding cooldown window of size $N$ (e.g., $N=4$ songs). It cross-references the candidate pool against the **History Stack** to guarantee that **no song played within the last $N$ tracks is repeated**.

---

## 📁 Project Directory Layout

```
smart_media_player/
├── models.py                # Song data model with duration formatting
├── data_structures.py        # Node-based DLL, CLL, Queue, Stack, BST
├── algorithms.py             # Custom QuickSort, MergeSort, Binary Search, Smart Shuffle
├── player.py                 # SmartMediaPlayer playback engine & precedence router
├── main.py                   # Interactive Terminal UI & automated demo
├── test_suite.py             # Exhaustive automated unit tests (9 tests)
├── smart_media_player.cpp    # Complete standalone C++17 implementation
├── PROJECT_REPORT.md         # Full academic project report with viva Q&A
└── README.md                 # Quickstart guide
```

---

## ⚡ How to Run

### 1. Interactive Terminal UI (Python)
Launch the interactive player with playback controls, search, queue, and dashboard:
```powershell
python main.py
```

### 2. Run Automated Examiner Demonstration
To showcase all 8 DSA modules sequentially in a live demo:
```powershell
python -c "import sys; sys.path.insert(0, '.'); from player import SmartMediaPlayer; from main import run_automated_live_demo; p = SmartMediaPlayer(); p.load_default_library(); run_automated_live_demo(p)"
```
*(Or launch `python main.py` and select option `17`)*.

### 3. Run Automated Unit Test Suite
Verify that every data structure, edge condition, and the smart shuffle cooldown passes:
```powershell
python test_suite.py
```

### 4. Compile and Run the C++ Implementation
If your examiner asks for C++:
```bash
g++ -std=c++17 smart_media_player.cpp -o smart_media_player
./smart_media_player
```
*(On Windows with MinGW or MSVC cl, or within any online C++ compiler like Replit/GDB).*

---

## 🎓 Viva Voce & Academic Defense
For the full complexity analysis table, architecture diagrams, and 20+ anticipated examiner questions with complete model answers, consult [PROJECT_REPORT.md](PROJECT_REPORT.md).
