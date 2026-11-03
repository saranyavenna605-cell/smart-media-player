#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "models.h"

// Tests Doubly Linked List operations: insertion, traversal, deletion
void test_dll(void) {
    DLL list;
    initDLL(&list);
    assert(list.head == NULL && list.tail == NULL && list.size == 0);

    Song s1 = createSong(1, "Song A", "Artist 1", 180);
    Song s2 = createSong(2, "Song B", "Artist 2", 200);
    Song s3 = createSong(3, "Song C", "Artist 3", 220);

    addSongDLL(&list, s1);
    addSongDLL(&list, s2);
    addSongDLL(&list, s3);
    assert(list.size == 3);
    assert(list.head->song.id == 1);
    assert(list.tail->song.id == 3);

    // Verify bidirectional linking
    assert(list.head->next->song.id == 2);
    assert(list.tail->prev->song.id == 2);
    assert(list.head->next->prev == list.head);

    // Test removing middle element
    int removed = removeSongDLL(&list, 2);
    assert(removed == 1);
    assert(list.size == 2);
    assert(list.head->next == list.tail);
    assert(list.tail->prev == list.head);

    freeDLL(&list);
    assert(list.head == NULL && list.size == 0);
    printf("  [PASS] Doubly Linked List (DLL) tests passed!\n");
}

// Tests Circular Doubly Linked List operations: cyclic wrapping
void test_cll(void) {
    CLL list;
    initCLL(&list);
    assert(list.head == NULL && list.size == 0);

    Song s1 = createSong(1, "Track 1", "Artist 1", 100);
    Song s2 = createSong(2, "Track 2", "Artist 2", 200);

    addSongCLL(&list, s1);
    assert(list.head->next == list.head); // 1 node points to itself
    assert(list.head->prev == list.head);

    addSongCLL(&list, s2);
    assert(list.size == 2);
    assert(list.head->next->song.id == 2);
    assert(list.head->next->next == list.head); // Circular wrap next
    assert(list.head->prev->song.id == 2);       // Circular wrap prev

    freeCLL(&list);
    assert(list.head == NULL && list.size == 0);
    printf("  [PASS] Circular Doubly Linked List (CLL) tests passed!\n");
}

// Tests Queue operations: FIFO ordering, enqueue, dequeue, empty check
void test_queue(void) {
    Queue q;
    initQueue(&q);
    assert(isQueueEmpty(&q) == 1);

    Song s1 = createSong(10, "First", "Artist", 120);
    Song s2 = createSong(20, "Second", "Artist", 150);

    enqueue(&q, s1);
    enqueue(&q, s2);
    assert(q.size == 2);
    assert(isQueueEmpty(&q) == 0);

    Song out;
    int ok = dequeue(&q, &out);
    assert(ok == 1 && out.id == 10); // First-in must be first-out

    ok = dequeue(&q, &out);
    assert(ok == 1 && out.id == 20);
    assert(isQueueEmpty(&q) == 1);

    ok = dequeue(&q, &out); // Queue underflow check
    assert(ok == 0);

    freeQueue(&q);
    printf("  [PASS] Queue (FIFO - Up Next) tests passed!\n");
}

// Tests Stack operations: LIFO ordering, push, pop, peek, empty check
void test_stack(void) {
    Stack st;
    initStack(&st);
    assert(isStackEmpty(&st) == 1);

    Song s1 = createSong(1, "Track One", "Artist", 100);
    Song s2 = createSong(2, "Track Two", "Artist", 200);

    pushStack(&st, s1);
    pushStack(&st, s2);
    assert(st.size == 2);

    Song topSong;
    int peekOk = peekStack(&st, &topSong);
    assert(peekOk == 1 && topSong.id == 2); // Most recent is at top

    Song popped;
    int popOk = popStack(&st, &popped);
    assert(popOk == 1 && popped.id == 2); // Last-in must be first-out

    popOk = popStack(&st, &popped);
    assert(popOk == 1 && popped.id == 1);
    assert(isStackEmpty(&st) == 1);

    freeStack(&st);
    printf("  [PASS] Stack (LIFO - History) tests passed!\n");
}

