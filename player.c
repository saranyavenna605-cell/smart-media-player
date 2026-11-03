#include <math.h>
#include "models.h"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

// Note structure storing frequency in Hertz and duration in seconds
typedef struct {
    double freq;
    double duration;
} AudioNote;

// Generates a compact standard 16-bit PCM WAV audio file for song melodies
static void writeMelodyWav(const char *filename, AudioNote notes[], int count) {
    int sampleRate = 22050; // Compact sample rate for crisp audio
    double totalDur = 0;
    for (int i = 0; i < count; i++) totalDur += notes[i].duration;
    int numSamples = (int)(sampleRate * totalDur);
    int dataSize = numSamples * 2;
    int fileSize = 36 + dataSize;

    FILE *f = fopen(filename, "wb");
    if (!f) return;

    // Standard 44-byte WAV header
    fwrite("RIFF", 1, 4, f);
    fwrite(&fileSize, 4, 1, f);
    fwrite("WAVEfmt ", 1, 8, f);
    int sub1 = 16;
    short fmt = 1, channels = 1;
    fwrite(&sub1, 4, 1, f);
    fwrite(&fmt, 2, 1, f);
    fwrite(&channels, 2, 1, f);
    fwrite(&sampleRate, 4, 1, f);
    int byteRate = sampleRate * 2;
    short align = 2, bits = 16;
    fwrite(&byteRate, 4, 1, f);
    fwrite(&align, 2, 1, f);
    fwrite(&bits, 2, 1, f);
    fwrite("data", 1, 4, f);
    fwrite(&dataSize, 4, 1, f);

    for (int n = 0; n < count; n++) {
        int noteSamples = (int)(sampleRate * notes[n].duration);
        for (int i = 0; i < noteSamples; i++) {
            double t = (double)i / sampleRate;
            double env = 1.0;
            if (i < 250) env = (double)i / 250.0; // Attack ramp
            else if (i > noteSamples - 500) env = (double)(noteSamples - i) / 500.0; // Decay
            
            // Harmonic synth: fundamental frequency + 2nd overtone for a richer timbre
            double val = sin(2.0 * 3.1415926535 * notes[n].freq * t) +
                         0.25 * sin(4.0 * 3.1415926535 * notes[n].freq * t);
            short sample = (short)(22000.0 * val * env);
            fwrite(&sample, 2, 1, f);
        }
    }
    fclose(f);
}

// Initializes the music folder and pre-generates melodic WAV files on startup
void initAudioEngine(void) {
#ifdef _WIN32
    CreateDirectory("music", NULL);
    FILE *chkMp3 = fopen("music/4.mp3", "rb");
    if (!chkMp3) chkMp3 = fopen("smart_media_player/music/4.mp3", "rb");
    if (chkMp3) { fclose(chkMp3); return; } // Studio MP3 library detected

    FILE *chk = fopen("music/1.wav", "rb");
    if (chk) { fclose(chk); return; } // Already generated

    AudioNote s1[]  = { {466.16, 0.3}, {349.23, 0.3}, {293.66, 0.3}, {311.13, 0.3}, {349.23, 0.3}, {293.66, 0.3}, {233.08, 0.6} };
    AudioNote s2[]  = { {698.46, 0.18}, {622.25, 0.18}, {698.46, 0.22}, {783.99, 0.25}, {698.46, 0.18}, {523.25, 0.22}, {622.25, 0.22}, {698.46, 0.35} };
    AudioNote s3[]  = { {493.88, 0.25}, {369.99, 0.25}, {293.66, 0.25}, {440.00, 0.25}, {329.63, 0.25}, {277.18, 0.25}, {246.94, 0.50} };
    AudioNote s4[]  = { {554.37, 0.2}, {659.25, 0.2}, {554.37, 0.2}, {554.37, 0.2}, {659.25, 0.2}, {554.37, 0.2}, {739.99, 0.3}, {659.25, 0.4} };
    AudioNote s5[]  = { {185.00, 0.25}, {277.18, 0.25}, {329.63, 0.25}, {369.99, 0.25}, {329.63, 0.25}, {277.18, 0.25}, {246.94, 0.25}, {277.18, 0.35} };
    AudioNote s6[]  = { {392.00, 0.2}, {392.00, 0.2}, {349.23, 0.2}, {293.66, 0.25}, {349.23, 0.2}, {392.00, 0.3}, {349.23, 0.2}, {293.66, 0.4} };
    AudioNote s7[]  = { {349.23, 0.25}, {349.23, 0.25}, {466.16, 0.25}, {466.16, 0.25}, {415.30, 0.25}, {554.37, 0.45} };
    AudioNote s8[]  = { {523.25, 0.22}, {587.33, 0.22}, {659.25, 0.25}, {783.99, 0.3}, {659.25, 0.22}, {587.33, 0.22}, {523.25, 0.4} };
    AudioNote s9[]  = { {440.00, 0.22}, {493.88, 0.22}, {554.37, 0.25}, {659.25, 0.3}, {554.37, 0.22}, {493.88, 0.22}, {440.00, 0.4} };
    AudioNote s10[] = { {261.63, 0.25}, {261.63, 0.25}, {311.13, 0.3}, {392.00, 0.3}, {349.23, 0.25}, {311.13, 0.25}, {261.63, 0.5} };

    writeMelodyWav("music/1.wav", s1, 7);
    writeMelodyWav("music/2.wav", s2, 8);
    writeMelodyWav("music/3.wav", s3, 7);
    writeMelodyWav("music/4.wav", s4, 8);
    writeMelodyWav("music/5.wav", s5, 8);
    writeMelodyWav("music/6.wav", s6, 8);
    writeMelodyWav("music/7.wav", s7, 6);
    writeMelodyWav("music/8.wav", s8, 7);
    writeMelodyWav("music/9.wav", s9, 7);
    writeMelodyWav("music/10.wav", s10, 7);
#endif
}

