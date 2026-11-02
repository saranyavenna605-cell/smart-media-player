"""
data_structures.py
Custom Node-Based Data Structures for the Smart Media Player:
1. Doubly Linked List (DLL) - Standard Playlist Navigation (Next / Prev)
2. Circular Doubly Linked List (CLL) - Repeat-All Mode Playlist Loop
3. Queue (FIFO) - "Up Next" Queue (Priority playback)
4. Stack (LIFO) - Play History & "Back" Navigation
5. Binary Search Tree (BST) - Library Indexing by Title with Inorder Traversal & Prefix Search
"""

from typing import Optional, List, Generator
from models import Song


# ============================================================================
# 1. DOUBLY LINKED LIST (DLL) - Playlist Navigation (Previous & Next)
# ============================================================================

class DLLNode:
    """Node for Doubly Linked List."""
    def __init__(self, song: Song):
        self.song = song
        self.prev: Optional['DLLNode'] = None
        self.next: Optional['DLLNode'] = None

    def __repr__(self):
        return f"DLLNode({self.song.title})"


class DoublyLinkedList:
    """
    Doubly Linked List for standard sequential playlist playback.
    Allows bidirectional $O(1)$ navigation (previous and next).
    """
    def __init__(self):
        self.head: Optional[DLLNode] = None
        self.tail: Optional[DLLNode] = None
        self._size: int = 0

    def is_empty(self) -> bool:
        return self.head is None

    def size(self) -> int:
        return self._size

    def append(self, song: Song) -> DLLNode:
        """Insert song at the end of playlist - O(1) time."""
        new_node = DLLNode(song)
        if self.is_empty():
            self.head = new_node
            self.tail = new_node
        else:
            new_node.prev = self.tail
            self.tail.next = new_node
            self.tail = new_node
        self._size += 1
        return new_node

    def prepend(self, song: Song) -> DLLNode:
        """Insert song at the start of playlist - O(1) time."""
        new_node = DLLNode(song)
        if self.is_empty():
            self.head = new_node
            self.tail = new_node
        else:
            new_node.next = self.head
            self.head.prev = new_node
            self.head = new_node
        self._size += 1
        return new_node

    def remove(self, song_id: int) -> bool:
        """Remove a song by its unique ID - O(n) search, O(1) unlinking."""
        curr = self.head
        while curr:
            if curr.song.id == song_id:
                if curr == self.head:
                    self.head = curr.next
                    if self.head:
                        self.head.prev = None
                    else:
                        self.tail = None
                elif curr == self.tail:
                    self.tail = curr.prev
                    if self.tail:
                        self.tail.next = None
                    else:
                        self.head = None
                else:
                    curr.prev.next = curr.next
                    curr.next.prev = curr.prev
                self._size -= 1
                return True
            curr = curr.next
        return False

    def find_node(self, song_id: int) -> Optional[DLLNode]:
        """Find a node containing song with song_id."""
        curr = self.head
        while curr:
            if curr.song.id == song_id:
                return curr
            curr = curr.next
        return None

    def to_list(self) -> List[Song]:
        """Return all songs as a standard list for display/processing."""
        songs = []
        curr = self.head
        while curr:
            songs.append(curr.song)
            curr = curr.next
        return songs

    def clear(self):
        self.head = None
        self.tail = None
        self._size = 0


# ============================================================================
# 2. CIRCULAR DOUBLY LINKED LIST (CLL) - Repeat-All Mode
# ============================================================================

class CLLNode:
    """Node for Circular Doubly Linked List."""
    def __init__(self, song: Song):
        self.song = song
        self.prev: Optional['CLLNode'] = None
        self.next: Optional['CLLNode'] = None

    def __repr__(self):
        return f"CLLNode({self.song.title})"


