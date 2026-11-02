/**
 * smart_media_player.cpp
 * Complete Data Structures & Algorithms Implementation for Smart Media Player:
 * 1. Doubly Linked List (DLL): Playlist navigation (previous and next)
 * 2. Circular Doubly Linked List (CLL): Repeat-all cyclic loop
 * 3. Singly Linked Queue (FIFO): "Up Next" queue priority playback
 * 4. Singly Linked Stack (LIFO): Play history and "Back" navigation
 * 5. Binary Search Tree (BST): Library indexed by title with in-order traversal
 * 6. QuickSort & MergeSort: Sorting by title, artist, duration
 * 7. Binary Search: O(log n) exact lookup on sorted arrays
 * 8. THE TWIST: Smart Shuffle with Cooldown N avoiding last N played songs
 *
 * Compile:
 *   g++ -std=c++17 smart_media_player.cpp -o smart_media_player
 *   ./smart_media_player
 */

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <random>
#include <chrono>
#include <cctype>

using namespace std;

// ============================================================================
// Song Model
// ============================================================================
struct Song {
    int id;
    string title;
    string artist;
    string album;
    int duration; // in seconds
    string genre;
    int play_count;

    Song() : id(0), title(""), artist(""), album(""), duration(0), genre(""), play_count(0) {}

    Song(int id, string title, string artist, string album, int duration, string genre = "Pop")
        : id(id), title(title), artist(artist), album(album), duration(duration), genre(genre), play_count(0) {}

    string getFormattedDuration() const {
        int m = duration / 60;
        int s = duration % 60;
        string res = (m < 10 ? "0" : "") + to_string(m) + ":" + (s < 10 ? "0" : "") + to_string(s);
        return res;
    }

    void incrementPlayCount() {
        play_count++;
    }
};

string toLower(const string& str) {
    string res = str;
    transform(res.begin(), res.end(), res.begin(), [](unsigned char c) { return tolower(c); });
    return res;
}

// ============================================================================
// 1. Doubly Linked List (DLL) - Playlist Navigation (Previous & Next)
// ============================================================================
struct DLLNode {
    Song song;
    DLLNode* prev;
    DLLNode* next;

