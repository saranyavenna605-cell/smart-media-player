"""
test_suite.py
Comprehensive automated test suite verifying all Data Structures & Algorithms
for the Smart Media Player:
- Doubly Linked List (DLL)
- Circular Doubly Linked List (CLL)
- Priority "Up Next" Queue (FIFO)
- Play History Stack (LIFO)
- Binary Search Tree (BST)
- QuickSort & MergeSort
- Binary Search
- Smart Shuffle Cooldown Algorithm (The Twist!)
"""

import unittest
from models import Song
from data_structures import (
    DoublyLinkedList,
    CircularDoublyLinkedList,
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
from player import SmartMediaPlayer


class TestDataStructuresAndAlgorithms(unittest.TestCase):

    def setUp(self):
        self.s1 = Song(1, "Bohemian Rhapsody", "Queen", "A Night at the Opera", 354, "Rock")
        self.s2 = Song(2, "Blinding Lights", "The Weeknd", "After Hours", 200, "Synthpop")
        self.s3 = Song(3, "Hotel California", "Eagles", "Hotel California", 391, "Classic Rock")
        self.s4 = Song(4, "Shape of You", "Ed Sheeran", "Divide", 233, "Pop")
        self.s5 = Song(5, "Billie Jean", "Michael Jackson", "Thriller", 294, "Pop")
        self.sample_songs = [self.s1, self.s2, self.s3, self.s4, self.s5]

    # ------------------------------------------------------------------------
    # 1. Doubly Linked List Tests
    # ------------------------------------------------------------------------
    def test_doubly_linked_list(self):
        dll = DoublyLinkedList()
        self.assertTrue(dll.is_empty())
        self.assertEqual(dll.size(), 0)

        # Append
        n1 = dll.append(self.s1)
        n2 = dll.append(self.s2)
        n3 = dll.append(self.s3)
        self.assertEqual(dll.size(), 3)
        self.assertEqual(dll.head.song.title, "Bohemian Rhapsody")
        self.assertEqual(dll.tail.song.title, "Hotel California")

        # Bidirectional links
        self.assertEqual(n2.prev.song.title, "Bohemian Rhapsody")
        self.assertEqual(n2.next.song.title, "Hotel California")
        self.assertEqual(n3.prev.song.title, "Blinding Lights")
        self.assertIsNone(n3.next)

        # Prepend
        dll.prepend(self.s4)
        self.assertEqual(dll.head.song.title, "Shape of You")
        self.assertEqual(dll.size(), 4)

        # Remove middle
        removed = dll.remove(2) # Blinding Lights
        self.assertTrue(removed)
        self.assertEqual(dll.size(), 3)
        titles = [s.title for s in dll.to_list()]
        self.assertEqual(titles, ["Shape of You", "Bohemian Rhapsody", "Hotel California"])

        # Remove head and tail
        dll.remove(4) # Shape of You
        dll.remove(3) # Hotel California
        self.assertEqual(dll.size(), 1)
        self.assertEqual(dll.head.song.title, "Bohemian Rhapsody")
        self.assertEqual(dll.tail.song.title, "Bohemian Rhapsody")

    # ------------------------------------------------------------------------
    # 2. Circular Doubly Linked List Tests (Repeat-All)
    # ------------------------------------------------------------------------
    def test_circular_doubly_linked_list(self):
        cll = CircularDoublyLinkedList()
        self.assertTrue(cll.is_empty())

        cll.append(self.s1)
        cll.append(self.s2)
        cll.append(self.s3)
        self.assertEqual(cll.size(), 3)

        # Circularity checks
        head = cll.head
        tail = cll.tail
        self.assertEqual(tail.next, head)      # Tail points to Head
        self.assertEqual(head.prev, tail)      # Head points back to Tail

        # Traversing 5 steps forward in a 3-element CLL should wrap around cleanly
        curr = head
        visited = []
        for _ in range(5):
            visited.append(curr.song.id)
            curr = curr.next
        self.assertEqual(visited, [1, 2, 3, 1, 2])

        # Traversing backward wraps around
        curr = head
        curr = curr.prev  # should be tail (s3)
        self.assertEqual(curr.song.id, 3)
        curr = curr.prev  # s2
        self.assertEqual(curr.song.id, 2)

    # ------------------------------------------------------------------------
    # 3. Queue Tests ("Up Next" FIFO)
    # ------------------------------------------------------------------------
    def test_queue_fifo(self):
        q = Queue()
        self.assertTrue(q.is_empty())
        self.assertIsNone(q.dequeue())

        q.enqueue(self.s1)
        q.enqueue(self.s2)
        q.enqueue(self.s3)

        self.assertEqual(q.size(), 3)
        self.assertEqual(q.peek().title, "Bohemian Rhapsody")

        # Dequeue order must strictly follow FIFO
        self.assertEqual(q.dequeue().title, "Bohemian Rhapsody")
        self.assertEqual(q.dequeue().title, "Blinding Lights")
        self.assertEqual(q.size(), 1)
        self.assertEqual(q.dequeue().title, "Hotel California")
        self.assertTrue(q.is_empty())
        self.assertIsNone(q.dequeue())

    # ------------------------------------------------------------------------
    # 4. Stack Tests (History & Back Navigation LIFO)
    # ------------------------------------------------------------------------
    def test_stack_lifo(self):
        st = Stack()
        self.assertTrue(st.is_empty())
        self.assertIsNone(st.pop())

        st.push(self.s1)
        st.push(self.s2)
        st.push(self.s3)

        self.assertEqual(st.size(), 3)
        self.assertEqual(st.peek().title, "Hotel California")

        # Pop order must strictly follow LIFO
        self.assertEqual(st.pop().title, "Hotel California")
        self.assertEqual(st.pop().title, "Blinding Lights")
        self.assertEqual(st.size(), 1)
        self.assertEqual(st.pop().title, "Bohemian Rhapsody")
        self.assertTrue(st.is_empty())
        self.assertIsNone(st.pop())

    # ------------------------------------------------------------------------
    # 5. Binary Search Tree Tests
    # ------------------------------------------------------------------------
    def test_binary_search_tree(self):
        bst = BinarySearchTree()
        for s in self.sample_songs:
            bst.insert(s)

        # Inorder traversal must return alphabetically sorted by title
        inorder = bst.inorder_traversal()
        inorder_titles = [s.title for s in inorder]
        self.assertEqual(
            inorder_titles,
            sorted(["Bohemian Rhapsody", "Blinding Lights", "Hotel California", "Shape of You", "Billie Jean"])
        )

        # Exact Search
        found = bst.search("hotel california")
        self.assertIsNotNone(found)
        self.assertEqual(found.id, 3)

        not_found = bst.search("Non Existent Track")
        self.assertIsNone(not_found)

        # Prefix Search
        b_prefix = bst.search_prefix("b")
        b_titles = sorted([s.title for s in b_prefix])
        self.assertEqual(b_titles, ["Billie Jean", "Blinding Lights", "Bohemian Rhapsody"])

        # Deletion
        del_result = bst.delete("blinding lights")
        self.assertTrue(del_result)
        self.assertIsNone(bst.search("blinding lights"))
        self.assertEqual(bst.size(), 4)

    # ------------------------------------------------------------------------
    # 6. Sorting Algorithms Tests (QuickSort & MergeSort)
    # ------------------------------------------------------------------------
    def test_quicksort_and_mergesort(self):
        # By Title Ascending
        qs_title = quicksort(self.sample_songs, key="title", ascending=True)
        ms_title = mergesort(self.sample_songs, key="title", ascending=True)
        expected_title = sorted(self.sample_songs, key=lambda s: s.title.lower())
        self.assertEqual([s.id for s in qs_title], [s.id for s in expected_title])
        self.assertEqual([s.id for s in ms_title], [s.id for s in expected_title])

        # By Duration Descending
        qs_dur = quicksort(self.sample_songs, key="duration", ascending=False)
        ms_dur = mergesort(self.sample_songs, key="duration", ascending=False)
        expected_dur = sorted(self.sample_songs, key=lambda s: s.duration, reverse=True)
        self.assertEqual([s.id for s in qs_dur], [s.id for s in expected_dur])
        self.assertEqual([s.id for s in ms_dur], [s.id for s in expected_dur])

        # By Artist Ascending
        qs_art = quicksort(self.sample_songs, key="artist", ascending=True)
        ms_art = mergesort(self.sample_songs, key="artist", ascending=True)
        expected_art = sorted(self.sample_songs, key=lambda s: s.artist.lower())
        self.assertEqual([s.id for s in qs_art], [s.id for s in expected_art])
        self.assertEqual([s.id for s in ms_art], [s.id for s in expected_art])

    # ------------------------------------------------------------------------
    # 7. Binary Search Tests
    # ------------------------------------------------------------------------
    def test_binary_search(self):
        sorted_lib = quicksort(self.sample_songs, key="title", ascending=True)
        result = binary_search_by_title(sorted_lib, "Hotel California")
        self.assertIsNotNone(result)
        idx, song = result
        self.assertEqual(song.title, "Hotel California")

        missing = binary_search_by_title(sorted_lib, "Random Track")
        self.assertIsNone(missing)

        # By Artist
        sorted_artist = mergesort(self.sample_songs, key="artist", ascending=True)
        artist_matches = binary_search_by_artist(sorted_artist, "Queen")
        self.assertEqual(len(artist_matches), 1)
        self.assertEqual(artist_matches[0].title, "Bohemian Rhapsody")

    # ------------------------------------------------------------------------
    # 8. The Twist: Smart Shuffle Cooldown Algorithm Verification
    # ------------------------------------------------------------------------
    def test_smart_shuffle_cooldown(self):
        """
        Verify that Smart Shuffle strictly avoids playing any song
        that was played within the last N songs!
        """
        # Create a pool of 10 songs
        pool = [
            Song(i, f"Track {i}", f"Artist {i}", "Album", 180 + i, "Pop")
            for i in range(1, 11)
        ]
        history = Stack()
        cooldown_n = 4

        # Simulate 20 consecutive smart shuffle plays
        curr_song = pool[0]
        history.push(curr_song)

        for play_idx in range(20):
            next_song, explanation = SmartShuffleEngine.select_next_smart_shuffle(
                pool=pool,
                history_stack=history,
                cooldown_n=cooldown_n,
                current_song_id=curr_song.id
            )
            self.assertIsNotNone(next_song)

            # Retrieve the last cooldown_n songs from history
            recent_ids = SmartShuffleEngine.get_recent_played_ids(history, cooldown_n)

            # CRITICAL ASSERTION: The newly picked song ID must NOT be in recent_ids!
            self.assertNotIn(
                next_song.id,
                recent_ids,
                f"Smart Shuffle failed on play {play_idx}! Picked ID {next_song.id} which is in cooldown {recent_ids}"
            )

            # Advance playback
            history.push(next_song)
            curr_song = next_song

    # ------------------------------------------------------------------------
    # 9. SmartMediaPlayer Integration & Priority Order Tests
    # ------------------------------------------------------------------------
    def test_player_precedence_rules(self):
        player = SmartMediaPlayer("TestPlayer")
        player.load_default_library()
        self.assertGreater(len(player.library_list), 10)

        # Rule 1: "Up Next" Queue must take precedence over normal playlist next!
        urgent_song = Song(999, "Priority Track", "VIP", "Exclusive", 120, "Jazz")
        player.add_to_up_next(urgent_song)

        played = player.play_next()
        self.assertEqual(played.id, 999)
        self.assertEqual(played.title, "Priority Track")
        self.assertTrue(player.up_next_queue.is_empty())

        # Test Back button with History Stack
        popped = player.play_prev()
        self.assertIsNotNone(popped)


if __name__ == "__main__":
    unittest.main()
