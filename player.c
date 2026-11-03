#include "models.h"

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

// Plays the current track and pushes it onto the history stack
void playCurrent(Player *p) {
    if (p->playlist.head == NULL) {
        printf("  [Player] Playlist is empty. Add songs first.\n");
        return;
    }
    if (p->currentTrack == NULL) {
        p->currentTrack = p->playlist.head; // Start from head if none selected
    }
    p->isPlaying = 1;
    pushStack(&p->history, p->currentTrack->song);
    printf("\n  >> STARTING PLAYBACK:\n");
    printPlaybackStatus(p);
}

// Advances playback to the next song (Queue priority, then DLL or CLL wrap)
void playNext(Player *p) {
    Song nextSong;
    // Up Next Queue has top priority over regular playlist
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
        p->currentTrack = p->playlist.head; // Circular wrap back to start
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
        p->currentTrack = p->playlist.tail; // Circular wrap backward to tail
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
    popStack(&p->history, &currentSong); // Remove currently playing song
    popStack(&p->history, &prevSong);    // Retrieve previously played song
    pushStack(&p->history, prevSong);    // Put it back as active history
    
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

// Adds a new song to the library array, BST, DLL, and CLL
void addNewSongToLibrary(Player *p, Song s) {
    if (p->libraryCount < MAX_LIBRARY) {
        p->libraryArray[p->libraryCount++] = s;
    }
    p->libraryBST = insertBST(p->libraryBST, s);
    addSongDLL(&p->playlist, s);
    addSongCLL(&p->circularPlaylist, s);
}

// Removes a song from the playlist by ID
int removeSongFromPlayer(Player *p, int songId) {
    if (p->currentTrack && p->currentTrack->song.id == songId) {
        p->currentTrack = p->currentTrack->next; // Advance if playing removed track
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
