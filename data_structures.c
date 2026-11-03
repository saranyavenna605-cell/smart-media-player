#include "models.h"

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

// ============================================================================
// 1. DOUBLY LINKED LIST (DLL) - Playlist Navigation (Next / Previous)
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
    else list->head = curr->next; // Removing the head node

    if (curr->next != NULL) curr->next->prev = curr->prev;
    else list->tail = curr->prev; // Removing the tail node

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
// 2. CIRCULAR DOUBLY LINKED LIST (CLL) - Repeat-All Mode
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
// 3. QUEUE (FIFO) - "Up Next" Priority Playback Queue
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
// 4. STACK (LIFO) - Play History for the Back Button
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
// 5. BINARY SEARCH TREE (BST) - Library Catalog Keyed by Title
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

    // Compare titles alphabetically ignoring uppercase/lowercase differences
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

// Recursively frees all nodes in the BST (Post-order: Left, Right, Root)
void freeBST(BSTNode *root) {
    if (root == NULL) return;
    freeBST(root->left);
    freeBST(root->right);
    free(root);
}