    DLLNode(const Song& s) : song(s), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList {
public:
    DLLNode* head;
    DLLNode* tail;
    int size;

    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    ~DoublyLinkedList() {
        clear();
    }

    void clear() {
        DLLNode* curr = head;
        while (curr) {
            DLLNode* temp = curr->next;
            delete curr;
            curr = temp;
        }
        head = tail = nullptr;
        size = 0;
    }

    DLLNode* append(const Song& s) {
        DLLNode* newNode = new DLLNode(s);
        if (!head) {
            head = tail = newNode;
        } else {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
        size++;
        return newNode;
    }

    bool remove(int songId) {
        DLLNode* curr = head;
        while (curr) {
            if (curr->song.id == songId) {
                if (curr == head) {
                    head = curr->next;
                    if (head) head->prev = nullptr;
                    else tail = nullptr;
                } else if (curr == tail) {
                    tail = curr->prev;
                    if (tail) tail->next = nullptr;
                    else head = nullptr;
                } else {
                    curr->prev->next = curr->next;
                    curr->next->prev = curr->prev;
                }
                delete curr;
                size--;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    DLLNode* find(int songId) {
        DLLNode* curr = head;
        while (curr) {
            if (curr->song.id == songId) return curr;
            curr = curr->next;
        }
        return nullptr;
    }

    vector<Song> toVector() const {
        vector<Song> res;
        DLLNode* curr = head;
        while (curr) {
            res.push_back(curr->song);
            curr = curr->next;
        }
        return res;
    }
};

// ============================================================================
// 2. Circular Doubly Linked List (CLL) - Repeat-All Mode
// ============================================================================
struct CLLNode {
    Song song;
    CLLNode* prev;
    CLLNode* next;

    CLLNode(const Song& s) : song(s), prev(nullptr), next(nullptr) {}
};

class CircularDoublyLinkedList {
public:
    CLLNode* head;
    CLLNode* tail;
    int size;

    CircularDoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    ~CircularDoublyLinkedList() {
        clear();
    }

    void clear() {
        if (!head) return;
        CLLNode* curr = head;
        for (int i = 0; i < size; ++i) {
            CLLNode* temp = curr->next;
            delete curr;
            curr = temp;
        }
        head = tail = nullptr;
        size = 0;
    }

    CLLNode* append(const Song& s) {
        CLLNode* newNode = new CLLNode(s);
        if (!head) {
            head = tail = newNode;
            newNode->next = newNode;
            newNode->prev = newNode;
        } else {
            newNode->prev = tail;
            newNode->next = head;
            tail->next = newNode;
            head->prev = newNode;
            tail = newNode;
        }
        size++;
        return newNode;
    }

    CLLNode* find(int songId) {
        if (!head) return nullptr;
        CLLNode* curr = head;
        for (int i = 0; i < size; ++i) {
            if (curr->song.id == songId) return curr;
            curr = curr->next;
        }
        return nullptr;
    }
};

// ============================================================================
// 3. Queue (FIFO) - "Up Next" Queue
// ============================================================================
struct QueueNode {
    Song song;
    QueueNode* next;
    QueueNode(const Song& s) : song(s), next(nullptr) {}
};

class Queue {
public:
    QueueNode* front;
    QueueNode* rear;
    int count;

    Queue() : front(nullptr), rear(nullptr), count(0) {}

    ~Queue() {
        clear();
    }

    void clear() {
        while (!isEmpty()) dequeue();
    }

    bool isEmpty() const { return front == nullptr; }

    void enqueue(const Song& s) {
        QueueNode* newNode = new QueueNode(s);
        if (!rear) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        count++;
    }

    Song dequeue() {
        if (isEmpty()) return Song();
        QueueNode* temp = front;
        Song s = temp->song;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        count--;
        return s;
    }

    Song peek() const {
        if (isEmpty()) return Song();
        return front->song;
    }

    vector<Song> toVector() const {
        vector<Song> res;
        QueueNode* curr = front;
        while (curr) {
            res.push_back(curr->song);
            curr = curr->next;
        }
        return res;
    }
};

// ============================================================================
// 4. Stack (LIFO) - Play History & Back Navigation
// ============================================================================
struct StackNode {
    Song song;
    StackNode* next;
    StackNode(const Song& s) : song(s), next(nullptr) {}
};

class Stack {
public:
    StackNode* top;
    int count;

    Stack() : top(nullptr), count(0) {}

    ~Stack() {
        clear();
    }

    void clear() {
        while (!isEmpty()) pop();
    }

    bool isEmpty() const { return top == nullptr; }

    void push(const Song& s) {
        StackNode* newNode = new StackNode(s);
        newNode->next = top;
        top = newNode;
        count++;
    }

    Song pop() {
        if (isEmpty()) return Song();
        StackNode* temp = top;
        Song s = temp->song;
        top = top->next;
        delete temp;
        count--;
        return s;
    }

    Song peek() const {
        if (isEmpty()) return Song();
        return top->song;
    }

    vector<Song> toVector() const {
        vector<Song> res;
        StackNode* curr = top;
        while (curr) {
            res.push_back(curr->song);
            curr = curr->next;
        }
        return res;
    }
};

// ============================================================================
// 5. Binary Search Tree (BST) - Library Indexed by Title
// ============================================================================
struct BSTNode {
    Song song;
    string key;
    BSTNode* left;
    BSTNode* right;

    BSTNode(const Song& s) : song(s), key(toLower(s.title)), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
public:
    BSTNode* root;
    int count;

    BinarySearchTree() : root(nullptr), count(0) {}

    ~BinarySearchTree() {
        clear(root);
    }

    void clear(BSTNode* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    void insert(const Song& s) {
        BSTNode* newNode = new BSTNode(s);
        if (!root) {
            root = newNode;
            count++;
            return;
        }
        BSTNode* curr = root;
        while (true) {
            if (newNode->key < curr->key) {
                if (!curr->left) {
                    curr->left = newNode;
                    count++;
                    break;
                }
                curr = curr->left;
            } else {
                if (!curr->right) {
                    curr->right = newNode;
                    count++;
                    break;
                }
                curr = curr->right;
            }
        }
    }

    Song* search(const string& title) {
        string target = toLower(title);
        BSTNode* curr = root;
        while (curr) {
            if (target == curr->key) return &(curr->song);
            else if (target < curr->key) curr = curr->left;
            else curr = curr->right;
        }
        return nullptr;
    }

    void inorderHelper(BSTNode* node, vector<Song>& res) const {
        if (!node) return;
        inorderHelper(node->left, res);
        res.push_back(node->song);
        inorderHelper(node->right, res);
    }

    vector<Song> inorder() const {
        vector<Song> res;
        inorderHelper(root, res);
        return res;
    }
};

// ============================================================================
// 6. Sorting Algorithms (QuickSort & MergeSort) & Binary Search
// ============================================================================
bool compareSongs(const Song& a, const Song& b, const string& key, bool ascending) {
    if (key == "title") {
        return ascending ? toLower(a.title) < toLower(b.title) : toLower(a.title) > toLower(b.title);
    } else if (key == "artist") {
        return ascending ? toLower(a.artist) < toLower(b.artist) : toLower(a.artist) > toLower(b.artist);
    } else if (key == "duration") {
        return ascending ? a.duration < b.duration : a.duration > b.duration;
    } else if (key == "play_count") {
        return ascending ? a.play_count < b.play_count : a.play_count > b.play_count;
    }
    return ascending ? toLower(a.title) < toLower(b.title) : toLower(a.title) > toLower(b.title);
}

int partition(vector<Song>& arr, int low, int high, const string& key, bool ascending) {
    Song pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (compareSongs(arr[j], pivot, key, ascending)) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSortHelper(vector<Song>& arr, int low, int high, const string& key, bool ascending) {
    if (low < high) {
        int pi = partition(arr, low, high, key, ascending);
        quickSortHelper(arr, low, pi - 1, key, ascending);
        quickSortHelper(arr, pi + 1, high, key, ascending);
    }
}

void quickSort(vector<Song>& arr, const string& key = "title", bool ascending = true) {
    if (arr.size() <= 1) return;
    quickSortHelper(arr, 0, arr.size() - 1, key, ascending);
}

void merge(vector<Song>& arr, int left, int mid, int right, const string& key, bool ascending) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<Song> L(n1), R(n2);
    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (compareSongs(L[i], R[j], key, ascending) || (!compareSongs(R[j], L[i], key, ascending))) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSortHelper(vector<Song>& arr, int left, int right, const string& key, bool ascending) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortHelper(arr, left, mid, key, ascending);
        mergeSortHelper(arr, mid + 1, right, key, ascending);
        merge(arr, left, mid, right, key, ascending);
    }
}

void mergeSort(vector<Song>& arr, const string& key = "title", bool ascending = true) {
    if (arr.size() <= 1) return;
    mergeSortHelper(arr, 0, arr.size() - 1, key, ascending);
}

int binarySearchTitle(const vector<Song>& sortedArr, const string& title) {
    string target = toLower(title);
    int low = 0, high = sortedArr.size() - 1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        string midTitle = toLower(sortedArr[mid].title);
        if (midTitle == target) return mid;
        else if (midTitle < target) low = mid + 1;
        else high = mid - 1;
    }
    return -1;
}

// ============================================================================
// 7. THE TWIST: Smart Shuffle Algorithm with Cooldown N
// ============================================================================
Song smartShuffleSelect(const vector<Song>& pool, Stack& history, int cooldownN, int currentSongId) {
    if (pool.empty()) return Song();
    if (pool.size() == 1) return pool[0];

    // Collect IDs of last N songs from history stack
    vector<int> recentIds;
    StackNode* curr = history.top;
    int count = 0;
    while (curr && count < cooldownN) {
        recentIds.push_back(curr->song.id);
        curr = curr->next;
        count++;
    }
    if (currentSongId != 0 && find(recentIds.begin(), recentIds.end(), currentSongId) == recentIds.end()) {
        recentIds.push_back(currentSongId);
    }

    // Filter candidate pool
    vector<Song> eligible;
    for (const auto& s : pool) {
        if (find(recentIds.begin(), recentIds.end(), s.id) == recentIds.end()) {
            eligible.push_back(s);
        }
    }

    static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

    if (!eligible.empty()) {
        uniform_int_distribution<int> dist(0, eligible.size() - 1);
        return eligible[dist(rng)];
    }

    // Fallback: pick song from pool (excluding current) that was played furthest ago
    for (const auto& s : pool) {
        if (s.id != currentSongId) return s;
    }
    return pool[0];
}

// ============================================================================
// 8. Main CLI Program
// ============================================================================
int main() {
    DoublyLinkedList playlist;
    CircularDoublyLinkedList repeatPlaylist;
    Queue upNextQueue;
    Stack historyStack;
    BinarySearchTree libraryBST;
    vector<Song> library;

    int cooldownN = 4;
    bool repeatAll = false;
    bool smartShuffle = false;
    bool isPlaying = false;
    Song currentSong;
    DLLNode* currentDLL = nullptr;
    CLLNode* currentCLL = nullptr;

    // Load sample songs
    vector<Song> defaultSongs = {
        Song(1, "Bohemian Rhapsody", "Queen", "A Night at the Opera", 354, "Rock"),
        Song(2, "Blinding Lights", "The Weeknd", "After Hours", 200, "Synthpop"),
        Song(3, "Hotel California", "Eagles", "Hotel California", 391, "Classic Rock"),
        Song(4, "Shape of You", "Ed Sheeran", "Divide", 233, "Pop"),
        Song(5, "Billie Jean", "Michael Jackson", "Thriller", 294, "Pop"),
        Song(6, "Starboy", "The Weeknd", "Starboy", 230, "R&B"),
        Song(7, "Smells Like Teen Spirit", "Nirvana", "Nevermind", 301, "Grunge"),
        Song(8, "Stay", "The Kid LAROI", "F*ck Love 3", 141, "Pop"),
        Song(9, "As It Was", "Harry Styles", "Harry's House", 167, "Indie Pop"),
        Song(10, "Rolling in the Deep", "Adele", "21", 228, "Soul")
    };

    for (const auto& s : defaultSongs) {
        library.push_back(s);
        libraryBST.insert(s);
        DLLNode* dn = playlist.append(s);
        CLLNode* cn = repeatPlaylist.append(s);
        if (!currentDLL) {
            currentDLL = dn;
            currentCLL = cn;
            currentSong = s;
        }
    }

    cout << "======================================================================\n";
    cout << "    SMART MEDIA PLAYER (C++ DATA STRUCTURES & ALGORITHMS ENGINE)      \n";
    cout << "======================================================================\n";
    cout << "Loaded " << library.size() << " initial songs into Library & Playlist.\n\n";

    int choice = -1;
    while (choice != 0) {
        cout << "\n---------------- CURRENT PLAYBACK DASHBOARD ----------------\n";
        cout << "Status: " << (isPlaying ? "[>] PLAYING" : "[||] PAUSED")
             << " | Repeat-All (CLL): " << (repeatAll ? "ON" : "OFF")
             << " | Smart Shuffle: " << (smartShuffle ? ("ON (N=" + to_string(cooldownN) + ")") : "OFF") << "\n";
        if (currentSong.id != 0) {
            cout << "Now Playing: '" << currentSong.title << "' by " << currentSong.artist
                 << " (" << currentSong.getFormattedDuration() << ") [Plays: " << currentSong.play_count << "]\n";
        } else {
            cout << "Now Playing: None\n";
        }
        cout << "Up Next (FIFO Queue Size: " << upNextQueue.count << ") | History (Stack Size: " << historyStack.count << ")\n";
        cout << "------------------------------------------------------------\n";
        cout << " [1] Play/Pause Toggle         [2] Next Song (Queue > Shuffle > CLL > DLL)\n";
        cout << " [3] Previous/Back (Stack)     [4] Add Song to 'Up Next' Queue (FIFO)\n";
        cout << " [5] Toggle Repeat-All (CLL)   [6] Toggle Smart Shuffle (Twist: Cooldown N)\n";
        cout << " [7] Search BST by Title       [8] In-order BST Alphabetical Catalogue\n";
        cout << " [9] QuickSort Library         [10] Binary Search Title O(log n)\n";
        cout << " [11] View Playlist (DLL)      [12] View 'Up Next' & History\n";
        cout << " [0] Exit\n";
        cout << "Enter choice (0-12): ";
        if (!(cin >> choice)) {
            cin.clear();
            string discard;
            cin >> discard;
            continue;
        }

        if (choice == 1) {
            isPlaying = !isPlaying;
            if (isPlaying && currentSong.id != 0) currentSong.incrementPlayCount();
            cout << "Playback is now " << (isPlaying ? "PLAYING" : "PAUSED") << ".\n";
        }
        else if (choice == 2) { // Next Song
            if (currentSong.id != 0) historyStack.push(currentSong);

            if (!upNextQueue.isEmpty()) {
                currentSong = upNextQueue.dequeue();
                currentSong.incrementPlayCount();
                isPlaying = true;
                currentDLL = playlist.find(currentSong.id);
                currentCLL = repeatPlaylist.find(currentSong.id);
                cout << ">> Played from 'Up Next' FIFO Queue: '" << currentSong.title << "'\n";
            }
            else if (smartShuffle) {
                currentSong = smartShuffleSelect(library, historyStack, cooldownN, currentSong.id);
                currentSong.incrementPlayCount();
                isPlaying = true;
                currentDLL = playlist.find(currentSong.id);
                currentCLL = repeatPlaylist.find(currentSong.id);
                cout << ">> Smart Shuffle Selected: '" << currentSong.title << "' (Avoided last " << cooldownN << " plays)\n";
            }
            else if (repeatAll && currentCLL) {
                currentCLL = currentCLL->next;
                currentSong = currentCLL->song;
                currentSong.incrementPlayCount();
                isPlaying = true;
                currentDLL = playlist.find(currentSong.id);
                cout << ">> CLL Loop Next: '" << currentSong.title << "'\n";
            }
            else if (currentDLL && currentDLL->next) {
                currentDLL = currentDLL->next;
                currentSong = currentDLL->song;
                currentSong.incrementPlayCount();
                isPlaying = true;
                currentCLL = repeatPlaylist.find(currentSong.id);
                cout << ">> DLL Next: '" << currentSong.title << "'\n";
            }
            else {
                cout << "Reached end of DLL playlist. Turn on Repeat-All (CLL) or Smart Shuffle to continue.\n";
            }
        }
        else if (choice == 3) { // Back Navigation (Stack LIFO)
            if (!historyStack.isEmpty()) {
                currentSong = historyStack.pop();
                currentSong.incrementPlayCount();
                isPlaying = true;
                currentDLL = playlist.find(currentSong.id);
                currentCLL = repeatPlaylist.find(currentSong.id);
                cout << ">> History Stack Popped: '" << currentSong.title << "'\n";
            } else if (currentDLL && currentDLL->prev) {
                currentDLL = currentDLL->prev;
                currentSong = currentDLL->song;
                isPlaying = true;
                cout << ">> DLL Previous: '" << currentSong.title << "'\n";
            } else {
                cout << "No previous songs in history or playlist.\n";
            }
        }
        else if (choice == 4) { // Add to Queue
            cout << "Select Song ID to Enqueue:\n";
            for (const auto& s : library) {
                cout << " [" << s.id << "] " << s.title << " - " << s.artist << "\n";
            }
            int qid;
            cout << "Enter ID: ";
            if (cin >> qid) {
                auto it = find_if(library.begin(), library.end(), [qid](const Song& s) { return s.id == qid; });
                if (it != library.end()) {
                    upNextQueue.enqueue(*it);
                    cout << "Enqueued '" << it->title << "' to Up Next FIFO Queue.\n";
                } else {
                    cout << "Song ID not found.\n";
                }
            }
        }
        else if (choice == 5) {
            repeatAll = !repeatAll;
            cout << "Repeat-All (CLL) is now " << (repeatAll ? "ENABLED" : "DISABLED") << ".\n";
        }
        else if (choice == 6) {
            smartShuffle = !smartShuffle;
            cout << "Smart Shuffle (Cooldown N=" << cooldownN << ") is now " << (smartShuffle ? "ENABLED" : "DISABLED") << ".\n";
        }
        else if (choice == 7) {
            cin.ignore();
            cout << "Enter Song Title to search in BST: ";
            string q;
            getline(cin, q);
            Song* found = libraryBST.search(q);
            if (found) {
                cout << "BST Found: '" << found->title << "' by " << found->artist << " (" << found->getFormattedDuration() << ")\n";
            } else {
                cout << "Title not found in BST.\n";
            }
        }
        else if (choice == 8) {
            cout << "\n--- BST In-Order Alphabetical Catalogue ---\n";
            vector<Song> inorderList = libraryBST.inorder();
            for (const auto& s : inorderList) {
                cout << " * " << left << setw(26) << s.title << " | " << setw(20) << s.artist << " | " << s.getFormattedDuration() << "\n";
            }
        }
        else if (choice == 9) {
            vector<Song> sortedLib = library;
            quickSort(sortedLib, "duration", false);
            cout << "\n--- Library Sorted by Duration Descending (QuickSort) ---\n";
            for (const auto& s : sortedLib) {
                cout << " * " << left << setw(26) << s.title << " | " << s.getFormattedDuration() << " (" << s.duration << "s)\n";
            }
        }
        else if (choice == 10) {
            vector<Song> sortedLib = library;
            quickSort(sortedLib, "title", true);
            cin.ignore();
            cout << "Enter exact title for Binary Search: ";
            string q;
            getline(cin, q);
            int idx = binarySearchTitle(sortedLib, q);
            if (idx != -1) {
                cout << "Binary Search O(log n): Found at index " << idx << " -> '" << sortedLib[idx].title << "' by " << sortedLib[idx].artist << "\n";
            } else {
                cout << "Binary Search: Title not found in sorted library.\n";
            }
        }
        else if (choice == 11) {
            cout << "\n--- Current Playlist (DLL) ---\n";
            vector<Song> pl = playlist.toVector();
            for (const auto& s : pl) {
                cout << (s.id == currentSong.id ? " > " : "   ")
                     << "[" << s.id << "] " << left << setw(24) << s.title << " | " << s.artist << "\n";
            }
        }
        else if (choice == 12) {
            cout << "\n--- Up Next Queue (FIFO) ---\n";
            vector<Song> qv = upNextQueue.toVector();
            if (qv.empty()) cout << "  (Empty)\n";
            for (size_t i = 0; i < qv.size(); ++i) {
                cout << "  " << (i + 1) << ". " << qv[i].title << "\n";
            }
            cout << "\n--- Play History Stack (LIFO: Top Most Recent) ---\n";
            vector<Song> hv = historyStack.toVector();
            if (hv.empty()) cout << "  (Empty)\n";
            for (size_t i = 0; i < hv.size(); ++i) {
                cout << "  [Top -" << i << "] " << hv[i].title << "\n";
            }
        }
    }

    cout << "Exiting Smart Media Player. Goodbye!\n";
    return 0;
}
