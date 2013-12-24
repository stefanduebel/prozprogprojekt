#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#include <SDL/SDL_ttf.h>

#include "game.h"

#define SCREEN_WIDTH    		1280
#define SCREEN_HEIGHT   		720
#define SCREEN_BPP      		32

#define FONT_SIZE 16

#define BLOCK_SIZE      		48
#define BLOCK_SET_WIDTH 		10
#define BLOCK_SET_HEIGHT		10
#define BLOCK_SET_SIZE  		(BLOCK_SET_WIDTH * BLOCK_SET_HEIGHT)

void drawBlock(SDL_Surface *screen, SDL_Surface *bitmap, int x, int y, int shift, int blocktype)
{
	if(blocktype >= 0 && blocktype <= BLOCK_SET_SIZE)
	{
		SDL_Rect source;
		source.x = blocktype%BLOCK_SET_WIDTH*BLOCK_SIZE;
		source.y = blocktype/BLOCK_SET_HEIGHT*BLOCK_SIZE;
		source.w = BLOCK_SIZE;
		source.h = BLOCK_SIZE;

		SDL_Rect destination;
		destination.x = x*BLOCK_SIZE-shift;
		destination.y = y*BLOCK_SIZE;
		destination.w = BLOCK_SIZE;
		destination.h = BLOCK_SIZE;

		SDL_BlitSurface(bitmap, &source, screen, &destination);
	}
}

int getBlock(int *world, int worldSizeX, int worldSizeY, int y, int x)
{
	if(x >= 0 && x < worldSizeX && y >= 0 && y < worldSizeY)
	{
		return world[worldSizeX*y+x];
	}
	else
	{
		return -1;
	}
}

int start_game(SDL_Surface *screen, SDL_Event event)
{
	// Hintergrund
	Uint32 color;
	color = SDL_MapRGB( screen->format, 0, 0, 0 );

	SDL_Surface *background = SDL_LoadBMP("resources/images/background.bmp");
	int backgroundWidth = 2560;
	int backgroundHeight = 720;

	int quit = 0;


	// Welt
	SDL_Surface *blockset = SDL_LoadBMP("resources/images/test.bmp");

	int worldSizeX = 40;
	int worldSizeY = 15;
	int world[15][40] =
	{{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 {-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 1, 2, 3, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 { 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 { 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 { 0, 0,-1,-1,-1,-1,-1,-1, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 { 0, 0, 0,-1,-1,-1, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
	 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1,-1,-1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0},
	 { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0}};

	int x;
	int y;


	// Spieler
	SDL_Surface *player = SDL_LoadBMP("resources/images/player.bmp");

	int playerPositionX = 0;
	int playerPositionY = 0;

	int goLeft = 0;
	int goRight = 0;
	double v = 0;
	double a = 0.6;


	// Kamera
	int camPositionX = 0;


	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				quit = 1;
				break;
			}
			case SDL_USEREVENT:
			{
				// Spielerbewegung
				playerPositionX = playerPositionX + (goRight - goLeft) * 9;

				if(playerPositionX < 0)
				{playerPositionX = 0;}
				else if(playerPositionX > (worldSizeX-1)*BLOCK_SIZE)
				{playerPositionX = (worldSizeX-1)*BLOCK_SIZE;}


				// X-Collision
				if(getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/BLOCK_SIZE, playerPositionX/BLOCK_SIZE) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%BLOCK_SIZE+BLOCK_SIZE;}
				if(playerPositionY%BLOCK_SIZE && getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/BLOCK_SIZE+1, playerPositionX/BLOCK_SIZE) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%BLOCK_SIZE+BLOCK_SIZE;}

				if(getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/BLOCK_SIZE, playerPositionX/BLOCK_SIZE+1) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%BLOCK_SIZE;}
				if(playerPositionY%BLOCK_SIZE && getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/BLOCK_SIZE+1, playerPositionX/BLOCK_SIZE+1) >= 0)
				{playerPositionX = playerPositionX-playerPositionX%BLOCK_SIZE;}


				// Schwerkraft
				v += a;
				playerPositionY += v;


				// Y-Collision
				if(getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/BLOCK_SIZE+1, playerPositionX/BLOCK_SIZE) >= 0)
				{playerPositionY = playerPositionY-playerPositionY%BLOCK_SIZE;  v = 0;}
				if(playerPositionX%BLOCK_SIZE && getBlock(&world[0][0], worldSizeX, worldSizeY, playerPositionY/BLOCK_SIZE+1, playerPositionX/BLOCK_SIZE+1) >= 0)
				{playerPositionY = playerPositionY-playerPositionY%BLOCK_SIZE; v = 0;}


				// 
				if(playerPositionY < 0-BLOCK_SIZE || playerPositionY > worldSizeY*BLOCK_SIZE)
				{
					playerPositionX = 0;
					playerPositionY = 0;
					a = 0.6;
					v = 0;
				}


				// Kamera
				camPositionX += ((playerPositionX - SCREEN_WIDTH / 2 + BLOCK_SIZE / 2) - camPositionX) / 10;

				if(camPositionX < 0)
				{camPositionX = 0;}
				else if(camPositionX > worldSizeX*BLOCK_SIZE-SCREEN_WIDTH)
				{camPositionX = worldSizeX*BLOCK_SIZE-SCREEN_WIDTH;}


				// Lösche den Hintergrund
				SDL_FillRect( screen, NULL, color );


				// Zeichne das Hintergrundbild
				SDL_Rect background_source;
				background_source.x = (int) ( (((double)camPositionX/(worldSizeX*BLOCK_SIZE-SCREEN_WIDTH))) * (backgroundWidth-SCREEN_WIDTH) );
				background_source.y = 0;
				background_source.w = SCREEN_WIDTH;
				background_source.h = SCREEN_HEIGHT;

				SDL_Rect background_destination;
				background_destination.x = 0;
				background_destination.y = 0;
				background_destination.w = SCREEN_WIDTH;
				background_destination.h = SCREEN_HEIGHT;

				SDL_BlitSurface(background, &background_source, screen, &background_destination);


				// Zeichne alle Blöcke ein
				for(y = 0; y < SCREEN_HEIGHT/BLOCK_SIZE; y++)
				{
					for(x = camPositionX/BLOCK_SIZE; x < camPositionX/BLOCK_SIZE+SCREEN_WIDTH/BLOCK_SIZE+1; x++)
					{
						drawBlock(screen, blockset, x, y, camPositionX, getBlock(&world[0][0], worldSizeX, worldSizeY, y, x));
					}
				}


				// Zeichne den Spieler
				SDL_Rect player_source;
				player_source.x = 0;
				player_source.y = 0;
				player_source.w = BLOCK_SIZE;
				player_source.h = BLOCK_SIZE;

				SDL_Rect player_destination;
				player_destination.x = playerPositionX-camPositionX;
				player_destination.y = playerPositionY;
				player_destination.w = BLOCK_SIZE;
				player_destination.h = BLOCK_SIZE;

				SDL_BlitSurface(player, &player_source, screen, &player_destination);


				// Zeichne das berechnete Bild
				SDL_Flip( screen );
				break;
			}
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_LEFT:
						goLeft = 1;
						break;
					case SDLK_RIGHT:
						goRight = 1;
						break;
					case SDLK_SPACE:
						if(v == 0)
							{v = -9;}
						break;
					case SDLK_UP:
						a *= -1;
						break;
					case SDLK_ESCAPE:
						quit = 1;
						break;
					default:
						break;
				}
				break;
			}
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

		if(quit){return 0;}
	}

	return 1;
}
