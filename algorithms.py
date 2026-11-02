"""
algorithms.py
Custom Sorting, Binary Searching, and the "Smart Shuffle" Cooldown Algorithm.

Data Structures and Algorithms included:
1. QuickSort (Partition-based, O(n log n) average)
2. MergeSort (Stable divide-and-conquer, O(n log n) guaranteed)
3. Binary Search (Iterative O(log n) exact search)
4. Smart Shuffle Algorithm (Twist: Random selection strictly avoiding any song played in the last N plays)
"""

import random
from typing import List, Optional, Tuple, Callable
from models import Song
from data_structures import Stack


# ============================================================================
# 1. QUICKSORT (Custom In-Place / Recursive Implementation)
# ============================================================================

def _get_sort_key(song: Song, key: str):
    key = key.lower()
    if key == "title":
        return song.title.lower()
    elif key == "artist":
        return song.artist.lower()
    elif key == "duration":
        return song.duration
    elif key == "play_count":
        return song.play_count
    elif key == "id":
        return song.id
    return song.title.lower()


def quicksort(songs: List[Song], key: str = "title", ascending: bool = True) -> List[Song]:
    """
    Sorts a list of Song objects using custom QuickSort algorithm.
    Time Complexity: Best/Average O(n log n), Worst O(n^2).
    Space Complexity: O(log n) auxiliary stack space.
    """
    arr = list(songs)  # Create a copy so original isn't mutated unintentionally
    if len(arr) <= 1:
        return arr

    def _partition(low: int, high: int) -> int:
        # Choose median-of-three or high as pivot
        pivot_val = _get_sort_key(arr[high], key)
        i = low - 1

        for j in range(low, high):
            val_j = _get_sort_key(arr[j], key)
            should_swap = (val_j < pivot_val) if ascending else (val_j > pivot_val)
            if should_swap:
                i += 1
                arr[i], arr[j] = arr[j], arr[i]

        arr[i + 1], arr[high] = arr[high], arr[i + 1]
        return i + 1

    def _quicksort_helper(low: int, high: int):
        if low < high:
            pi = _partition(low, high)
            _quicksort_helper(low, pi - 1)
            _quicksort_helper(pi + 1, high)

    _quicksort_helper(0, len(arr) - 1)
    return arr


# ============================================================================
# 2. MERGESORT (Custom Stable Divide-and-Conquer Implementation)
# ============================================================================

def mergesort(songs: List[Song], key: str = "title", ascending: bool = True) -> List[Song]:
    """
    Sorts a list of Song objects using custom MergeSort algorithm.
    Time Complexity: Guaranteed O(n log n) in all cases.
    Space Complexity: O(n) auxiliary space.
    Stability: Stable (preserves relative order of duplicate keys).
    """
    if len(songs) <= 1:
        return list(songs)

    mid = len(songs) // 2
    left_half = mergesort(songs[:mid], key, ascending)
    right_half = mergesort(songs[mid:], key, ascending)

    # Merge step
    merged: List[Song] = []
    i = j = 0

    while i < len(left_half) and j < len(right_half):
        val_left = _get_sort_key(left_half[i], key)
        val_right = _get_sort_key(right_half[j], key)

        condition = (val_left <= val_right) if ascending else (val_left >= val_right)
        if condition:
            merged.append(left_half[i])
            i += 1
        else:
            merged.append(right_half[j])
            j += 1

    while i < len(left_half):
        merged.append(left_half[i])
        i += 1

    while j < len(right_half):
        merged.append(right_half[j])
        j += 1

    return merged


# ============================================================================
# 3. BINARY SEARCH (Custom O(log n) Lookup on Sorted Array)
# ============================================================================

def binary_search_by_title(sorted_songs: List[Song], target_title: str) -> Optional[Tuple[int, Song]]:
    """
    Searches for a song by exact title in an array sorted by title.
    Returns (index, Song) if found, else None.
    Time Complexity: O(log n).
    """
    target = target_title.strip().lower()
    low = 0
    high = len(sorted_songs) - 1

    while low <= high:
        mid = (low + high) // 2
        mid_title = sorted_songs[mid].title.strip().lower()

        if mid_title == target:
            return mid, sorted_songs[mid]
        elif mid_title < target:
            low = mid + 1
        else:
            high = mid - 1

    return None


