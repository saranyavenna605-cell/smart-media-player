#ifndef MODELS_H
#define MODELS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TITLE 50
#define MAX_ARTIST 50
#define MAX_LIBRARY 50
#define SMART_SHUFFLE_WINDOW 4

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

// --- Function Declarations for Data Structures (data_structures.c) ---
Song createSong(int id, const char *title, const char *artist, int duration);
void printSong(Song s);
void formatDuration(int duration, char *buffer);

// DLL Functions
void initDLL(DLL *list);
void addSongDLL(DLL *list, Song s);
int removeSongDLL(DLL *list, int songId);
void displayDLL(const DLL *list);
DLLNode* findSongDLL(const DLL *list, int songId);
void freeDLL(DLL *list);

// CLL Functions
void initCLL(CLL *list);
void addSongCLL(CLL *list, Song s);
void displayCLL(const CLL *list);
void freeCLL(CLL *list);

// Queue Functions
void initQueue(Queue *q);
void enqueue(Queue *q, Song s);
int dequeue(Queue *q, Song *outSong);
void displayQueue(const Queue *q);
int isQueueEmpty(const Queue *q);
void freeQueue(Queue *q);

// Stack Functions
void initStack(Stack *st);
void pushStack(Stack *st, Song s);
int popStack(Stack *st, Song *outSong);
int peekStack(const Stack *st, Song *outSong);
void displayStack(const Stack *st);
int isStackEmpty(const Stack *st);
void freeStack(Stack *st);

// BST Functions
BSTNode* createBSTNode(Song s);
BSTNode* insertBST(BSTNode *root, Song s);
BSTNode* searchBST(BSTNode *root, const char *title);
void inorderBST(BSTNode *root);
void freeBST(BSTNode *root);

// --- Function Declarations for Algorithms (algorithms.c) ---
int stringCompareIgnoreCase(const char *s1, const char *s2);
int compareSongs(Song a, Song b, int choice);
void quickSort(Song arr[], int low, int high, int choice);
void mergeSort(Song arr[], int left, int right, int choice);
int binarySearchSong(Song arr[], int n, const char *title);
Song smartShufflePick(Song pool[], int poolSize, const Stack *history, int windowSize);

// --- Function Declarations for Player Engine (player.c) ---
void initPlayer(Player *p);
void loadDefaultSongs(Player *p);
void playCurrent(Player *p);
void playNext(Player *p);
void playPrev(Player *p);
void playBack(Player *p);
void toggleRepeatAll(Player *p);
int addSongToUpNext(Player *p, int songId);
void playUpNext(Player *p);
void addNewSongToLibrary(Player *p, Song s);
int removeSongFromPlayer(Player *p, int songId);
void freePlayer(Player *p);

#endif // MODELS_H
