"""
models.py
Song data model for the Smart Media Player DSA project.
"""

class Song:
    def __init__(self, song_id: int, title: str, artist: str, album: str, duration: int, genre: str = "Pop"):
        """
        :param song_id: Unique integer identifier
        :param title: Song Title
        :param artist: Artist Name
        :param album: Album Name
        :param duration: Duration in seconds (e.g., 215 for 3:35)
        :param genre: Music Genre
        """
        self.id = song_id
        self.title = title
        self.artist = artist
        self.album = album
        self.duration = duration  # in seconds
        self.genre = genre
        self.play_count = 0

    @property
    def formatted_duration(self) -> str:
        """Returns duration formatted as MM:SS."""
        minutes = self.duration // 60
        seconds = self.duration % 60
        return f"{minutes:02d}:{seconds:02d}"

    def increment_play_count(self):
        self.play_count += 1

    def __str__(self) -> str:
        return f"[{self.id}] '{self.title}' by {self.artist} ({self.formatted_duration}) [{self.genre}]"

    def __repr__(self) -> str:
        return f"Song(id={self.id}, title='{self.title}', artist='{self.artist}', duration={self.duration})"

    def to_row(self) -> str:
        return f"{self.id:<4} | {self.title:<24} | {self.artist:<20} | {self.formatted_duration:<8} | {self.genre:<10} | Plays: {self.play_count}"