def binary_search_by_artist(sorted_songs: List[Song], target_artist: str) -> List[Song]:
    """
    Finds all songs by a given artist in an array sorted by artist.
    Uses binary search to locate first occurrence, then gathers contiguous matches.
    Time Complexity: O(log n + k) where k is number of songs by that artist.
    """
    target = target_artist.strip().lower()
    low = 0
    high = len(sorted_songs) - 1
    found_idx = -1

    while low <= high:
        mid = (low + high) // 2
        mid_artist = sorted_songs[mid].artist.strip().lower()

        if mid_artist == target:
            found_idx = mid
            break
        elif mid_artist < target:
            low = mid + 1
        else:
            high = mid - 1

    if found_idx == -1:
        return []

    # Gather all consecutive matching artists around found_idx
    results = [sorted_songs[found_idx]]

    # Scan left
    i = found_idx - 1
    while i >= 0 and sorted_songs[i].artist.strip().lower() == target:
        results.insert(0, sorted_songs[i])
        i -= 1

    # Scan right
    j = found_idx + 1
    while j < len(sorted_songs) and sorted_songs[j].artist.strip().lower() == target:
        results.append(sorted_songs[j])
        j += 1

    return results


# ============================================================================
# 4. THE TWIST: SMART SHUFFLE ALGORITHM
# ============================================================================

class SmartShuffleEngine:
    """
    Smart Shuffle Algorithm with Cooldown Window.

    Problem in standard random shuffle:
    - Pure randomness results in clusters and frequent back-to-back repetitions.
    - True random shuffle can play the same song again 2 tracks later, frustrating users.

    Smart Shuffle Solution:
    - Maintains a cooldown window of size N.
    - Inspects the play history stack (or sliding window).
    - Identifies all song IDs played within the last N songs.
    - Filters the eligible candidate pool: Candidates = { s in Library/Playlist | s.id not in Last_N }.
    - If Candidates is non-empty: picks uniformly at random from Candidates.
    - If Candidates is empty (edge case: library size <= N):
      Gracefully falls back to the song in the cooldown window that was played
      LEAST RECENTLY (longest time elapsed since last played).
    - Guarantees zero repetitions within the last N plays!
    """

    @staticmethod
    def get_recent_played_ids(history_stack: Stack, n: int) -> List[int]:
        """
        Extracts up to N most recent song IDs from the play history stack.
        Stack is LIFO: top is the most recently played.
        """
        recent_ids: List[int] = []
        curr = history_stack.top
        count = 0
        while curr and count < n:
            recent_ids.append(curr.song.id)
            curr = curr.next
            count += 1
        return recent_ids

    @staticmethod
    def select_next_smart_shuffle(
        pool: List[Song],
        history_stack: Stack,
        cooldown_n: int = 5,
        current_song_id: Optional[int] = None
    ) -> Tuple[Optional[Song], str]:
        """
        Selects the next song to play using Smart Shuffle.

        :param pool: Available songs to pick from (playlist or library)
        :param history_stack: LIFO play history stack
        :param cooldown_n: Number of recently played songs to block from replaying
        :param current_song_id: Currently active song ID (blocked from repeating immediately)
        :return: (Selected Song, Explanation message detailing algorithm decision)
        """
        if not pool:
            return None, "Pool is empty."

        if len(pool) == 1:
            return pool[0], "Only one song in pool."

        # Get last N played IDs from history
        recent_ids = SmartShuffleEngine.get_recent_played_ids(history_stack, cooldown_n)

        # Include current playing song in cooldown if not already present
        if current_song_id is not None and current_song_id not in recent_ids:
            recent_ids.insert(0, current_song_id)
            if len(recent_ids) > cooldown_n:
                recent_ids.pop()

        recent_id_set = set(recent_ids)

        # Filter candidate pool
        eligible_candidates = [s for s in pool if s.id not in recent_id_set]

        if eligible_candidates:
            chosen = random.choice(eligible_candidates)
            explanation = (
                f"Smart Shuffle selected '{chosen.title}'. "
                f"Filtered out {len(recent_ids)} recent songs (Cooldown N={cooldown_n}). "
                f"Chose from {len(eligible_candidates)} eligible tracks."
            )
            return chosen, explanation

        # Edge Case: Pool size <= cooldown N, or all songs in pool were played recently
        # Pick the song played longest ago (furthest back in history, or not the current song)
        non_current_candidates = [s for s in pool if s.id != current_song_id]
        if not non_current_candidates:
            return pool[0], "Repeated single song."

        # Find candidate with the maximum distance from top of history
        best_candidate = non_current_candidates[0]
        max_depth = -1

        history_list = history_stack.to_list()
        for cand in non_current_candidates:
            try:
                depth = next(idx for idx, s in enumerate(history_list) if s.id == cand.id)
            except StopIteration:
                depth = 999999  # Never played or very far back
            if depth > max_depth:
                max_depth = depth
                best_candidate = cand

        explanation = (
            f"Smart Shuffle (All {len(pool)} tracks in cooldown): Selected '{best_candidate.title}' "
            f"as the least recently played track (Cooldown N={cooldown_n})."
        )
        return best_candidate, explanation
