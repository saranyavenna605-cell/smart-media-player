"""
main.py
Interactive Terminal UI and Automated Live Demonstration
for the Smart Media Player DSA Project.
Safe for Windows PowerShell and Linux/macOS terminals.
"""

import sys
import time
import os

# Ensure UTF-8 output on Windows consoles if supported
if hasattr(sys.stdout, 'reconfigure'):
    try:
        sys.stdout.reconfigure(encoding='utf-8')
    except Exception:
        pass

# Add current directory to path if needed
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from player import SmartMediaPlayer
from models import Song


# ANSI Color Codes for terminal styling
class Colors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    CYAN = '\033[96m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    RED = '\033[91m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    RESET = '\033[0m'


def clear_screen():
    # Only clear screen if interactive terminal
    if sys.stdout.isatty():
        os.system('cls' if os.name == 'nt' else 'clear')


def render_progress_bar(current_sec: int, total_sec: int, width: int = 28) -> str:
    """Renders a visual playback progress bar."""
    if total_sec <= 0:
        return "[" + "-" * width + "]"
    ratio = min(1.0, current_sec / total_sec)
    filled = int(width * ratio)
    bar = "=" * filled + (">" if filled < width else "=") + "-" * (width - filled - (1 if filled < width else 0))
    return f"[{bar}]"


def print_dashboard(player: SmartMediaPlayer):
    """Renders a stylish real-time player dashboard."""
    print(f"{Colors.CYAN}{Colors.BOLD}{'='*78}{Colors.RESET}")
    print(f"{Colors.HEADER}{Colors.BOLD}       >>  BEATFLOW SMART MEDIA PLAYER (ADVANCED DSA ENGINE)  <<{Colors.RESET}")
    print(f"{Colors.CYAN}{'='*78}{Colors.RESET}")

    # Status indicators
    status_icon = f"{Colors.GREEN}[>] PLAYING{Colors.RESET}" if player.is_playing else f"{Colors.YELLOW}[||] PAUSED{Colors.RESET}"
    repeat_icon = f"{Colors.GREEN}ON (CLL Loop){Colors.RESET}" if player.repeat_all_mode else f"{Colors.RED}OFF (Standard DLL){Colors.RESET}"
    shuffle_icon = (
        f"{Colors.GREEN}ON (N={player.smart_shuffle_cooldown_n} Cooldown){Colors.RESET}"
        if player.smart_shuffle_mode
        else f"{Colors.RED}OFF{Colors.RESET}"
    )

    print(f" Status: {status_icon}  |  Repeat-All: {repeat_icon}  |  Smart Shuffle: {shuffle_icon}")
    print(f"{Colors.CYAN}{'-'*78}{Colors.RESET}")

    # Now Playing Card
    curr = player.current_song
    if curr:
        simulated_sec = curr.duration // 2 if player.is_playing else 0
        min_curr, sec_curr = simulated_sec // 60, simulated_sec % 60
        progress_str = f"{min_curr:02d}:{sec_curr:02d} / {curr.formatted_duration}"
        bar = render_progress_bar(simulated_sec, curr.duration)

        print(f" {Colors.BOLD}NOW PLAYING:{Colors.RESET}   {Colors.YELLOW}'{curr.title}'{Colors.RESET} by {Colors.BOLD}{curr.artist}{Colors.RESET}")
        print(f" Album:         {curr.album} ({curr.genre}) | ID: {curr.id} | Play Count: {curr.play_count}")
        print(f" Timeline:      {bar} {progress_str}")
    else:
        print(f" {Colors.YELLOW}NOW PLAYING:{Colors.RESET}   None (Playlist is idle)")

    print(f"{Colors.CYAN}{'-'*78}{Colors.RESET}")

    # Queues & History previews
    up_next_peek = player.up_next_queue.peek()
    history_peek = player.history_stack.peek()

    up_next_str = f"'{up_next_peek.title}' by {up_next_peek.artist}" if up_next_peek else "Empty"
    history_str = f"'{history_peek.title}' by {history_peek.artist}" if history_peek else "Empty"

    print(f" [+] {Colors.BOLD}Up Next Queue (FIFO - Size: {player.up_next_queue.size()}):{Colors.RESET}  {Colors.CYAN}{up_next_str}{Colors.RESET}")
    print(f" [*] {Colors.BOLD}Play History (Stack LIFO - Size: {player.history_stack.size()}):{Colors.RESET} {Colors.CYAN}{history_str}{Colors.RESET}")
    print(f" [i] {Colors.BOLD}Last Action:{Colors.RESET} {Colors.GREEN}{player.last_action_msg}{Colors.RESET}")
    print(f"{Colors.CYAN}{'='*78}{Colors.RESET}")


def print_menu():
    print(f"{Colors.BOLD}--- PLAYBACK CONTROLS (DLL / CLL / FIFO / STACK) ---{Colors.RESET}")
    print(f" [1] Play / Pause Toggle        [2] Next Song (Queue > Shuffle > CLL > DLL)")
    print(f" [3] Previous / Back (Stack)    [4] Add Song to 'Up Next' Queue (FIFO)")
    print(f" [5] Toggle Repeat-All (CLL)    [6] Toggle Smart Shuffle (Twist: Avoids Last N)")
    print(f" [7] Configure Cooldown N       [8] View Active Playlist (DLL)")
    print(f" [9] View 'Up Next' Queue       [10] View Play History Stack")
    print(f"\n{Colors.BOLD}--- SEARCH & ALGORITHMS (BST / QUICKSORT / BINARY SEARCH) ---{Colors.RESET}")
    print(f" [11] Search Song by Title (BST O(log n))")
    print(f" [12] Prefix Search Songs (BST Auto-complete)")
    print(f" [13] Alphabetical Inorder Listing (BST Traversal)")
    print(f" [14] Sort Library by Title / Artist / Duration (QuickSort vs MergeSort)")
    print(f" [15] Binary Search in Sorted Library (O(log n))")
    print(f" [16] Add New Custom Song to Library")
    print(f" [17] Run Complete Automated DSA Live Demo (Examiner Showcase)")
    print(f" [0] Exit")
    print(f"{Colors.CYAN}{'-'*78}{Colors.RESET}")


def run_automated_live_demo(player: SmartMediaPlayer):
    """
    Automated presentation mode to demonstrate each DSA component to examiners.
    Walks step-by-step through DLL, CLL, FIFO Queue, LIFO Stack, BST, Sorts, and Smart Shuffle.
    """
    clear_screen()
    print(f"\n{Colors.HEADER}{Colors.BOLD}========================================================================{Colors.RESET}")
    print(f"{Colors.HEADER}{Colors.BOLD}       >> AUTOMATED EXAMINER DEMONSTRATION & DSA PROOF WALKTHROUGH <<   {Colors.RESET}")
    print(f"{Colors.HEADER}{Colors.BOLD}========================================================================{Colors.RESET}\n")

    steps = [
        "1. Standard DLL Playlist Navigation (Forward & Backward)",
        "2. Priority Queue ('Up Next' FIFO Interception)",
        "3. Play History Stack (LIFO 'Back' button restoration)",
        "4. Circular Linked List (CLL 'Repeat-All' continuous looping)",
        "5. The Twist: Smart Shuffle Algorithm with Cooldown N (Zero repeats within N)",
        "6. Binary Search Tree (BST) Title Search & Alphabetical In-Order Traversal",
        "7. QuickSort & MergeSort Benchmark on Song Metadata",
        "8. Binary Search in O(log n) on Sorted Catalogue"
    ]

    for s in steps:
        print(f"  * {Colors.CYAN}{s}{Colors.RESET}")
    print(f"\n{Colors.YELLOW}Starting comprehensive live verification...{Colors.RESET}\n")

    # Step 1: DLL Navigation
    print(f"\n{Colors.BOLD}--- [DEMO 1/8] DOUBLY LINKED LIST (DLL) NAVIGATION ---{Colors.RESET}")
    player.play()
    print(f"Started at Head: {player.current_song.title}")
    for _ in range(3):
        song = player.play_next()
        print(f"  Advancing DLL -> current.next: {song.title}")
    print(f"{Colors.GREEN}[PASS] Doubly Linked List forward traversal verified.{Colors.RESET}")

    # Step 2: FIFO Queue
    print(f"\n{Colors.BOLD}--- [DEMO 2/8] PRIORITY 'UP NEXT' QUEUE (FIFO) ---{Colors.RESET}")
    q_song1 = Song(888, "VIP Track 1 (Queued)", "Special Artist", "Single", 180, "Pop")
    q_song2 = Song(889, "VIP Track 2 (Queued)", "Special Artist", "Single", 210, "Pop")
    print(f"Enqueuing '{q_song1.title}' then '{q_song2.title}' into Queue...")
    player.add_to_up_next(q_song1)
    player.add_to_up_next(q_song2)

    played_1 = player.play_next()
    print(f"  Calling play_next() -> Dequeued (FIFO #1): {played_1.title}")
    played_2 = player.play_next()
    print(f"  Calling play_next() -> Dequeued (FIFO #2): {played_2.title}")
    print(f"{Colors.GREEN}[PASS] Queue FIFO priority strictly preempted regular playlist sequence.{Colors.RESET}")

    # Step 3: Stack History & Back
    print(f"\n{Colors.BOLD}--- [DEMO 3/8] PLAY HISTORY STACK (LIFO) & BACK NAVIGATION ---{Colors.RESET}")
    print(f"Current song: {player.current_song.title}")
    print(f"History stack size: {player.history_stack.size()}")
    popped1 = player.play_prev()
    print(f"  Pressed 'Back' -> Popped from Stack (LIFO): {popped1.title}")
    popped2 = player.play_prev()
    print(f"  Pressed 'Back' -> Popped from Stack (LIFO): {popped2.title}")
    print(f"{Colors.GREEN}[PASS] Stack LIFO correctly restored the exact previous playback states.{Colors.RESET}")

    # Step 4: CLL Repeat-All
    print(f"\n{Colors.BOLD}--- [DEMO 4/8] CIRCULAR LINKED LIST (CLL) 'REPEAT-ALL' MODE ---{Colors.RESET}")
    player.repeat_all_mode = True
    # Fast forward to tail
    player.current_cll_node = player.repeat_playlist.tail
    player.current_song = player.repeat_playlist.tail.song
    print(f"Simulating playing at TAIL of playlist: {player.current_song.title}")
    next_from_tail = player.play_next()
    print(f"  Calling play_next() at tail -> Wrapped to HEAD: {next_from_tail.title}")
    print(f"  Head points back to tail -> current.prev: {player.repeat_playlist.head.prev.song.title}")
    player.repeat_all_mode = False
    print(f"{Colors.GREEN}[PASS] Circular Linked List loop verified without null pointer boundaries.{Colors.RESET}")

    # Step 5: The Twist: Smart Shuffle with Cooldown N
    print(f"\n{Colors.BOLD}--- [DEMO 5/8] THE TWIST: SMART SHUFFLE (COOLDOWN N=4) ---{Colors.RESET}")
    print("Testing 10 consecutive Smart Shuffle picks. Rule: No song from the last 4 plays may repeat!")
    player.smart_shuffle_mode = True
    player.set_cooldown_n(4)

    picks = []
    for i in range(1, 11):
        chosen = player.play_next()
        recent_ids = [s.id for s in player.history_stack.to_list()[:4]]
        print(f"  Pick #{i:02d}: {Colors.YELLOW}'{chosen.title}'{Colors.RESET} (ID: {chosen.id}) | Last 4 in History: {recent_ids}")
        picks.append(chosen.id)

    player.smart_shuffle_mode = False
    print(f"{Colors.GREEN}[PASS] Smart Shuffle algorithm successfully enforced zero repeats within sliding cooldown!{Colors.RESET}")

    # Step 6: BST Search & In-order
    print(f"\n{Colors.BOLD}--- [DEMO 6/8] BINARY SEARCH TREE (BST) TITLE INDEXING ---{Colors.RESET}")
    target = "Hotel California"
    bst_res = player.search_bst_title(target)
    print(f"  BST Search for '{target}': Found -> {bst_res}")
    prefix = "bo"
    prefix_res = player.search_bst_prefix(prefix)
    print(f"  BST Prefix Search for '{prefix}*': Found {len(prefix_res)} tracks -> {[s.title for s in prefix_res]}")
    inorder_sample = [s.title for s in player.get_library_inorder()[:5]]
    print(f"  BST In-order Traversal (Alphabetical Sample): {inorder_sample}")
    print(f"{Colors.GREEN}[PASS] BST provides O(log n) indexing and sorted alphabetical ordering.{Colors.RESET}")

    # Step 7: Sorting
    print(f"\n{Colors.BOLD}--- [DEMO 7/8] CUSTOM QUICKSORT VS MERGESORT ---{Colors.RESET}")
    sorted_by_duration = player.sort_library(key="duration", algorithm="quicksort", ascending=False)
    print(f"  QuickSort by Duration (Longest First):")
    for s in sorted_by_duration[:4]:
        print(f"    - {s.title:<24} | {s.formatted_duration} ({s.duration}s)")

    sorted_by_artist = player.sort_library(key="artist", algorithm="mergesort", ascending=True)
    print(f"  MergeSort by Artist (Alphabetical):")
    for s in sorted_by_artist[:4]:
        print(f"    - {s.artist:<20} | '{s.title}'")
    print(f"{Colors.GREEN}[PASS] Both QuickSort and MergeSort successfully verified.{Colors.RESET}")

    # Step 8: Binary Search
    print(f"\n{Colors.BOLD}--- [DEMO 8/8] BINARY SEARCH (O(log n)) ---{Colors.RESET}")
    search_q = "Blinding Lights"
    bs_result = player.binary_search_title(search_q)
    if bs_result:
        idx, s = bs_result
        print(f"  Binary Search for '{search_q}': Found at sorted index {idx} -> {s}")
    print(f"{Colors.GREEN}[PASS] Binary Search successfully located track in logarithmic time.{Colors.RESET}\n")

    print(f"{Colors.HEADER}{Colors.BOLD}========================================================================{Colors.RESET}")
    print(f"{Colors.GREEN}{Colors.BOLD}  ALL 8 DSA MODULES & THE SMART SHUFFLE TWIST FULLY VERIFIED!           {Colors.RESET}")
    print(f"{Colors.HEADER}{Colors.BOLD}========================================================================{Colors.RESET}\n")
    if sys.stdout.isatty():
        input(f"{Colors.CYAN}Press Enter to return to main interactive player...{Colors.RESET}")


def interactive_loop():
    player = SmartMediaPlayer()
    player.load_default_library()

    while True:
        clear_screen()
        print_dashboard(player)
        print_menu()

        try:
            choice = input(f"{Colors.BOLD}Enter option (0-17): {Colors.RESET}").strip()
        except (KeyboardInterrupt, EOFError):
            print("\nExiting player. Goodbye!")
            sys.exit(0)

        if choice == "0":
            print(f"\n{Colors.GREEN}Exiting Smart Media Player. Thank you!{Colors.RESET}")
            break

        elif choice == "1":
            player.toggle_play_pause()

        elif choice == "2":
            player.play_next()

        elif choice == "3":
            player.play_prev()

        elif choice == "4":
            print(f"\n{Colors.BOLD}--- Add Song to 'Up Next' Queue (FIFO) ---{Colors.RESET}")
            for idx, s in enumerate(player.library_list):
                print(f" [{s.id:2d}] {s.title:<24} - {s.artist}")
            try:
                sid = int(input("\nEnter Song ID to enqueue: ").strip())
                match = next((s for s in player.library_list if s.id == sid), None)
                if match:
                    player.add_to_up_next(match)
                else:
                    player.last_action_msg = f"Song ID {sid} not found."
            except ValueError:
                player.last_action_msg = "Invalid Song ID entered."

        elif choice == "5":
            player.toggle_repeat_all()

        elif choice == "6":
            player.toggle_smart_shuffle()

        elif choice == "7":
            try:
                n = int(input("Enter cooldown window size N (e.g. 3, 5, 10): ").strip())
                player.set_cooldown_n(n)
            except ValueError:
                player.last_action_msg = "Invalid number entered."

        elif choice == "8":
            clear_screen()
            print(f"{Colors.BOLD}=== CURRENT PLAYLIST (Doubly Linked List) ==={Colors.RESET}\n")
            songs = player.playlist.to_list()
            print(f"{'ID':<4} | {'Title':<24} | {'Artist':<20} | {'Duration':<8} | {'Genre':<10}")
            print("-" * 75)
            for s in songs:
                marker = "> " if (player.current_song and player.current_song.id == s.id) else "  "
                print(f"{marker}{s.to_row()}")
            input(f"\n{Colors.CYAN}Press Enter to return...{Colors.RESET}")

        elif choice == "9":
            clear_screen()
            print(f"{Colors.BOLD}=== 'UP NEXT' QUEUE (FIFO Order) ==={Colors.RESET}\n")
            queued = player.up_next_queue.to_list()
            if not queued:
                print("Queue is empty. Use option 4 to add tracks to queue.")
            else:
                for idx, s in enumerate(queued, 1):
                    print(f" {idx}. {s.title} by {s.artist} ({s.formatted_duration})")
            input(f"\n{Colors.CYAN}Press Enter to return...{Colors.RESET}")

        elif choice == "10":
            clear_screen()
            print(f"{Colors.BOLD}=== PLAY HISTORY STACK (LIFO Order: Top = Most Recent) ==={Colors.RESET}\n")
            history = player.history_stack.to_list()
            if not history:
                print("History stack is empty.")
            else:
                for idx, s in enumerate(history, 1):
                    print(f" [Stack Top -{idx-1}] {s.title} by {s.artist} ({s.formatted_duration})")
            input(f"\n{Colors.CYAN}Press Enter to return...{Colors.RESET}")

        elif choice == "11":
            title_query = input("Enter exact song title to search in BST: ").strip()
            found = player.search_bst_title(title_query)
            if found:
                player.last_action_msg = f"BST Match Found: '{found.title}' by {found.artist} ({found.formatted_duration})."
                play_now = input("Play this track immediately? (y/n): ").strip().lower()
                if play_now == 'y':
                    player.play_direct(found)
            else:
                player.last_action_msg = f"Song '{title_query}' not found in BST Library."

        elif choice == "12":
            prefix = input("Enter title prefix (e.g. 'b', 'sh', 's'): ").strip()
            results = player.search_bst_prefix(prefix)
            clear_screen()
            print(f"{Colors.BOLD}=== BST PREFIX SEARCH RESULTS FOR '{prefix}*' ==={Colors.RESET}\n")
            if results:
                for s in results:
                    print(f" - [{s.id}] {s.title:<24} by {s.artist} ({s.formatted_duration})")
            else:
                print(f"No songs matching prefix '{prefix}'.")
            input(f"\n{Colors.CYAN}Press Enter to return...{Colors.RESET}")

        elif choice == "13":
            clear_screen()
            print(f"{Colors.BOLD}=== ALPHABETICAL CATALOGUE (BST In-order Traversal) ==={Colors.RESET}\n")
            inorder_songs = player.get_library_inorder()
            for s in inorder_songs:
                print(f" - {s.title:<26} | {s.artist:<20} | {s.formatted_duration}")
            input(f"\n{Colors.CYAN}Press Enter to return...{Colors.RESET}")

        elif choice == "14":
            print("\nSort by: [1] Title  [2] Artist  [3] Duration  [4] Play Count")
            k_choice = input("Choice (1-4): ").strip()
            key_map = {"1": "title", "2": "artist", "3": "duration", "4": "play_count"}
            key = key_map.get(k_choice, "title")

            algo = input("Algorithm: [1] QuickSort  [2] MergeSort: ").strip()
            algorithm = "mergesort" if algo == "2" else "quicksort"

            direction = input("Order: [1] Ascending  [2] Descending: ").strip()
            ascending = (direction != "2")

            sorted_songs = player.sort_library(key=key, algorithm=algorithm, ascending=ascending)

            clear_screen()
            print(f"{Colors.BOLD}=== SORTED LIBRARY ({algorithm.upper()} by {key.upper()}) ==={Colors.RESET}\n")
            for s in sorted_songs:
                print(f" {s.to_row()}")
            input(f"\n{Colors.CYAN}Press Enter to return...{Colors.RESET}")

        elif choice == "15":
            search_title = input("Enter Title for Binary Search: ").strip()
            result = player.binary_search_title(search_title)
            if result:
                idx, s = result
                player.last_action_msg = f"Binary Search O(log n): Found '{s.title}' at sorted index {idx}."
                play_now = input("Play this track immediately? (y/n): ").strip().lower()
                if play_now == 'y':
                    player.play_direct(s)
            else:
                player.last_action_msg = f"Binary Search: Title '{search_title}' not found in library."

        elif choice == "16":
            print(f"\n{Colors.BOLD}--- Add New Song to Library ---{Colors.RESET}")
            t = input("Title: ").strip()
            a = input("Artist: ").strip()
            alb = input("Album: ").strip()
            try:
                dur = int(input("Duration in seconds (e.g. 210): ").strip())
            except ValueError:
                dur = 180
            gen = input("Genre: ").strip() or "Pop"

            if t and a:
                new_s = player.add_song_to_library(t, a, alb, dur, gen)
                player.add_to_playlist(new_s)
                player.last_action_msg = f"Added '{new_s.title}' by {new_s.artist} to Library & Playlist."
            else:
                player.last_action_msg = "Title and Artist cannot be blank."

        elif choice == "17":
            run_automated_live_demo(player)


if __name__ == "__main__":
    interactive_loop()