class CircularDoublyLinkedList:
    """
    Circular Doubly Linked List for 'Repeat-All' mode.
    The last node points forward to the head, and head points back to the tail.
    Guarantees seamless infinite cyclic traversal in both directions.
    """
    def __init__(self):
        self.head: Optional[CLLNode] = None
        self.tail: Optional[CLLNode] = None
        self._size: int = 0

    def is_empty(self) -> bool:
        return self.head is None

    def size(self) -> int:
        return self._size

    def append(self, song: Song) -> CLLNode:
        """Append a song and maintain circular links - O(1) time."""
        new_node = CLLNode(song)
        if self.is_empty():
            self.head = new_node
            self.tail = new_node
            new_node.next = new_node
            new_node.prev = new_node
        else:
            new_node.prev = self.tail
            new_node.next = self.head
            self.tail.next = new_node
            self.head.prev = new_node
            self.tail = new_node
        self._size += 1
        return new_node

    def find_node(self, song_id: int) -> Optional[CLLNode]:
        """Find node by song ID in circular list."""
        if self.is_empty():
            return None
        curr = self.head
        for _ in range(self._size):
            if curr.song.id == song_id:
                return curr
            curr = curr.next
        return None

    def to_list(self) -> List[Song]:
        """Returns non-repeating linear list of songs."""
        if self.is_empty():
            return []
        songs = []
        curr = self.head
        for _ in range(self._size):
            songs.append(curr.song)
            curr = curr.next
        return songs

    def clear(self):
        self.head = None
        self.tail = None
        self._size = 0


# ============================================================================
# 3. QUEUE (FIFO) - "Up Next" Queue
# ============================================================================

class QueueNode:
    """Node for Singly Linked Queue."""
    def __init__(self, song: Song):
        self.song = song
        self.next: Optional['QueueNode'] = None


class Queue:
    """
    FIFO Queue for 'Up Next' playback queue.
    Queued songs have immediate priority over normal playlist tracks.
    Enqueue: O(1), Dequeue: O(1).
    """
    def __init__(self):
        self.front: Optional[QueueNode] = None
        self.rear: Optional[QueueNode] = None
        self._size: int = 0

    def is_empty(self) -> bool:
        return self.front is None

    def size(self) -> int:
        return self._size

    def enqueue(self, song: Song):
        """Add song to the end of the Up Next queue - O(1)."""
        new_node = QueueNode(song)
        if self.rear is None:
            self.front = new_node
            self.rear = new_node
        else:
            self.rear.next = new_node
            self.rear = new_node
        self._size += 1

    def dequeue(self) -> Optional[Song]:
        """Remove and return the next song to play - O(1)."""
        if self.is_empty():
            return None
        popped = self.front.song
        self.front = self.front.next
        if self.front is None:
            self.rear = None
        self._size -= 1
        return popped

    def peek(self) -> Optional[Song]:
        """Inspect the next song in queue without removing it - O(1)."""
        if self.is_empty():
            return None
        return self.front.song

    def to_list(self) -> List[Song]:
        """Return all queued songs in FIFO order."""
        items = []
        curr = self.front
        while curr:
            items.append(curr.song)
            curr = curr.next
        return items

    def clear(self):
        self.front = None
        self.rear = None
        self._size = 0


# ============================================================================
# 4. STACK (LIFO) - Play History and "Back" Navigation
# ============================================================================

class StackNode:
    """Node for Singly Linked Stack."""
    def __init__(self, song: Song):
        self.song = song
        self.next: Optional['StackNode'] = None


class Stack:
    """
    LIFO Stack for Play History and 'Back' button navigation.
    Every played song is pushed onto the stack.
    Pressing 'Back' pops the most recently played song.
    Push: O(1), Pop: O(1), Peek: O(1).
    """
    def __init__(self):
        self.top: Optional[StackNode] = None
        self._size: int = 0

    def is_empty(self) -> bool:
        return self.top is None

    def size(self) -> int:
        return self._size

    def push(self, song: Song):
        """Push a played song onto the history stack - O(1)."""
        new_node = StackNode(song)
        new_node.next = self.top
        self.top = new_node
        self._size += 1

    def pop(self) -> Optional[Song]:
        """Pop the most recently played song to navigate back - O(1)."""
        if self.is_empty():
            return None
        popped = self.top.song
        self.top = self.top.next
        self._size -= 1
        return popped

    def peek(self) -> Optional[Song]:
        """Inspect the last played song without popping - O(1)."""
        if self.is_empty():
            return None
        return self.top.song

    def to_list(self) -> List[Song]:
        """Return history items from most recent to oldest."""
        items = []
        curr = self.top
        while curr:
            items.append(curr.song)
            curr = curr.next
        return items

    def clear(self):
        self.top = None
        self._size = 0


# ============================================================================
# 5. BINARY SEARCH TREE (BST) - Library Indexed by Title
# ============================================================================

