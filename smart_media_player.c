/**
 * ============================================================================
 * Smart Media Player - Complete Single-File Implementation in Pure C
 * ============================================================================
 * College Data Structures and Algorithms (DSA) Project
 * Language: Pure C (C99, standard library only, gcc compatible)
 *
 * Included Data Structures:
 *   1. Doubly Linked List (DLL)          - Regular Playlist Navigation (Next/Prev)
 *   2. Circular Doubly Linked List (CLL) - Repeat-All Mode (Cyclic wrapping)
 *   3. Queue (FIFO)                      - "Up Next" Priority Playback Queue
 *   4. Stack (LIFO)                      - Play History & Back Button
 *   5. Binary Search Tree (BST)          - Song Catalog & Title Search (A-Z)
 *
 * Included Algorithms:
 *   - QuickSort & MergeSort              - Multi-field sorting (Title/Artist/Duration)
 *   - Binary Search                      - O(log n) exact title lookup
 *   - Smart Shuffle                      - Sliding Cooldown Window (N = 4)
 *
 * Compilation:
 *   gcc -Wall -Wextra -std=c99 smart_media_player.c -o smart_media_player
 *   ./smart_media_player
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_TITLE 50
#define MAX_ARTIST 50
#define MAX_LIBRARY 50
#define SMART_SHUFFLE_WINDOW 4

// ============================================================================
// Data Models and Node Definitions
// ============================================================================

// Song structure storing basic track information
typedef struct Song {
    int id;
    char title[MAX_TITLE];
    char artist[MAX_ARTIST];
    int duration; // Duration in seconds
} Song;

// Doubly Linked List Node for regular playlist
typedef struct DLLNode {
    Song song;
    struct DLLNode *prev;
    struct DLLNode *next;
} DLLNode;

// Doubly Linked List structure
typedef struct DLL {
    DLLNode *head;
    DLLNode *tail;
    int size;
} DLL;

// Circular Doubly Linked List Node for Repeat-All mode
typedef struct CLLNode {
    Song song;
    struct CLLNode *prev;
    struct CLLNode *next;
} CLLNode;

// Circular Doubly Linked List structure
typedef struct CLL {
    CLLNode *head;
    int size;
} CLL;

// Queue Node for Up Next priority queue (FIFO)
typedef struct QueueNode {
    Song song;
    struct QueueNode *next;
} QueueNode;

// Queue structure
typedef struct Queue {
    QueueNode *front;
    QueueNode *rear;
    int size;
} Queue;

// Stack Node for Play History (LIFO)
typedef struct StackNode {
    Song song;
    struct StackNode *next;
} StackNode;

// Stack structure
typedef struct Stack {
    StackNode *top;
    int size;
} Stack;

// Binary Search Tree Node for A-Z catalog
typedef struct BSTNode {
    Song song;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

// Main Player Engine structure connecting all modules
typedef struct Player {
    DLL playlist;                  // Doubly linked list for standard playback
    CLL circularPlaylist;          // Circular doubly linked list for repeat-all
    Queue upNext;                  // FIFO queue for "Up Next" songs
    Stack history;                 // LIFO stack for "Back" button history
    BSTNode *libraryBST;           // Binary Search Tree for catalog lookup
    Song libraryArray[MAX_LIBRARY];// Array for sorting and binary search
    int libraryCount;              // Total number of songs in library
    DLLNode *currentTrack;         // Pointer to currently playing song in DLL
    int repeatAll;                 // 0 = Off, 1 = On (uses CLL logic)
    int isPlaying;                 // 0 = Stopped, 1 = Playing
} Player;

// ============================================================================
// Helper Functions
// ============================================================================

// Creates and returns a new Song struct
Song createSong(int id, const char *title, const char *artist, int duration) {
    Song s;
    s.id = id;
    strncpy(s.title, title, MAX_TITLE - 1);
    s.title[MAX_TITLE - 1] = '\0';
    strncpy(s.artist, artist, MAX_ARTIST - 1);
    s.artist[MAX_ARTIST - 1] = '\0';
    s.duration = duration;
    return s;
}

// Formats seconds into MM:SS format string
void formatDuration(int duration, char *buffer) {
    int minutes = duration / 60;
    int seconds = duration % 60;
    sprintf(buffer, "%02d:%02d", minutes, seconds);
}

// Prints a single song in a clean formatted row
void printSong(Song s) {
    char timeStr[10];
    formatDuration(s.duration, timeStr);
    printf("  [%2d] %-25s | %-18s | %s\n", s.id, s.title, s.artist, timeStr);
}

// Compares two strings case-insensitively
int stringCompareIgnoreCase(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

// ============================================================================
// 1. Doubly Linked List (DLL) - Playlist Navigation (Next / Prev)
// ============================================================================

// Initializes an empty Doubly Linked List
void initDLL(DLL *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

// Appends a new song to the end of the Doubly Linked List
void addSongDLL(DLL *list, Song s) {
    DLLNode *newNode = (DLLNode *)malloc(sizeof(DLLNode));
    newNode->song = s;
    newNode->next = NULL;
    newNode->prev = list->tail; // Link backward to current tail

    if (list->head == NULL) {
        list->head = newNode;   // First node becomes both head and tail
    } else {
        list->tail->next = newNode; // Link forward from current tail
    }
    list->tail = newNode;       // Update tail pointer to the new node
    list->size++;
}

// Removes a song by its ID from the Doubly Linked List
int removeSongDLL(DLL *list, int songId) {
    DLLNode *curr = list->head;
    while (curr != NULL && curr->song.id != songId) {
        curr = curr->next;
    }
    if (curr == NULL) return 0; // Song not found

    // Adjust surrounding pointers to bypass the removed node
    if (curr->prev != NULL) curr->prev->next = curr->next;
    else list->head = curr->next; // Removing head node

    if (curr->next != NULL) curr->next->prev = curr->prev;
    else list->tail = curr->prev; // Removing tail node

    free(curr);
    list->size--;
    return 1;
}

// Displays all songs in the Doubly Linked List
void displayDLL(const DLL *list) {
    if (list->head == NULL) {
        printf("  (Playlist is empty)\n");
        return;
    }
    DLLNode *curr = list->head;
    while (curr != NULL) {
        printSong(curr->song);
        curr = curr->next;
    }
    printf("  Total songs in playlist: %d\n", list->size);
}

// Displays playlist with an active indicator on the currently playing song
void displayPlaylistWithCurrent(const DLL *list, const DLLNode *current) {
    if (list->head == NULL) {
        printf("  (Playlist is empty)\n");
        return;
    }
    DLLNode *curr = list->head;
    while (curr != NULL) {
        char timeStr[10];
        formatDuration(curr->song.duration, timeStr);
        if (curr == current) {
            printf("  >[%2d] %-25s | %-18s | %s  <-- [PLAYING NOW]\n",
                   curr->song.id, curr->song.title, curr->song.artist, timeStr);
        } else {
            printf("   [%2d] %-25s | %-18s | %s\n",
                   curr->song.id, curr->song.title, curr->song.artist, timeStr);
        }
        curr = curr->next;
    }
}

// Searches for a song by ID in the Doubly Linked List
DLLNode* findSongDLL(const DLL *list, int songId) {
    DLLNode *curr = list->head;
    while (curr != NULL) {
        if (curr->song.id == songId) return curr;
        curr = curr->next;
    }
    return NULL;
}

// Frees all memory allocated for the Doubly Linked List
void freeDLL(DLL *list) {
    DLLNode *curr = list->head;
    while (curr != NULL) {
        DLLNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

// ============================================================================
// 2. Circular Doubly Linked List (CLL) - Repeat-All Mode
// ============================================================================

// Initializes an empty Circular Doubly Linked List
void initCLL(CLL *list) {
    list->head = NULL;
    list->size = 0;
}

// Adds a song to the Circular Doubly Linked List maintaining cyclic links
void addSongCLL(CLL *list, Song s) {
    CLLNode *newNode = (CLLNode *)malloc(sizeof(CLLNode));
    newNode->song = s;

    if (list->head == NULL) {
        newNode->next = newNode; // Points to itself when single node
        newNode->prev = newNode;
        list->head = newNode;
    } else {
        CLLNode *tail = list->head->prev; // In CLL, head->prev is the tail
        tail->next = newNode;
        newNode->prev = tail;
        newNode->next = list->head;       // Complete the circle to head
        list->head->prev = newNode;       // Head links backward to new tail
    }
    list->size++;
}

// Displays all songs in the Circular Doubly Linked List
void displayCLL(const CLL *list) {
    if (list->head == NULL) {
        printf("  (Circular playlist is empty)\n");
        return;
    }
    CLLNode *curr = list->head;
    do {
        printSong(curr->song);
        curr = curr->next;
    } while (curr != list->head); // Stop when circle loops back to head
    printf("  [Looping Enabled: Last track wraps back to '%s']\n", list->head->song.title);
}

// Frees all memory allocated for the Circular Doubly Linked List
void freeCLL(CLL *list) {
    if (list->head == NULL) return;
    CLLNode *curr = list->head;
    CLLNode *tail = list->head->prev;
    tail->next = NULL; // Break circle so we can free like a normal list

    while (curr != NULL) {
        CLLNode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    list->head = NULL;
    list->size = 0;
}

// ============================================================================
// 3. Queue (FIFO) - "Up Next" Priority Playback Queue
// ============================================================================

// Initializes an empty Queue
void initQueue(Queue *q) {
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

// Enqueues a song at the rear of the Queue
void enqueue(Queue *q, Song s) {
    QueueNode *newNode = (QueueNode *)malloc(sizeof(QueueNode));
    newNode->song = s;
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = newNode; // If empty, new node is both front and rear
    } else {
        q->rear->next = newNode;
    }
    q->rear = newNode;
    q->size++;
}

// Dequeues a song from the front of the Queue (First In, First Out)
int dequeue(Queue *q, Song *outSong) {
    if (q->front == NULL) return 0; // Queue underflow (empty)

    QueueNode *temp = q->front;
    *outSong = temp->song;
    q->front = q->front->next;

    if (q->front == NULL) q->rear = NULL; // Queue is now empty
    free(temp);
    q->size--;
    return 1;
}

// Displays all songs currently waiting in the Up Next Queue
void displayQueue(const Queue *q) {
    if (q->front == NULL) {
        printf("  (Up Next queue is empty)\n");
        return;
    }
    QueueNode *curr = q->front;
    int pos = 1;
    while (curr != NULL) {
        printf("  %d. [%d] %s by %s\n", pos++, curr->song.id, curr->song.title, curr->song.artist);
        curr = curr->next;
    }
}

// Checks if the Queue is empty
int isQueueEmpty(const Queue *q) {
    return (q->front == NULL);
}

// Frees all memory allocated for the Queue
void freeQueue(Queue *q) {
    Song dummy;
    while (dequeue(q, &dummy)); // Dequeue all elements
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

// ============================================================================
// 4. Stack (LIFO) - Play History for the Back Button
// ============================================================================

// Initializes an empty Stack
void initStack(Stack *st) {
    st->top = NULL;
    st->size = 0;
}

// Pushes a played song onto the top of the history Stack
void pushStack(Stack *st, Song s) {
    StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
    newNode->song = s;
    newNode->next = st->top; // Insert at head of linked list
    st->top = newNode;
    st->size++;
}

// Pops the most recently played song from the Stack (Last In, First Out)
int popStack(Stack *st, Song *outSong) {
    if (st->top == NULL) return 0; // Stack underflow (empty)

    StackNode *temp = st->top;
    *outSong = temp->song;
    st->top = st->top->next;
    free(temp);
    st->size--;
    return 1;
}

// Peeks at the top song on the Stack without removing it
int peekStack(const Stack *st, Song *outSong) {
    if (st->top == NULL) return 0;
    *outSong = st->top->song;
    return 1;
}

// Displays the entire play history from newest to oldest
void displayStack(const Stack *st) {
    if (st->top == NULL) {
        printf("  (Play history is empty)\n");
        return;
    }
    StackNode *curr = st->top;
    int step = 1;
    while (curr != NULL) {
        printf("  %d. [Recently Played] %s by %s\n", step++, curr->song.title, curr->song.artist);
        curr = curr->next;
    }
}

// Checks if the Stack is empty
int isStackEmpty(const Stack *st) {
    return (st->top == NULL);
}

// Frees all memory allocated for the Stack
void freeStack(Stack *st) {
    Song dummy;
    while (popStack(st, &dummy)); // Pop all elements
    st->top = NULL;
    st->size = 0;
}

// ============================================================================
// 5. Binary Search Tree (BST) - Library Catalog Keyed by Title
// ============================================================================

// Allocates and initializes a new BST node
BSTNode* createBSTNode(Song s) {
    BSTNode *node = (BSTNode *)malloc(sizeof(BSTNode));
    node->song = s;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Recursively inserts a song into the BST ordered alphabetically by title
BSTNode* insertBST(BSTNode *root, Song s) {
    if (root == NULL) return createBSTNode(s);

    int cmp = stringCompareIgnoreCase(s.title, root->song.title);
    if (cmp < 0) {
        root->left = insertBST(root->left, s);   // Go to left subtree
    } else {
        root->right = insertBST(root->right, s); // Go to right subtree
    }
    return root;
}

// Recursively searches for a song by title in the BST
BSTNode* searchBST(BSTNode *root, const char *title) {
    if (root == NULL) return NULL;

    int cmp = stringCompareIgnoreCase(title, root->song.title);
    if (cmp == 0) return root;                   // Match found!
    if (cmp < 0) return searchBST(root->left, title);
    return searchBST(root->right, title);
}

// In-order traversal (Left, Root, Right) to print songs in A-Z alphabetical order
void inorderBST(BSTNode *root) {
    if (root == NULL) return;
    inorderBST(root->left);  // Visit left subtree (smaller titles)
    printSong(root->song);   // Visit root node
    inorderBST(root->right); // Visit right subtree (larger titles)
}

// Recursively frees all nodes in the BST
void freeBST(BSTNode *root) {
    if (root == NULL) return;
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}

// ============================================================================
// Algorithms: Sorting, Searching, and Smart Shuffle
// ============================================================================

// Compares two songs based on user choice: 1=Title, 2=Artist, 3=Duration
int compareSongs(Song a, Song b, int choice) {
    if (choice == 1) {
        return stringCompareIgnoreCase(a.title, b.title);
    } else if (choice == 2) {
        return stringCompareIgnoreCase(a.artist, b.artist);
    } else {
        return a.duration - b.duration;
    }
}

// Partition function for QuickSort using Lomuto scheme
int partition(Song arr[], int low, int high, int choice) {
    Song pivot = arr[high]; // Last element is chosen as pivot
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (compareSongs(arr[j], pivot, choice) < 0) {
            i++;
            Song temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    Song temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

// QuickSort algorithm to recursively sort song array
void quickSort(Song arr[], int low, int high, int choice) {
    if (low < high) {
        int pi = partition(arr, low, high, choice);
        quickSort(arr, low, pi - 1, choice);
        quickSort(arr, pi + 1, high, choice);
    }
}

// Merges two sorted subarrays into one sorted array
void merge(Song arr[], int left, int mid, int right, int choice) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    Song L[MAX_LIBRARY], R[MAX_LIBRARY];

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (compareSongs(L[i], R[j], choice) <= 0) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// MergeSort algorithm to divide and sort song array
void mergeSort(Song arr[], int left, int right, int choice) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid, choice);
        mergeSort(arr, mid + 1, right, choice);
        merge(arr, left, mid, right, choice);
    }
}

// Binary Search for a song title in an alphabetically sorted array: O(log n)
int binarySearchSong(Song arr[], int n, const char *title) {
    int low = 0;
    int high = n - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = stringCompareIgnoreCase(title, arr[mid].title);

        if (cmp == 0) return mid;
        else if (cmp < 0) high = mid - 1;
        else low = mid + 1;
    }
    return -1;
}

// Checks if a given song ID is present inside the cooldown window
static int isIdInCooldown(int id, const int windowIds[], int count) {
    for (int i = 0; i < count; i++) {
        if (windowIds[i] == id) return 1;
    }
    return 0;
}

// Collects the most recent N song IDs from the history stack
static int getCooldownWindow(const Stack *history, int maxWindow, int windowIds[]) {
    int count = 0;
    StackNode *curr = history->top;
    while (curr != NULL && count < maxWindow) {
        windowIds[count++] = curr->song.id;
        curr = curr->next;
    }
    return count;
}

// Smart Shuffle: Picks a random song avoiding any song in the last N history window
Song smartShufflePick(Song pool[], int poolSize, const Stack *history, int windowSize) {
    if (poolSize <= 0) return createSong(-1, "", "", 0);
    if (poolSize == 1) return pool[0];

    // Relax window size if total pool is too small so it never gets stuck
    if (poolSize <= windowSize) windowSize = poolSize - 1;

    int windowIds[SMART_SHUFFLE_WINDOW];
    int windowCount = getCooldownWindow(history, windowSize, windowIds);

    int candidates[MAX_LIBRARY];
    int candidateCount = 0;
    for (int i = 0; i < poolSize; i++) {
        if (!isIdInCooldown(pool[i].id, windowIds, windowCount)) {
            candidates[candidateCount++] = i;
        }
    }

    if (candidateCount == 0) return pool[rand() % poolSize];
    int pickedIndex = candidates[rand() % candidateCount];
    return pool[pickedIndex];
}

// ============================================================================
// Player Engine Functions
// ============================================================================

// Adds a new song to the library array, BST, DLL, and CLL
void addNewSongToLibrary(Player *p, Song s) {
    if (p->libraryCount < MAX_LIBRARY) {
        p->libraryArray[p->libraryCount++] = s;
    }
    p->libraryBST = insertBST(p->libraryBST, s);
    addSongDLL(&p->playlist, s);
    addSongCLL(&p->circularPlaylist, s);
}

// Initializes all data structures and state variables of the Player
void initPlayer(Player *p) {
    initDLL(&p->playlist);
    initCLL(&p->circularPlaylist);
    initQueue(&p->upNext);
    initStack(&p->history);
    p->libraryBST = NULL;
    p->libraryCount = 0;
    p->currentTrack = NULL;
    p->repeatAll = 0;
    p->isPlaying = 0;
}

// Loads 10 starter songs into the library array, BST, DLL, and CLL
void loadDefaultSongs(Player *p) {
    Song list[10] = {
        createSong(1, "Bohemian Rhapsody", "Queen", 354),
        createSong(2, "Blinding Lights", "The Weeknd", 200),
        createSong(3, "Hotel California", "Eagles", 391),
        createSong(4, "Shape of You", "Ed Sheeran", 233),
        createSong(5, "Billie Jean", "Michael Jackson", 294),
        createSong(6, "Starboy", "The Weeknd", 230),
        createSong(7, "Smells Like Teen Spirit", "Nirvana", 301),
        createSong(8, "Stay", "The Kid LAROI", 141),
        createSong(9, "As It Was", "Harry Styles", 167),
        createSong(10, "Rolling in the Deep", "Adele", 228)
    };
    for (int i = 0; i < 10; i++) {
        addNewSongToLibrary(p, list[i]);
    }
}

// Prints clear playback status showing PREV song, NOW PLAYING song, and NEXT song
void printPlaybackStatus(const Player *p) {
    if (p->currentTrack == NULL) {
        printf("  [Status]: Idle (No song currently playing)\n");
        return;
    }
    printf("  -------------------------------------------------------------\n");
    if (p->currentTrack->prev != NULL) {
        printf("  [<< PREV TRACK]: (#%d) '%s' by %s\n",
               p->currentTrack->prev->song.id, p->currentTrack->prev->song.title, p->currentTrack->prev->song.artist);
    } else if (p->repeatAll && p->playlist.tail != NULL) {
        printf("  [<< PREV TRACK]: (#%d) '%s' by %s [Repeat-All Wrap]\n",
               p->playlist.tail->song.id, p->playlist.tail->song.title, p->playlist.tail->song.artist);
    } else {
        printf("  [<< PREV TRACK]: (None - Already at start of playlist)\n");
    }

    char dur[10];
    formatDuration(p->currentTrack->song.duration, dur);
    printf("  [>>> NOW PLAYING]: (#%d) '%s' by %s (%s)\n",
           p->currentTrack->song.id, p->currentTrack->song.title, p->currentTrack->song.artist, dur);

    if (p->upNext.front != NULL) {
        printf("  [>> NEXT TRACK]: (#%d) '%s' by %s [Priority: Up Next Queue]\n",
               p->upNext.front->song.id, p->upNext.front->song.title, p->upNext.front->song.artist);
    } else if (p->currentTrack->next != NULL) {
        printf("  [>> NEXT TRACK]: (#%d) '%s' by %s\n",
               p->currentTrack->next->song.id, p->currentTrack->next->song.title, p->currentTrack->next->song.artist);
    } else if (p->repeatAll && p->playlist.head != NULL) {
        printf("  [>> NEXT TRACK]: (#%d) '%s' by %s [Repeat-All Wrap]\n",
               p->playlist.head->song.id, p->playlist.head->song.title, p->playlist.head->song.artist);
    } else {
        printf("  [>> NEXT TRACK]: (None - Reached end of playlist)\n");
    }
    printf("  -------------------------------------------------------------\n");
}

// Plays current track and pushes it onto the history stack
void playCurrent(Player *p) {
    if (p->playlist.head == NULL) {
        printf("  [Player] Playlist is empty. Add songs first.\n");
        return;
    }
    if (p->currentTrack == NULL) {
        p->currentTrack = p->playlist.head;
    }
    p->isPlaying = 1;
    pushStack(&p->history, p->currentTrack->song);
    printf("\n  >> STARTING PLAYBACK:\n");
    printPlaybackStatus(p);
}

// Advances playback to the next song (Queue priority, then DLL or CLL wrap)
void playNext(Player *p) {
    Song nextSong;
    if (dequeue(&p->upNext, &nextSong)) {
        pushStack(&p->history, nextSong);
        DLLNode *found = findSongDLL(&p->playlist, nextSong.id);
        if (found) p->currentTrack = found;
        printf("\n  >> PLAYING FROM UP NEXT QUEUE:\n");
        printPlaybackStatus(p);
        return;
    }
    if (p->currentTrack == NULL) {
        playCurrent(p);
        return;
    }
    if (p->currentTrack->next != NULL) {
        p->currentTrack = p->currentTrack->next;
    } else if (p->repeatAll) {
        p->currentTrack = p->playlist.head; // Looping back to start
        printf("\n  [Repeat-All Active: Wrapped back to start of playlist]\n");
    } else {
        printf("\n  [End of Playlist] Reached last track. (Toggle Repeat-All to loop)\n");
        return;
    }
    pushStack(&p->history, p->currentTrack->song);
    printf("\n  >> ADVANCING TO NEXT TRACK:\n");
    printPlaybackStatus(p);
}

// Navigates to the previous song in the playlist
void playPrev(Player *p) {
    if (p->currentTrack == NULL) {
        playCurrent(p);
        return;
    }
    if (p->currentTrack->prev != NULL) {
        p->currentTrack = p->currentTrack->prev;
    } else if (p->repeatAll) {
        p->currentTrack = p->playlist.tail; // Wrapping backward to tail
        printf("\n  [Repeat-All Active: Wrapped to end of playlist]\n");
    } else {
        printf("\n  [Start of Playlist] Already at first track.\n");
        return;
    }
    pushStack(&p->history, p->currentTrack->song);
    printf("\n  << RETURNING TO PREVIOUS TRACK:\n");
    printPlaybackStatus(p);
}

// Pops history stack to navigate backwards in playback history
void playBack(Player *p) {
    if (p->history.size < 2) {
        printf("  [History] No previous track in history stack to return to.\n");
        return;
    }
    Song currentSong, prevSong;
    popStack(&p->history, &currentSong);
    popStack(&p->history, &prevSong);
    pushStack(&p->history, prevSong);

    DLLNode *node = findSongDLL(&p->playlist, prevSong.id);
    if (node != NULL) p->currentTrack = node;
    printf("\n  << BACK BUTTON (POPPED FROM HISTORY STACK):\n");
    printPlaybackStatus(p);
}

// Toggles Repeat-All mode on and off
void toggleRepeatAll(Player *p) {
    p->repeatAll = !p->repeatAll;
    printf("  [Repeat-All] Mode is now %s (Circular DLL traversal)\n",
           p->repeatAll ? "ENABLED [ON]" : "DISABLED [OFF]");
}

// Adds a song by ID into the Up Next FIFO Queue
int addSongToUpNext(Player *p, int songId) {
    DLLNode *node = findSongDLL(&p->playlist, songId);
    if (node == NULL) {
        printf("  Song with ID %d not found in playlist.\n", songId);
        return 0;
    }
    enqueue(&p->upNext, node->song);
    printf("  Added '%s' to Up Next queue (Position: %d)\n", node->song.title, p->upNext.size);
    return 1;
}

// Dequeues and immediately plays the front song from the Up Next Queue
void playUpNext(Player *p) {
    Song s;
    if (!dequeue(&p->upNext, &s)) {
        printf("  [Up Next] Queue is empty.\n");
        return;
    }
    pushStack(&p->history, s);
    DLLNode *node = findSongDLL(&p->playlist, s.id);
    if (node) p->currentTrack = node;
    printf("  >> PLAYING FROM UP NEXT: '%s' by %s\n", s.title, s.artist);
}

// Removes a song from the playlist by ID
int removeSongFromPlayer(Player *p, int songId) {
    if (p->currentTrack && p->currentTrack->song.id == songId) {
        p->currentTrack = p->currentTrack->next;
    }
    return removeSongDLL(&p->playlist, songId);
}

// Frees all dynamic memory associated with the Player
void freePlayer(Player *p) {
    freeDLL(&p->playlist);
    freeCLL(&p->circularPlaylist);
    freeQueue(&p->upNext);
    freeStack(&p->history);
    freeBST(p->libraryBST);
    p->libraryBST = NULL;
    p->libraryCount = 0;
    p->currentTrack = NULL;
}

// ============================================================================
// Terminal Menu & Live Demo
// ============================================================================

// Clears remaining characters from input buffer
static int readLine(char *buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        return 0; // End-of-file (EOF) or stream closed
    }
    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
        buffer[--len] = '\0';
    }
    return 1;
}

// Reads an integer from stdin safely without buffer leftovers
static int readInt(int *outVal) {
    char buf[64];
    if (!readLine(buf, sizeof(buf))) return 0; // EOF reached
    if (buf[0] == '\0') return -1;             // User pressed enter on empty line
    char *endPtr;
    long val = strtol(buf, &endPtr, 10);
    if (endPtr == buf) return -1;              // Not a valid number
    *outVal = (int)val;
    return 1;
}

// Displays the interactive main menu with full playlist and current track indicator
static void showMenu(const Player *p) {
    printf("\n================================================================================\n");
    printf("                  SMART MEDIA PLAYER (Pure C Data Structures)                   \n");
    printf("================================================================================\n");

    // Playback status showing PREV, NOW PLAYING, and NEXT track
    printPlaybackStatus(p);

    // Full list of songs from the Doubly Linked List with active playing indicator
    printf("\n  [CURRENT PLAYLIST (Doubly Linked List)]:\n");
    displayPlaylistWithCurrent(&p->playlist, p->currentTrack);

    printf("  [Repeat-All: %s | Play History: %d songs | Up Next Queue: %d songs]\n",
           p->repeatAll ? "ON (Circular DLL)" : "OFF", p->history.size, p->upNext.size);
    printf("--------------------------------------------------------------------------------\n");
    printf("  1. Add Song to Playlist         10. Toggle Repeat-All (CLL)\n");
    printf("  2. Remove Song from Playlist    11. Search Title in BST\n");
    printf("  3. Refresh Playlist View        12. Display A-Z Catalog (BST)\n");
    printf("  4. Play / Select Song by ID     13. QuickSort Library\n");
    printf("  5. Next Song (Queue / DLL)      14. MergeSort Library\n");
    printf("  6. Previous Song (DLL)          15. Binary Search (O(log n))\n");
    printf("  7. Back Button (Stack - LIFO)   16. Smart Shuffle (Cooldown 4)\n");
    printf("  8. Add to Up Next Queue (FIFO)  17. RUN FULL LIVE DEMO\n");
    printf("  9. Play from Up Next Queue       0. Exit\n");
    printf("================================================================================\n");
    printf("Select an option (0-17): ");
}

// Handles user input for adding a new song
static void menuAddSong(Player *p) {
    int id, duration;
    char title[MAX_TITLE], artist[MAX_ARTIST];

    printf("Enter Song ID: ");
    if (readInt(&id) <= 0) { printf("  [Error] Invalid Song ID.\n"); return; }

    printf("Enter Song Title: ");
    if (!readLine(title, sizeof(title)) || title[0] == '\0') {
        printf("  [Error] Song Title cannot be empty.\n"); return;
    }

    printf("Enter Artist Name: ");
    if (!readLine(artist, sizeof(artist)) || artist[0] == '\0') {
        printf("  [Error] Artist Name cannot be empty.\n"); return;
    }

    printf("Enter Duration in seconds: ");
    if (readInt(&duration) <= 0 || duration <= 0) {
        printf("  [Error] Invalid duration.\n"); return;
    }

    Song s = createSong(id, title, artist, duration);
    addNewSongToLibrary(p, s);
    printf("  [Success] Song '%s' added successfully!\n", title);
}

// Handles removing a song by ID
static void menuRemoveSong(Player *p) {
    int id;
    printf("Enter Song ID to remove: ");
    if (readInt(&id) <= 0) { printf("  [Error] Invalid Song ID.\n"); return; }

    if (removeSongFromPlayer(p, id)) {
        printf("  [Success] Song ID %d removed from playlist.\n", id);
    } else {
        printf("  [Error] Song ID %d not found in playlist.\n", id);
    }
}

// Handles playing current track or selecting a specific song by ID
static void menuPlay(Player *p) {
    printf("Enter Song ID to play (or press Enter for current): ");
    char buf[64];
    if (readLine(buf, sizeof(buf)) && buf[0] != '\0') {
        int id = atoi(buf);
        if (id > 0) {
            DLLNode *node = findSongDLL(&p->playlist, id);
            if (node != NULL) {
                p->currentTrack = node;
            } else {
                printf("  [Note] Song ID %d not found. Playing current selection.\n", id);
            }
        }
    }
    playCurrent(p);
}

// Handles adding a song to Up Next queue by ID
static void menuAddUpNext(Player *p) {
    int id;
    printf("Enter Song ID to add to Up Next Queue: ");
    if (readInt(&id) <= 0) { printf("  [Error] Invalid Song ID.\n"); return; }
    addSongToUpNext(p, id);
}

// Handles searching for a song in the BST
static void menuSearchBST(Player *p) {
    char query[MAX_TITLE];
    printf("Enter Song Title to search in BST: ");
    if (!readLine(query, sizeof(query)) || query[0] == '\0') {
        printf("  [Error] Search title cannot be empty.\n"); return;
    }

    BSTNode *node = searchBST(p->libraryBST, query);
    if (node != NULL) {
        printf("  [Found in BST]:\n");
        printSong(node->song);
    } else {
        printf("  [Not Found] Song '%s' not found in library BST.\n", query);
    }
}

// Handles sorting the library array using QuickSort or MergeSort
static void menuSort(Player *p, int isMerge) {
    int choice;
    printf("Sort by: 1. Title  2. Artist  3. Duration: ");
    if (readInt(&choice) <= 0 || choice < 1 || choice > 3) {
        printf("  [Error] Invalid choice! Select 1, 2, or 3.\n");
        return;
    }

    if (isMerge) {
        mergeSort(p->libraryArray, 0, p->libraryCount - 1, choice);
        printf("\n  [MergeSort Complete - Sorted by %s]:\n",
               choice == 1 ? "Title" : (choice == 2 ? "Artist" : "Duration"));
    } else {
        quickSort(p->libraryArray, 0, p->libraryCount - 1, choice);
        printf("\n  [QuickSort Complete - Sorted by %s]:\n",
               choice == 1 ? "Title" : (choice == 2 ? "Artist" : "Duration"));
    }
    for (int i = 0; i < p->libraryCount; i++) {
        printSong(p->libraryArray[i]);
    }
}

// Handles O(log n) binary search on sorted array
static void menuBinarySearch(Player *p) {
    char query[MAX_TITLE];
    printf("Enter Title for Binary Search: ");
    if (!readLine(query, sizeof(query)) || query[0] == '\0') {
        printf("  [Error] Title query cannot be empty.\n"); return;
    }

    // Binary search requires the array to be sorted by Title first
    quickSort(p->libraryArray, 0, p->libraryCount - 1, 1);
    int idx = binarySearchSong(p->libraryArray, p->libraryCount, query);
    if (idx != -1) {
        printf("  [Binary Search: FOUND at index %d in O(log n) steps]:\n", idx);
        printSong(p->libraryArray[idx]);
    } else {
        printf("  [Binary Search: NOT FOUND] '%s' is not in library.\n", query);
    }
}

// Handles Smart Shuffle playback
static void menuSmartShuffle(Player *p) {
    if (p->libraryCount == 0) {
        printf("  [Error] Song library is empty.\n");
        return;
    }
    Song pick = smartShufflePick(p->libraryArray, p->libraryCount, &p->history, SMART_SHUFFLE_WINDOW);
    DLLNode *node = findSongDLL(&p->playlist, pick.id);
    if (node) p->currentTrack = node;
    pushStack(&p->history, pick);
    printf("  [Smart Shuffle Pick]: '%s' by %s (Guaranteed not in last %d plays)\n",
           pick.title, pick.artist, SMART_SHUFFLE_WINDOW);
}

// Executes Option 17: Full automated live demo covering every module
static void runLiveDemo(void) {
    Player demo;
    initPlayer(&demo);
    loadDefaultSongs(&demo);

    printf("\n=======================================================\n");
    printf("      STARTING LIVE AUTOMATED DEMO OF ALL MODULES       \n");
    printf("=======================================================\n\n");

    printf("--- 1. Doubly Linked List (Playlist Navigation) ---\n");
    playCurrent(&demo);
    playNext(&demo);
    playNext(&demo);
    playPrev(&demo);

    printf("\n--- 2. Stack (LIFO - Play History & Back Button) ---\n");
    displayStack(&demo.history);
    playBack(&demo);

    printf("\n--- 3. Queue (FIFO - 'Up Next' Priority Playback) ---\n");
    addSongToUpNext(&demo, 8); // Stay
    addSongToUpNext(&demo, 9); // As It Was
    printf("Current Up Next Queue:\n");
    displayQueue(&demo.upNext);
    printf("Playing Next (Queue takes priority over playlist):\n");
    playNext(&demo); // Dequeues Stay
    playNext(&demo); // Dequeues As It Was

    printf("\n--- 4. Circular Doubly Linked List (Repeat-All Mode) ---\n");
    toggleRepeatAll(&demo);
    demo.currentTrack = demo.playlist.tail; // Jump to end of playlist
    printf("At last track: '%s'\n", demo.currentTrack->song.title);
    printf("Calling playNext() at end of playlist with Repeat-All ON:\n");
    playNext(&demo); // Wraps around to playlist head!

    printf("\n--- 5. Binary Search Tree (BST Catalog & Search) ---\n");
    printf("In-order Traversal (A-Z Catalog Listing):\n");
    inorderBST(demo.libraryBST);
    printf("\nSearching for 'Hotel California' in BST:\n");
    BSTNode *bstRes = searchBST(demo.libraryBST, "Hotel California");
    if (bstRes) printSong(bstRes->song);

    printf("\n--- 6. QuickSort & MergeSort Algorithms ---\n");
    printf("QuickSort Library by Artist:\n");
    quickSort(demo.libraryArray, 0, demo.libraryCount - 1, 2);
    for (int i = 0; i < 5; i++) printSong(demo.libraryArray[i]);

    printf("\nMergeSort Library by Duration (Shortest first):\n");
    mergeSort(demo.libraryArray, 0, demo.libraryCount - 1, 3);
    for (int i = 0; i < 5; i++) printSong(demo.libraryArray[i]);

    printf("\n--- 7. Binary Search (O(log n)) ---\n");
    quickSort(demo.libraryArray, 0, demo.libraryCount - 1, 1);
    int bIdx = binarySearchSong(demo.libraryArray, demo.libraryCount, "Starboy");
    printf("Binary Search result for 'Starboy': index %d\n", bIdx);
    if (bIdx != -1) printSong(demo.libraryArray[bIdx]);

    printf("\n--- 8. Smart Shuffle (Sliding Cooldown Window N = 4) ---\n");
    printf("Running 5 Smart Shuffle picks with History Cooldown:\n");
    for (int i = 1; i <= 5; i++) {
        Song pick = smartShufflePick(demo.libraryArray, demo.libraryCount, &demo.history, SMART_SHUFFLE_WINDOW);
        pushStack(&demo.history, pick);
        printf("  Shuffle Pick #%d: '%s' by %s\n", i, pick.title, pick.artist);
    }

    printf("\n=======================================================\n");
    printf("         DATA STRUCTURES & ALGORITHMS SUMMARY          \n");
    printf("=======================================================\n");
    printf("  Module       | Data Structure / Algo | Complexity\n");
    printf("  Playlist     | Doubly Linked List    | O(1) step\n");
    printf("  Repeat-All   | Circular Doubly List  | O(1) wrap\n");
    printf("  Up Next      | FIFO Queue            | O(1) enqueue/dequeue\n");
    printf("  Back Button  | LIFO Stack            | O(1) push/pop\n");
    printf("  Catalogue    | Binary Search Tree    | O(log n) search\n");
    printf("  Sorting      | QuickSort & MergeSort | O(n log n)\n");
    printf("  Lookup       | Binary Search         | O(log n)\n");
    printf("  Shuffle      | Sliding Cooldown      | O(n) window check\n");
    printf("=======================================================\n");
    printf("  LIVE DEMO COMPLETED SUCCESSFULLY!\n\n");

    freePlayer(&demo);
}

// Main function containing the terminal menu loop
int main(void) {
    srand((unsigned int)time(NULL));
    Player player;
    initPlayer(&player);
    loadDefaultSongs(&player);

    int choice = -1;
    while (choice != 0) {
        showMenu(&player);
        int status = readInt(&choice);
        if (status == 0) {
            // Reached EOF on input stream
            printf("\nExiting Smart Media Player. Goodbye!\n");
            break;
        }
        if (status < 0) {
            printf("\nInvalid input! Please enter a number between 0 and 17.\n");
            continue;
        }

        switch (choice) {
            case 1:  menuAddSong(&player); break;
            case 2:  menuRemoveSong(&player); break;
            case 3:  printf("\n--- Current Playlist (DLL) ---\n"); displayDLL(&player.playlist); break;
            case 4:  menuPlay(&player); break;
            case 5:  playNext(&player); break;
            case 6:  playPrev(&player); break;
            case 7:  playBack(&player); break;
            case 8:  menuAddUpNext(&player); break;
            case 9:  playUpNext(&player); break;
            case 10: toggleRepeatAll(&player); break;
            case 11: menuSearchBST(&player); break;
            case 12: printf("\n--- A-Z Catalog (BST In-Order Traversal) ---\n"); inorderBST(player.libraryBST); break;
            case 13: menuSort(&player, 0); break;
            case 14: menuSort(&player, 1); break;
            case 15: menuBinarySearch(&player); break;
            case 16: menuSmartShuffle(&player); break;
            case 17: runLiveDemo(); break;
            case 0:  printf("\nExiting Smart Media Player. Goodbye!\n"); break;
            default: printf("\nInvalid option [%d]! Please select 0 to 17.\n", choice); break;
        }
    }

    freePlayer(&player);
    return 0;
}
