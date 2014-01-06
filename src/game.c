#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#include <SDL/SDL_ttf.h>
#include "SDL/SDL_image.h"

#include "game.h"

#define SCREEN_BPP      		32

#define FONT_SIZE 16

#define SOURCE_blockSize		48
#define BLOCK_SET_WIDTH 		10
#define BLOCK_SET_HEIGHT		10
#define BLOCK_SET_SIZE  		(BLOCK_SET_WIDTH * BLOCK_SET_HEIGHT)

void drawBlock(SDL_Surface *screen, SDL_Surface *bitmap, int x, int y, int shift, int blocktype, unsigned short blockSize)
{
	if(blocktype >= 0 && blocktype <= BLOCK_SET_SIZE)
	{
		// Ausschnitt aus der Blöckebitmap welche einen Block beinhaltet
		SDL_Rect source;
		source.x = blocktype%BLOCK_SET_WIDTH*blockSize;
		source.y = blocktype/BLOCK_SET_HEIGHT*blockSize;
		source.w = blockSize;
		source.h = blockSize;

		// Ausschnitt aus dem Bildschirminhalt wo der Block gezeichnet werden soll
		SDL_Rect destination;
		destination.x = x*blockSize-shift;
		destination.y = y*blockSize;
		destination.w = blockSize;
		destination.h = blockSize;

		// Block auf den Screen zeichnen
		SDL_BlitSurface(bitmap, &source, screen, &destination);
	}
}

int getBlock(int *world, int worldSizeX, int worldSizeY, int y, int x)
{
	// wenn existierender Block den Typ zurückgeben
	if(x >= 0 && x < worldSizeX && y >= 0 && y < worldSizeY)
	{
		return world[worldSizeX*y+x];
	}
	else
	{
		return -1;
	}
}