class BSTNode:
    """Node for Title-Indexed Binary Search Tree."""
    def __init__(self, song: Song):
        self.song = song
        self.key: str = song.title.strip().lower()
        self.left: Optional['BSTNode'] = None
        self.right: Optional['BSTNode'] = None

    def __repr__(self):
        return f"BSTNode('{self.song.title}')"


class BinarySearchTree:
    """
    Binary Search Tree organizing the entire music library alphabetically by song title.
    Provides:
    - O(log n) average title search
    - In-order traversal yielding alphabetically sorted library
    - Prefix search for autocomplete / real-time search
    - Node deletion with in-order successor handling
    """
    def __init__(self):
        self.root: Optional[BSTNode] = None
        self._size: int = 0

    def size(self) -> int:
        return self._size

    def is_empty(self) -> bool:
        return self.root is None

    def insert(self, song: Song):
        """Insert a song into the BST ordered by title - O(log n) avg."""
        new_node = BSTNode(song)
        if self.root is None:
            self.root = new_node
            self._size += 1
            return

        curr = self.root
        while True:
            if new_node.key < curr.key:
                if curr.left is None:
                    curr.left = new_node
                    self._size += 1
                    break
                curr = curr.left
            elif new_node.key > curr.key:
                if curr.right is None:
                    curr.right = new_node
                    self._size += 1
                    break
                curr = curr.right
            else:
                # Key already exists: handle collision by differentiating with artist or id
                if song.id < curr.song.id:
                    if curr.left is None:
                        curr.left = new_node
                        self._size += 1
                        break
                    curr = curr.left
                else:
                    if curr.right is None:
                        curr.right = new_node
                        self._size += 1
                        break
                    curr = curr.right

    def search(self, title: str) -> Optional[Song]:
        """Exact match search for a song title - O(log n) avg."""
        target = title.strip().lower()
        curr = self.root
        while curr:
            if target == curr.key:
                return curr.song
            elif target < curr.key:
                curr = curr.left
            else:
                curr = curr.right
        return None

    def search_prefix(self, prefix: str) -> List[Song]:
        """Return all songs whose title starts with given prefix."""
        results: List[Song] = []
        target = prefix.strip().lower()

        def _traverse(node: Optional[BSTNode]):
            if not node:
                return
            # If current node starts with prefix, collect it
            if node.key.startswith(target):
                results.append(node.song)
                _traverse(node.left)
                _traverse(node.right)
            elif target < node.key:
                _traverse(node.left)
                # It's possible prefix matches partially on right if node.key starts with prefix,
                # handled above. If target < node.key, right subtree might still have prefix matches
                # only if target is a prefix of something greater, but since target < node.key,
                # node.key doesn't start with target.
            else:
                _traverse(node.right)

        # Full in-order traversal with prefix filter for robustness
        def _inorder_prefix(node: Optional[BSTNode]):
            if not node:
                return
            _inorder_prefix(node.left)
            if node.key.startswith(target):
                results.append(node.song)
            _inorder_prefix(node.right)

        _inorder_prefix(self.root)
        return results

    def inorder_traversal(self) -> List[Song]:
        """Returns all songs in alphabetical order by title - O(n)."""
        result: List[Song] = []

        def _inorder(node: Optional[BSTNode]):
            if node:
                _inorder(node.left)
                result.append(node.song)
                _inorder(node.right)

        _inorder(self.root)
        return result

    def delete(self, title: str) -> bool:
        """Delete a song by title from the BST."""
        target = title.strip().lower()
        deleted = [False]

        def _delete_node(node: Optional[BSTNode], key: str) -> Optional[BSTNode]:
            if node is None:
                return None

            if key < node.key:
                node.left = _delete_node(node.left, key)
            elif key > node.key:
                node.right = _delete_node(node.right, key)
            else:
                deleted[0] = True
                # Case 1: Node with 0 or 1 child
                if node.left is None:
                    return node.right
                elif node.right is None:
                    return node.left

                # Case 2: Node with 2 children - find in-order successor (min in right subtree)
                succ = node.right
                while succ.left:
                    succ = succ.left
                node.song = succ.song
                node.key = succ.key
                node.right = _delete_node(node.right, succ.key)

            return node

        self.root = _delete_node(self.root, target)
        if deleted[0]:
            self._size -= 1
        return deleted[0]

    def height(self) -> int:
        """Calculate the height of the BST."""
        def _h(node: Optional[BSTNode]) -> int:
            if not node:
                return 0
            return 1 + max(_h(node.left), _h(node.right))
        return _h(self.root)
