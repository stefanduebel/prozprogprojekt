# Makefile by Daniel Stolpmann and Stefan Duebel

# Compiler
CC=gcc

# Flags
CFLAGS=-std=c99 -pedantic -Wall -Wextra

# SDL
SDL=`sdl-config --cflags --libs` -lSDL_ttf -lm
 
%:
	mkdir -p bin
	mkdir -p bin/$(dir $@)
	$(CC) $@.c $(CFLAGS) $(SDL) -o bin/$@

all: main
	
clean:
	rm -Rf bin