// Stops all currently playing audio
void stopSongAudio(void) {
#ifdef _WIN32
    PlaySound(NULL, 0, 0);
    mciSendString("close all", NULL, 0, NULL);
#endif
}

// Helper to locate audio file across standard relative paths
static int findAudioFile(int id, const char *ext, char *outPath, int maxLen) {
    snprintf(outPath, maxLen, "music/%d.%s", id, ext);
    FILE *f = fopen(outPath, "rb");
    if (f) { fclose(f); return 1; }

    snprintf(outPath, maxLen, "smart_media_player/music/%d.%s", id, ext);
    f = fopen(outPath, "rb");
    if (f) { fclose(f); return 1; }
    return 0;
}

// Plays song audio live through laptop speakers or bluetooth earpods
void playSongAudio(int songId, const char *title) {
    (void)title;
#ifdef _WIN32
    stopSongAudio();

    // Check if studio MP3 exists in music folder
    char mp3Path[256];
    if (findAudioFile(songId, "mp3", mp3Path, sizeof(mp3Path))) {
        char cmd[300];
        sprintf(cmd, "open \"%s\" type mpegvideo alias current_song", mp3Path);
        mciSendString(cmd, NULL, 0, NULL);
        mciSendString("play current_song", NULL, 0, NULL);
        printf("  ♪ [Audio Output]: Playing real studio MP3 track live through laptop/earpods!\n");
        printf("    Track File  : %s\n", mp3Path);
        printf("    Visualizer  : [ █ ▄ █ ▆ ▃ █ ▇ █ ▆ ▅ █ ] Real Studio Audio Active\n");
        return;
    }

    // Play signature acoustic melody WAV asynchronously as fallback
    char wavPath[256];
    if (findAudioFile(songId, "wav", wavPath, sizeof(wavPath))) {
        PlaySound(wavPath, NULL, SND_ASYNC | SND_FILENAME);
        printf("  ♪ [Audio Output]: Playing acoustic melody through laptop/earpods!\n");
        printf("    Visualizer  : [ █ ▄ █ ▆ ▃ █ ▇ █ ] Active Sound Waveform\n");
    }
#else
    (void)songId; (void)title;
#endif
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
    initAudioEngine();
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
        createSong(8, "Believer", "Imagine Dragons", 204),
        createSong(9, "Perfect", "Ed Sheeran", 263),
        createSong(10, "Rolling in the Deep", "Adele", 228)
    };
    for (int i = 0; i < 10; i++) {
        addNewSongToLibrary(p, list[i]);
    }
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
    playSongAudio(p->currentTrack->song.id, p->currentTrack->song.title);
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
        playSongAudio(p->currentTrack->song.id, p->currentTrack->song.title);
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
    playSongAudio(p->currentTrack->song.id, p->currentTrack->song.title);
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
    playSongAudio(p->currentTrack->song.id, p->currentTrack->song.title);
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
    playSongAudio(p->currentTrack->song.id, p->currentTrack->song.title);
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
    printf("\n  >> PLAYING FROM UP NEXT QUEUE:\n");
    printPlaybackStatus(p);
    playSongAudio(p->currentTrack->song.id, p->currentTrack->song.title);
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
        p->currentTrack = p->currentTrack->next;
    }
    return removeSongDLL(&p->playlist, songId);
}

// Frees all dynamic memory associated with the Player
void freePlayer(Player *p) {
    stopSongAudio();
    freeDLL(&p->playlist);
    freeCLL(&p->circularPlaylist);
    freeQueue(&p->upNext);
    freeStack(&p->history);
    freeBST(p->libraryBST);
    p->libraryBST = NULL;
    p->libraryCount = 0;
    p->currentTrack = NULL;
}