int startGame(SDL_Surface *screen, SDL_Event event, struct resolution res, int level)
{
	IMG_Init(IMG_INIT_PNG);

	// Hintergrund
	Uint32 color;
	color = SDL_MapRGB( screen->format, 0, 0, 0 );

	// Lade Hintergrundgrafik
	SDL_Surface *background = SDL_LoadBMP("resources/images/background.bmp");


	int quit = 0;

	// initialisiere die Welt (dies soll eigentlich aus einer Textdatei geladen werden)
	int worldSizeX;
	int worldSizeY;

	char filepath[50];
	sprintf(filepath, "resources/maps/level%d.map", level);
	FILE *worldFile = fopen(filepath, "r");
	if (worldFile == NULL)
		return -1;
	char *line = NULL;
	size_t len = 0;

	// erste Zeile: Größe des Levels
	getline(&line, &len, worldFile);
	char *tok = strtok(line, ",");
	sscanf(tok, "%d", &worldSizeY);
	tok = strtok(NULL, ",");
	sscanf(tok, "%d", &worldSizeX);

	int world[worldSizeY][worldSizeX];
	int column = 0;
	int row = 0;

	// lese restliche Zeilen: das eigentliche Level
	while (row < worldSizeY)
	{
		getline(&line, &len, worldFile);
		printf("Zeile: %s\n", line);
		
		tok = strtok(line, ",");
		column = 0;
		// lese einzelnen Elemente ein
		while (tok)
		{
			printf("Token: %s\n", tok);
			sscanf(tok, "%d", &world[row][column]);
			
			column++;
			tok = strtok(NULL, ",");
		}
		row++;
	}

	int x;
	int y;

	// berechne Blockgröße
	unsigned short blockSize = res.height / worldSizeY;

	// Lade Grafik für die Blöcke
	sprintf(filepath, "resources/images/blocks_%d.png", blockSize);
	SDL_Surface *blockset = IMG_Load(filepath);


	// lade Grafik für den Spieler
	SDL_Surface *player = SDL_LoadBMP("resources/images/player.bmp");

	int playerPositionX = 0;
	int playerPositionY = 0;

	int goLeft = 0;
	int goRight = 0;
	double v = 0; // Vertikalgeschwindigkeit
	double a = 0.6 * ((double) blockSize / 48); // Gravitation


	// Kamera
	int camPositionX = 0;


	// auf SDL-Events warten
	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			// Quit-Event: quit-Flag setzen
			case SDL_QUIT:
			{
				quit = 1;
				break;
			}

			// Timer-Event: Bildschirm neu zeichnen
			case SDL_USEREVENT:
			{
				// Spielerbewegung
				playerPositionX = playerPositionX + (goRight - goLeft) * 9 * ((double) blockSize / 48);

				// Kontrollieren ob Welt verlassen wurde (in horizontaler Richtung)
				if(playerPositionX < 0)
				{playerPositionX = 0;}
				else if(playerPositionX > (worldSizeX-1)*blockSize)
				{playerPositionX = (worldSizeX-1)*blockSize;}


				// X-Collision
				if(getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/blockSize, playerPositionX/blockSize) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%blockSize+blockSize;}
				// verhindere das setzen auf einen Block wenn man gegen diesen springt
				if(playerPositionY%blockSize && getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/blockSize+1, playerPositionX/blockSize) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%blockSize+blockSize;}

				// X-Kollision von der anderen Seite
				if(getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/blockSize, playerPositionX/blockSize+1) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%blockSize;}
				// verhindere das setzen auf einen Block wenn man gegen diesen springt
				if(playerPositionY%blockSize && getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/blockSize+1, playerPositionX/blockSize+1) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%blockSize;}


				// Schwerkraft
				v += a;
				playerPositionY += v;


				// Y-Collision: nicht durch Blöcke hindurchfallen
				if(getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/blockSize+1, playerPositionX/blockSize) >= 0)
				{playerPositionY = playerPositionY-playerPositionY%blockSize;  v = 0;}
				if(playerPositionX%blockSize && getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/blockSize+1, playerPositionX/blockSize+1) >= 0)
				{playerPositionY = playerPositionY-playerPositionY%blockSize; v = 0;}


				// in vertikaler Richtung aus der Welt: zum Start zurücksetzen
				if(playerPositionY < 0-blockSize || playerPositionY > worldSizeY*blockSize)
				{
					return 1;
				}


				// Kamera
				camPositionX += ((playerPositionX - res.width / 2 + blockSize / 2) - camPositionX) / 10;

				if(camPositionX < 0)
				{camPositionX = 0;}
				else if(camPositionX > worldSizeX*blockSize-res.width)
				{camPositionX = worldSizeX*blockSize-res.width;}


				// Lösche den Hintergrund
				SDL_FillRect( screen, NULL, color );


				// Zeichne das Hintergrundbild
				// sichtbarer Ausschnitt des Bildes
				SDL_Rect backgroundSource;
				backgroundSource.x = (int) ( (((double)camPositionX/(worldSizeX*blockSize-res.width))) * (background->clip_rect.w-res.width) );
				backgroundSource.y = 0;
				backgroundSource.w = res.width;
				backgroundSource.h = res.height;

				// zeichne den Ausschnitt auf den gesamten Screen
				SDL_BlitSurface(background, &backgroundSource, screen, NULL);


				// Zeichne alle Blöcke ein
				for(y = 0; y < res.height/blockSize; y++)
				{
					for(x = camPositionX/blockSize; x < camPositionX/blockSize+res.width/blockSize+2; x++)
					{
						drawBlock(screen, blockset, x, y, camPositionX, getBlock(&world[0][0], worldSizeX, worldSizeY, y, x), blockSize);
					}
				}


				// Zeichne den Spieler
				SDL_Rect playerSource;
				playerSource.x = 0;
				playerSource.y = 0;
				playerSource.w = blockSize;
				playerSource.h = blockSize;

				SDL_Rect playerDestination;
				playerDestination.x = playerPositionX-camPositionX;
				playerDestination.y = playerPositionY;
				playerDestination.w = blockSize;
				playerDestination.h = blockSize;

				SDL_BlitSurface(player, &playerSource, screen, &playerDestination);


				// Zeichne das berechnete Bild
				SDL_Flip( screen );
				break;
			}
			// Tasten gedrückt
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					// Pfeil-Links
					case SDLK_LEFT:
						goLeft = 1;
						break;
					// Pfeil-Rechts
					case SDLK_RIGHT:
						goRight = 1;
						break;
					// Leertaster
					case SDLK_SPACE:
						if(v == 0)
							{v = -9 * ((double) blockSize / 48);}
						break;
					// Pfeil hoch
					case SDLK_UP:
						a *= -1;
						break;
					// Escape
					case SDLK_ESCAPE:
						quit = 1;
						break;
					// alles andere ignorieren
					default:
						break;
				}
				break;
			}
			// Taste losgelassen
			case SDL_KEYUP:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_LEFT:
						goLeft = 0;
						break;
					case SDLK_RIGHT:
						goRight = 0;
						break;
					default:
						break;
				}
				break;
			}
		}

		// wenn das quit-Flag gesetzt ist beenden
		if(quit){return 0;}
	}

	return 1;
}
