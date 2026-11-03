#include <ctype.h>
#include "models.h"

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

// Compares two songs based on user choice: 1=Title, 2=Artist, 3=Duration
int compareSongs(Song a, Song b, int choice) {
    if (choice == 1) {
        return stringCompareIgnoreCase(a.title, b.title);
    } else if (choice == 2) {
        return stringCompareIgnoreCase(a.artist, b.artist);
    } else {
        // Compare numerically by duration in seconds
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
            Song temp = arr[i]; // Swap smaller element to front
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    Song temp = arr[i + 1];     // Place pivot at its final sorted position
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return i + 1;
}

// QuickSort algorithm to recursively sort song array
void quickSort(Song arr[], int low, int high, int choice) {
    if (low < high) {
        int pi = partition(arr, low, high, choice);
        quickSort(arr, low, pi - 1, choice);  // Sort elements before partition
        quickSort(arr, pi + 1, high, choice); // Sort elements after partition
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
        // '<=' ensures MergeSort remains a stable sorting algorithm
        if (compareSongs(L[i], R[j], choice) <= 0) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// MergeSort algorithm to divide and sort song array
void mergeSort(Song arr[], int left, int right, int choice) {
    if (left < right) {
        int mid = left + (right - left) / 2; // Avoid potential integer overflow
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

        if (cmp == 0) return mid;      // Exact match found at mid index
        else if (cmp < 0) high = mid - 1; // Song title is in left half
        else low = mid + 1;               // Song title is in right half
    }
    return -1; // Song was not found
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

    // Step 1 Window: Take up to windowSize songs from History Stack
    int windowIds[SMART_SHUFFLE_WINDOW];
    int windowCount = getCooldownWindow(history, windowSize, windowIds);

    // Step 2 Check: Collect candidate songs that are OUTSIDE the window
    int candidates[MAX_LIBRARY];
    int candidateCount = 0;
    for (int i = 0; i < poolSize; i++) {
        if (!isIdInCooldown(pool[i].id, windowIds, windowCount)) {
            candidates[candidateCount++] = i;
        }
    }

    // Step 3 Pick: Choose randomly from eligible songs outside the window
    if (candidateCount == 0) return pool[rand() % poolSize]; // Fallback safety
    int pickedIndex = candidates[rand() % candidateCount];
    return pool[pickedIndex];
}
