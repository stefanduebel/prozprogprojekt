#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif


#include <SDL/SDL_ttf.h>

#include "game.h"

#define SCREEN_BPP      		32

#define FONT_SIZE 16

#define SOURCE_BLOCK_SIZE		48
#define BLOCK_SET_WIDTH 		10
#define BLOCK_SET_HEIGHT		10
#define BLOCK_SET_SIZE  		(BLOCK_SET_WIDTH * BLOCK_SET_HEIGHT)

void drawBlock(SDL_Surface *screen, SDL_Surface *bitmap, int x, int y, int shift, int blocktype, unsigned short block_size)
{
	if(blocktype >= 0 && blocktype <= BLOCK_SET_SIZE)
	{
		// Ausschnitt aus der Blöckebitmap welche einen Block beinhaltet
		SDL_Rect source;
		source.x = blocktype%BLOCK_SET_WIDTH*block_size;
		source.y = blocktype/BLOCK_SET_HEIGHT*block_size;
		source.w = block_size;
		source.h = block_size;

		// Ausschnitt aus dem Bildschirminhalt wo der Block gezeichnet werden soll
		SDL_Rect destination;
		destination.x = x*block_size-shift;
		destination.y = y*block_size;
		destination.w = block_size;
		destination.h = block_size;

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

int start_game(SDL_Surface *screen, SDL_Event event, struct resolution res, int level)
{
	// Hintergrund
	Uint32 color;
	color = SDL_MapRGB( screen->format, 0, 0, 0 );

	// Lade Hintergrundgrafik
	SDL_Surface *background = SDL_LoadBMP("resources/images/background.bmp");


	int quit = 0;

	// initialisiere die Welt (dies soll eigentlich aus einer Textdatei geladen werden)
	int worldSizeX;
	int worldSizeY;

	char worldFilename[35];
	sprintf(worldFilename, "resources/maps/level%d.map", level);
	printf("worldfile: %s\n", worldFilename);
	if (worldFilename == NULL)
		return -1;
	FILE *world_file = fopen(worldFilename, "r");
	if (world_file == NULL)
		return -1;
	char *line = NULL;
	size_t len = 0;

	// erste Zeile: Größe des Levels
	getline(&line, &len, world_file);
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
		getline(&line, &len, world_file);
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
	unsigned short block_size = res.height / worldSizeY;

	// Lade Grafik für die Blöcke
	char filepath[50];
	sprintf(filepath, "resources/images/blocks_%d.bmp", block_size);
	SDL_Surface *blockset = SDL_LoadBMP(filepath);


	// lade Grafik für den Spieler
	SDL_Surface *player = SDL_LoadBMP("resources/images/player.bmp");

	int playerPositionX = 0;
	int playerPositionY = 0;

	int goLeft = 0;
	int goRight = 0;
	double v = 0; // Vertikalgeschwindigkeit
	double a = 0.6 * (48 / (double) block_size); // Gravitation


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
				playerPositionX = playerPositionX + (goRight - goLeft) * 9;

				// Kontrollieren ob Welt verlassen wurde (in horizontaler Richtung)
				if(playerPositionX < 0)
				{playerPositionX = 0;}
				else if(playerPositionX > (worldSizeX-1)*block_size)
				{playerPositionX = (worldSizeX-1)*block_size;}


				// X-Collision
				if(getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/block_size, playerPositionX/block_size) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%block_size+block_size;}
				// verhindere das setzen auf einen Block wenn man gegen diesen springt
				if(playerPositionY%block_size && getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/block_size+1, playerPositionX/block_size) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%block_size+block_size;}

				// X-Kollision von der anderen Seite
				if(getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/block_size, playerPositionX/block_size+1) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%block_size;}
				// verhindere das setzen auf einen Block wenn man gegen diesen springt
				if(playerPositionY%block_size && getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/block_size+1, playerPositionX/block_size+1) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%block_size;}


				// Schwerkraft
				v += a;
				playerPositionY += v;


				// Y-Collision: nicht durch Blöcke hindurchfallen
				if(getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/block_size+1, playerPositionX/block_size) >= 0)
				{playerPositionY = playerPositionY-playerPositionY%block_size;  v = 0;}
				if(playerPositionX%block_size && getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/block_size+1, playerPositionX/block_size+1) >= 0)
				{playerPositionY = playerPositionY-playerPositionY%block_size; v = 0;}


				// in vertikaler Richtung aus der Welt: zum Start zurücksetzen
				if(playerPositionY < 0-block_size || playerPositionY > worldSizeY*block_size)
				{
					return 1;
				}


				// Kamera
				camPositionX += ((playerPositionX - res.width / 2 + block_size / 2) - camPositionX) / 10;

				if(camPositionX < 0)
				{camPositionX = 0;}
				else if(camPositionX > worldSizeX*block_size-res.width)
				{camPositionX = worldSizeX*block_size-res.width;}


				// Lösche den Hintergrund
				SDL_FillRect( screen, NULL, color );


				// Zeichne das Hintergrundbild
				// sichtbarer Ausschnitt des Bildes
				SDL_Rect background_source;
				background_source.x = (int) ( (((double)camPositionX/(worldSizeX*block_size-res.width))) * (background->clip_rect.w-res.width) );
				background_source.y = 0;
				background_source.w = res.width;
				background_source.h = res.height;

				// zeichne den Ausschnitt auf den gesamten Screen
				SDL_BlitSurface(background, &background_source, screen, NULL);


				// Zeichne alle Blöcke ein
				for(y = 0; y < res.height/block_size; y++)
				{
					for(x = camPositionX/block_size; x < camPositionX/block_size+res.width/block_size+1; x++)
					{
						drawBlock(screen, blockset, x, y, camPositionX, getBlock(&world[0][0], worldSizeX, worldSizeY, y, x), block_size);
					}
				}


				// Zeichne den Spieler
				SDL_Rect player_source;
				player_source.x = 0;
				player_source.y = 0;
				player_source.w = block_size;
				player_source.h = block_size;

				SDL_Rect player_destination;
				player_destination.x = playerPositionX-camPositionX;
				player_destination.y = playerPositionY;
				player_destination.w = block_size;
				player_destination.h = block_size;

				SDL_BlitSurface(player, &player_source, screen, &player_destination);


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
							{v = -9;}
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
