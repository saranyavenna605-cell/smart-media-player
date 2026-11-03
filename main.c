#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "models.h"

// Reads a clean line from stdin, stripping newline characters
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

// Displays the interactive main menu
static void showMenu(const Player *p) {
    printf("\n=======================================================\n");
    printf("        SMART MEDIA PLAYER (C Data Structures)         \n");
    printf("=======================================================\n");
    if (p->currentTrack != NULL) {
        printf("  [Now Playing: (#%d) '%s' | Repeat-All: %s]\n",
               p->currentTrack->song.id, p->currentTrack->song.title, p->repeatAll ? "ON" : "OFF");
    } else {
        printf("  [Status: Idle | Repeat-All: %s]\n", p->repeatAll ? "ON" : "OFF");
    }
    printf("-------------------------------------------------------\n");
    printf("  1. Add Song to Playlist         10. Toggle Repeat-All (CLL)\n");
    printf("  2. Remove Song from Playlist    11. Search Title in BST\n");
    printf("  3. Show Playlist (DLL)          12. Display A-Z Catalog (BST)\n");
    printf("  4. Play / Select Song by ID     13. QuickSort Library\n");
    printf("  5. Next Song (Queue / DLL)      14. MergeSort Library\n");
    printf("  6. Previous Song (DLL)          15. Binary Search (O(log n))\n");
    printf("  7. Back Button (Stack - LIFO)   16. Smart Shuffle (Cooldown 4)\n");
    printf("  8. Add to Up Next Queue (FIFO)  17. RUN FULL LIVE DEMO\n");
    printf("  9. Play from Up Next Queue       0. Exit\n");
    printf("=======================================================\n");
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