// Tests Binary Search Tree: insertion and search by title
void test_bst(void) {
    BSTNode *root = NULL;
    Song s1 = createSong(1, "Hotel California", "Eagles", 391);
    Song s2 = createSong(2, "Bohemian Rhapsody", "Queen", 354);
    Song s3 = createSong(3, "Starboy", "The Weeknd", 230);

    root = insertBST(root, s1);
    root = insertBST(root, s2);
    root = insertBST(root, s3);

    // Search existing song
    BSTNode *found = searchBST(root, "Bohemian Rhapsody");
    assert(found != NULL);
    assert(found->song.id == 2);

    // Search case-insensitive
    found = searchBST(root, "hotel california");
    assert(found != NULL && found->song.id == 1);

    // Search non-existing song
    found = searchBST(root, "Non Existing Title");
    assert(found == NULL);

    freeBST(root);
    printf("  [PASS] Binary Search Tree (BST) tests passed!\n");
}

// Tests QuickSort and MergeSort on song arrays
void test_sorting(void) {
    Song arr[4] = {
        createSong(1, "Zebra", "B Artist", 300),
        createSong(2, "Alpha", "A Artist", 120),
        createSong(3, "Beta", "C Artist", 250),
        createSong(4, "Delta", "A Artist", 180)
    };

    // Test QuickSort by Title (choice = 1)
    quickSort(arr, 0, 3, 1);
    assert(strcmp(arr[0].title, "Alpha") == 0);
    assert(strcmp(arr[3].title, "Zebra") == 0);

    // Test MergeSort by Duration (choice = 3)
    mergeSort(arr, 0, 3, 3);
    assert(arr[0].duration == 120);
    assert(arr[1].duration == 180);
    assert(arr[2].duration == 250);
    assert(arr[3].duration == 300);

    printf("  [PASS] QuickSort & MergeSort tests passed!\n");
}

// Tests Binary Search in sorted array: O(log n)
void test_binary_search(void) {
    Song sorted[4] = {
        createSong(1, "Alpha", "Artist", 100),
        createSong(2, "Beta", "Artist", 200),
        createSong(3, "Delta", "Artist", 300),
        createSong(4, "Gamma", "Artist", 400)
    };

    int idx = binarySearchSong(sorted, 4, "Beta");
    assert(idx == 1);

    idx = binarySearchSong(sorted, 4, "beta"); // Case-insensitive lookup
    assert(idx == 1);

    idx = binarySearchSong(sorted, 4, "Omega"); // Not present
    assert(idx == -1);

    printf("  [PASS] Binary Search (O(log n)) tests passed!\n");
}

// Tests Smart Shuffle: guarantees no song inside cooldown window of 4 is selected
void test_smart_shuffle(void) {
    srand(42); // Fixed seed for reproducible test
    Song pool[6] = {
        createSong(1, "Song 1", "Artist", 100),
        createSong(2, "Song 2", "Artist", 100),
        createSong(3, "Song 3", "Artist", 100),
        createSong(4, "Song 4", "Artist", 100),
        createSong(5, "Song 5", "Artist", 100),
        createSong(6, "Song 6", "Artist", 100)
    };

    Stack history;
    initStack(&history);
    // Push 4 songs to history (IDs 1, 2, 3, 4 are in cooldown)
    pushStack(&history, pool[0]); // ID 1
    pushStack(&history, pool[1]); // ID 2
    pushStack(&history, pool[2]); // ID 3
    pushStack(&history, pool[3]); // ID 4

    // Run Smart Shuffle 30 times: must ONLY pick song ID 5 or 6!
    for (int i = 0; i < 30; i++) {
        Song pick = smartShufflePick(pool, 6, &history, SMART_SHUFFLE_WINDOW);
        assert(pick.id == 5 || pick.id == 6);
    }

    // Test relaxation when pool <= window
    Song smallPool[2] = { pool[0], pool[1] };
    Song pickSmall = smartShufflePick(smallPool, 2, &history, SMART_SHUFFLE_WINDOW);
    assert(pickSmall.id == 1 || pickSmall.id == 2);

    freeStack(&history);
    printf("  [PASS] Smart Shuffle Cooldown Algorithm tests passed!\n");
}

// Main test entry point running all test functions
int main(void) {
    printf("\n=======================================================\n");
    printf("   RUNNING SMART MEDIA PLAYER DATA STRUCTURE TEST SUITE\n");
    printf("=======================================================\n\n");

    test_dll();
    test_cll();
    test_queue();
    test_stack();
    test_bst();
    test_sorting();
    test_binary_search();
    test_smart_shuffle();

    printf("\n-------------------------------------------------------\n");
    printf("  ALL 8 TEST SUITES COMPLETED SUCCESSFULLY! (8/8 PASS)\n");
    printf("=======================================================\n\n");
    return 0;
}
