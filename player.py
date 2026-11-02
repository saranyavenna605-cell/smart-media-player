"""
player.py
SmartMediaPlayer core engine integrating:
- Doubly Linked List (DLL) for sequential playlist (next / prev)
- Circular Doubly Linked List (CLL) for Repeat-All mode
- Queue (FIFO) for "Up Next" priority playback
- Stack (LIFO) for Play History and "Back" navigation
- Binary Search Tree (BST) for fast Title search & Inorder listing
- Sorting (QuickSort / MergeSort) & Binary Search
- Smart Shuffle Engine with Cooldown N
"""

from typing import Optional, List, Tuple
from models import Song
from data_structures import (
    DoublyLinkedList, DLLNode,
    CircularDoublyLinkedList, CLLNode,
    Queue,
    Stack,
    BinarySearchTree
)
from algorithms import (
    quicksort,
    mergesort,
    binary_search_by_title,
    binary_search_by_artist,
    SmartShuffleEngine
)


class SmartMediaPlayer:
    def __init__(self, name: str = "AuraWave Smart Media Player"):
        self.name = name

        # 1. BST for Library Search by Title
        self.library_bst = BinarySearchTree()
        self.library_list: List[Song] = []

        # 2. DLL for Standard Playlist
        self.playlist = DoublyLinkedList()
        self.current_dll_node: Optional[DLLNode] = None

        # 3. CLL for Repeat-All Mode
        self.repeat_playlist = CircularDoublyLinkedList()
        self.current_cll_node: Optional[CLLNode] = None

        # 4. Queue for "Up Next" (FIFO)
        self.up_next_queue = Queue()

        # 5. Stack for History and Back navigation (LIFO)
        self.history_stack = Stack()

        # Playback State
        self.current_song: Optional[Song] = None
        self.is_playing: bool = False
        self.repeat_all_mode: bool = False
        self.smart_shuffle_mode: bool = False
        self.smart_shuffle_cooldown_n: int = 4
        self.last_action_msg: str = "Player Initialized."
        self._next_song_id = 1

    # ========================================================================
    # Library Management
    # ========================================================================

    def add_song_to_library(self, title: str, artist: str, album: str, duration: int, genre: str = "Pop") -> Song:
        """Adds a song to the master library list and indexes it in the BST."""
        song = Song(self._next_song_id, title, artist, album, duration, genre)
        self._next_song_id += 1
        self.library_list.append(song)
        self.library_bst.insert(song)
        return song

    def load_default_library(self):
        """Loads a rich pre-configured library with realistic diverse songs."""
        sample_songs = [
            ("Bohemian Rhapsody", "Queen", "A Night at the Opera", 354, "Rock"),
            ("Blinding Lights", "The Weeknd", "After Hours", 200, "Synthpop"),
            ("Hotel California", "Eagles", "Hotel California", 391, "Classic Rock"),
            ("Shape of You", "Ed Sheeran", "Divide", 233, "Pop"),
            ("Billie Jean", "Michael Jackson", "Thriller", 294, "Pop"),
            ("Starboy", "The Weeknd", "Starboy", 230, "R&B"),
            ("Smells Like Teen Spirit", "Nirvana", "Nevermind", 301, "Grunge"),
            ("Stay", "The Kid LAROI & Justin Bieber", "F*ck Love 3", 141, "Pop"),
            ("As It Was", "Harry Styles", "Harry's House", 167, "Indie Pop"),
            ("Rolling in the Deep", "Adele", "21", 228, "Soul"),
            ("Levitating", "Dua Lipa", "Future Nostalgia", 203, "Disco Pop"),
            ("Take Five", "Dave Brubeck", "Time Out", 324, "Jazz"),
            ("Clair de Lune", "Claude Debussy", "Suite Bergamasque", 302, "Classical"),
            ("Sweet Child O' Mine", "Guns N' Roses", "Appetite for Destruction", 356, "Hard Rock"),
            ("Bad Guy", "Billie Eilish", "When We All Fall Asleep", 194, "Electropop"),
            ("Sunflower", "Post Malone & Swae Lee", "Spider-Man Soundtrack", 158, "Hip-Hop"),
        ]
        for title, artist, album, duration, genre in sample_songs:
            song = self.add_song_to_library(title, artist, album, duration, genre)
            # Add to initial default playlist
            self.add_to_playlist(song)

        self.last_action_msg = f"Loaded {len(sample_songs)} tracks into Library & Playlist."

    # ========================================================================
    # Playlist Management (DLL & CLL Synchronized)
    # ========================================================================

    def add_to_playlist(self, song: Song):
        """Appends a song to both the Doubly Linked List and Circular Linked List."""
        dll_node = self.playlist.append(song)
        cll_node = self.repeat_playlist.append(song)

        # If currently not playing any node, point to first
        if self.current_dll_node is None:
            self.current_dll_node = dll_node
            self.current_cll_node = cll_node
            self.current_song = song

    def add_to_up_next(self, song: Song):
        """Pushes song into FIFO 'Up Next' Queue."""
        self.up_next_queue.enqueue(song)
        self.last_action_msg = f"Enqueued to 'Up Next': '{song.title}'."

    # ========================================================================
    # Playback Controls
    # ========================================================================

    def play(self):
        """Resume or start playing current song."""
        if not self.current_song and self.playlist.head:
            self.current_song = self.playlist.head.song
            self.current_dll_node = self.playlist.head
            self.current_cll_node = self.repeat_playlist.head

        if self.current_song:
            self.is_playing = True
            self.current_song.increment_play_count()
            self.last_action_msg = f"Now Playing: '{self.current_song.title}'."
        else:
            self.last_action_msg = "Playlist is empty. Add songs to play."

    def pause(self):
        """Pause playback."""
        if self.is_playing:
            self.is_playing = False
            self.last_action_msg = f"Paused: '{self.current_song.title if self.current_song else ''}'."
        else:
            self.last_action_msg = "Player already paused."

    def toggle_play_pause(self):
        if self.is_playing:
            self.pause()
        else:
            self.play()

    def play_direct(self, song: Song):
        """Play a specific song directly (e.g. from BST search or sorted list)."""
        # Record previous song to history stack
        if self.current_song:
            self.history_stack.push(self.current_song)

        self.current_song = song
        self.current_song.increment_play_count()
        self.is_playing = True

        # Synchronize pointers in playlist if song exists there
        dll_node = self.playlist.find_node(song.id)
        if dll_node:
            self.current_dll_node = dll_node
            self.current_cll_node = self.repeat_playlist.find_node(song.id)

        self.last_action_msg = f"Directly Playing: '{song.title}' (Pushed previous to History Stack)."

    def play_next(self) -> Optional[Song]:
        """
        Plays the next song following strict DSA precedence rules:
        1. FIFO Queue: Check 'Up Next' queue first! If available, dequeue and play.
        2. Smart Shuffle: If smart shuffle mode is active, select using Smart Shuffle Engine.
        3. Repeat-All CLL: If repeat-all mode is active, advance in Circular Linked List.
        4. Standard DLL: Advance in Doubly Linked List (current.next).
        """
        # Save current song to history stack before transitioning
        if self.current_song:
            self.history_stack.push(self.current_song)

        # Rule 1: Priority FIFO "Up Next" Queue
        if not self.up_next_queue.is_empty():
            next_song = self.up_next_queue.dequeue()
            self.current_song = next_song
            self.current_song.increment_play_count()
            self.is_playing = True
            # Update pointers if song exists in playlist
            self.current_dll_node = self.playlist.find_node(next_song.id)
            self.current_cll_node = self.repeat_playlist.find_node(next_song.id)
            self.last_action_msg = f"Played from 'Up Next' Queue: '{next_song.title}' (Queue remaining: {self.up_next_queue.size()})."
            return next_song

        # Rule 2: Smart Shuffle Mode (The Twist!)
        if self.smart_shuffle_mode:
            pool = self.playlist.to_list() or self.library_list
            next_song, explanation = SmartShuffleEngine.select_next_smart_shuffle(
                pool=pool,
                history_stack=self.history_stack,
                cooldown_n=self.smart_shuffle_cooldown_n,
                current_song_id=self.current_song.id if self.current_song else None
            )
            if next_song:
                self.current_song = next_song
                self.current_song.increment_play_count()
                self.is_playing = True
                self.current_dll_node = self.playlist.find_node(next_song.id)
                self.current_cll_node = self.repeat_playlist.find_node(next_song.id)
                self.last_action_msg = explanation
                return next_song
            else:
                self.last_action_msg = "Smart Shuffle: No eligible songs found."
                return None

        # Rule 3: Repeat-All Mode (Circular Doubly Linked List)
        if self.repeat_all_mode:
            if self.current_cll_node and self.current_cll_node.next:
                self.current_cll_node = self.current_cll_node.next
                self.current_song = self.current_cll_node.song
                self.current_song.increment_play_count()
                self.current_dll_node = self.playlist.find_node(self.current_song.id)
                self.is_playing = True
                self.last_action_msg = f"CLL Loop Next: '{self.current_song.title}' (Repeat-All ON)."
                return self.current_song
            elif self.repeat_playlist.head:
                self.current_cll_node = self.repeat_playlist.head
                self.current_song = self.current_cll_node.song
                self.current_song.increment_play_count()
                self.current_dll_node = self.playlist.find_node(self.current_song.id)
                self.is_playing = True
                self.last_action_msg = f"CLL Loop Next: '{self.current_song.title}' (Repeat-All ON)."
                return self.current_song

        # Rule 4: Standard Sequential DLL Navigation
        if self.current_dll_node and self.current_dll_node.next:
            self.current_dll_node = self.current_dll_node.next
            self.current_song = self.current_dll_node.song
            self.current_song.increment_play_count()
            self.current_cll_node = self.repeat_playlist.find_node(self.current_song.id)
            self.is_playing = True
            self.last_action_msg = f"DLL Next: '{self.current_song.title}'."
            return self.current_song
        else:
            self.is_playing = False
            self.last_action_msg = "Reached end of DLL playlist. Turn on Repeat-All (CLL) or Smart Shuffle to continue."
            return None

    def play_prev(self) -> Optional[Song]:
        """
        Navigates backward:
        - If Stack history is available, pops the last song played (Spotify 'Back' button behavior).
        - Otherwise, moves backward in the Doubly Linked List (current.prev).
        """
        if not self.history_stack.is_empty():
            prev_song = self.history_stack.pop()
            self.current_song = prev_song
            self.current_song.increment_play_count()
            self.is_playing = True
            self.current_dll_node = self.playlist.find_node(prev_song.id)
            self.current_cll_node = self.repeat_playlist.find_node(prev_song.id)
            self.last_action_msg = f"History Stack Back: Popped '{prev_song.title}' (Remaining history: {self.history_stack.size()})."
            return prev_song

        # Fallback to DLL previous node if history stack is empty
        if self.current_dll_node and self.current_dll_node.prev:
            self.current_dll_node = self.current_dll_node.prev
            self.current_song = self.current_dll_node.song
            self.current_song.increment_play_count()
            self.current_cll_node = self.repeat_playlist.find_node(self.current_song.id)
            self.is_playing = True
            self.last_action_msg = f"DLL Previous: '{self.current_song.title}'."
            return self.current_song
        elif self.repeat_all_mode and self.current_cll_node and self.current_cll_node.prev:
            self.current_cll_node = self.current_cll_node.prev
            self.current_song = self.current_cll_node.song
            self.current_song.increment_play_count()
            self.current_dll_node = self.playlist.find_node(self.current_song.id)
            self.is_playing = True
            self.last_action_msg = f"CLL Loop Previous: '{self.current_song.title}' (Repeat-All ON)."
            return self.current_song
        else:
            self.last_action_msg = "Already at the very beginning (History stack empty & no previous DLL node)."
            return None

    # ========================================================================
    # Toggles & Modes
    # ========================================================================

    def toggle_repeat_all(self) -> bool:
        """Toggles Repeat-All mode (Circular Doubly Linked List)."""
        self.repeat_all_mode = not self.repeat_all_mode
        state = "ENABLED (CLL Loop Active)" if self.repeat_all_mode else "DISABLED (Standard DLL)"
        self.last_action_msg = f"Repeat-All {state}."
        return self.repeat_all_mode

    def toggle_smart_shuffle(self) -> bool:
        """Toggles Smart Shuffle mode with cooldown."""
        self.smart_shuffle_mode = not self.smart_shuffle_mode
        state = f"ENABLED (Cooldown N={self.smart_shuffle_cooldown_n})" if self.smart_shuffle_mode else "DISABLED"
        self.last_action_msg = f"Smart Shuffle {state}."
        return self.smart_shuffle_mode

    def set_cooldown_n(self, n: int):
        """Sets the cooldown parameter N for Smart Shuffle."""
        self.smart_shuffle_cooldown_n = max(1, n)
        self.last_action_msg = f"Smart Shuffle Cooldown set to N={self.smart_shuffle_cooldown_n} songs."

    # ========================================================================
    # Search and Sort APIs
    # ========================================================================

    def search_bst_title(self, title: str) -> Optional[Song]:
        """Search title using BST in O(log n)."""
        return self.library_bst.search(title)

    def search_bst_prefix(self, prefix: str) -> List[Song]:
        """Search titles by prefix using BST."""
        return self.library_bst.search_prefix(prefix)

    def get_library_inorder(self) -> List[Song]:
        """Get library in alphabetical order via BST inorder traversal."""
        return self.library_bst.inorder_traversal()

    def sort_library(self, key: str = "title", algorithm: str = "quicksort", ascending: bool = True) -> List[Song]:
        """Sorts the library using QuickSort or MergeSort."""
        if algorithm.lower() == "mergesort":
            return mergesort(self.library_list, key=key, ascending=ascending)
        else:
            return quicksort(self.library_list, key=key, ascending=ascending)

    def binary_search_title(self, target_title: str) -> Optional[Tuple[int, Song]]:
        """Sorts library by title via QuickSort, then performs Binary Search in O(log n)."""
        sorted_lib = quicksort(self.library_list, key="title", ascending=True)
        return binary_search_by_title(sorted_lib, target_title)

    def binary_search_artist(self, target_artist: str) -> List[Song]:
        """Sorts library by artist via MergeSort, then performs Binary Search."""
        sorted_lib = mergesort(self.library_list, key="artist", ascending=True)
        return binary_search_by_artist(sorted_lib, target_artist)
