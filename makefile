# Makefile by Daniel Stolpmann and Stefan Duebel

# Compiler
CC=gcc

# Flags
CFLAGS=-std=c99 -pedantic -Wall -Wextra

# SDL
SDL=`sdl-config --cflags --libs` -lSDL_ttf -lSDL_mixer -lSDL_image -lm

OBJDIR = bin/obj

all: game

game: init $(OBJDIR)/main.o $(OBJDIR)/game.o $(OBJDIR)/menu.o $(OBJDIR)/image.o $(OBJDIR)/highscore.o $(OBJDIR)/sound.o
	$(CC) $(OBJDIR)/main.o $(OBJDIR)/game.o $(OBJDIR)/menu.o $(OBJDIR)/image.o $(OBJDIR)/highscore.o $(OBJDIR)/sound.o $(CFLAGS) $(SDL) -o bin/game

init:
	mkdir -p bin
	mkdir -p $(OBJDIR)

$(OBJDIR)/main.o: src/main.c
	$(CC) src/main.c $(CFLAGS) $(SDL) -c -o $(OBJDIR)/main.o

$(OBJDIR)/game.o: src/game.c
	$(CC) src/game.c $(CFLAGS) $(SDL) -c -o $(OBJDIR)/game.o

$(OBJDIR)/menu.o: src/menu.c
	$(CC) src/menu.c $(CFLAGS) $(SDL) -c -o $(OBJDIR)/menu.o

$(OBJDIR)/image.o: src/image.c
	$(CC) src/image.c $(CFLAGS) $(SDL) -c -o $(OBJDIR)/image.o

$(OBJDIR)/highscore.o: src/highscore.c
	$(CC) src/highscore.c $(CFLAGS) $(SDL) -c -o $(OBJDIR)/highscore.o

$(OBJDIR)/sound.o: src/sound.c
	$(CC) src/sound.c $(CFLAGS) $(SDL) -c -o $(OBJDIR)/sound.o

clean:
	rm -Rf bin
