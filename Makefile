CC = gcc
CFLAGS = -Wall -Wextra -std=c99

.PHONY: all test clean single

# Default target: builds the main Smart Media Player application
all: smart_media_player

smart_media_player: main.o player.o algorithms.o data_structures.o
	$(CC) $(CFLAGS) -o smart_media_player main.o player.o algorithms.o data_structures.o

# Build standalone single-file version
single: smart_media_player.c
	$(CC) $(CFLAGS) -o smart_media_player smart_media_player.c

# Test target: builds and runs the automated unit test suite
test: test_suite
	./test_suite

test_suite: test_suite.o algorithms.o data_structures.o
	$(CC) $(CFLAGS) -o test_suite test_suite.o algorithms.o data_structures.o

main.o: main.c models.h
	$(CC) $(CFLAGS) -c main.c

player.o: player.c models.h
	$(CC) $(CFLAGS) -c player.c

algorithms.o: algorithms.c models.h
	$(CC) $(CFLAGS) -c algorithms.c

data_structures.o: data_structures.c models.h
	$(CC) $(CFLAGS) -c data_structures.c

test_suite.o: test_suite.c models.h
	$(CC) $(CFLAGS) -c test_suite.c

ifeq ($(OS),Windows_NT)
CLEAN_CMD = -del /f /q *.o smart_media_player.exe test_suite.exe 2>nul
else
CLEAN_CMD = -rm -f *.o smart_media_player smart_media_player.exe test_suite test_suite.exe
endif

clean:
	$(CLEAN_CMD)
