#include <stdlib.h>
#include <stdio.h>


#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>

#ifndef MAIN_H
#include "main.h"
#define MAIN_H
#endif

#include "menu.h"
#include "game.h"
#include "highscore.h"
#include "sound.h"

#define FONT_SIZE 30

Uint32 generate_userevent (Uint32 intervall, void *parameter)
{
	SDL_Event event;
	SDL_UserEvent userevent;

	/* initialisiere einen UserEvent */
	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = NULL;
	userevent.data2 = NULL; 

	/* ... initialisiere damit einen Event */
	event.type = SDL_USEREVENT;
	event.user = userevent;

	/* neuen Event erzeugen */
	SDL_PushEvent (&event);
	return intervall;
}

int main( int argc, char *argv[] )
{
	struct highscoreItem *highscore = loadHighscore();

	res.width = 1280;
	res.height = 720;

	// Load a font

	if (TTF_Init() != 0)
	{
		printf("TTF_Init() Failed: %s\n", TTF_GetError());
		SDL_Quit();
		exit(1);
	}

	font = TTF_OpenFont("resources/fonts/DejaVuSans.ttf", FONT_SIZE);
	if (font == NULL)
	{
		printf("TTF_OpenFont() Failed: %s\n", TTF_GetError());
		TTF_Quit();
		SDL_Quit();
		exit(1);
	}
	
	// Initialisiere SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0 ) {
		fprintf( stderr, "SDL konnte nicht initialisiert werden:  %s\n", SDL_GetError() );
		return 1;
	}

	// Initialisiere SDL_Image
	IMG_Init(IMG_INIT_PNG);

	// Timer einrichten
	SDL_AddTimer (16, generate_userevent, NULL);

	// Event-System initialisieren
	SDL_Event event;

	SDL_WM_SetCaption("Jump and Run", "Jump and Run");
	SDL_Surface *icon = IMG_Load("resources/images/icon.png");
	if (icon != NULL)
		SDL_WM_SetIcon(icon, NULL);

	// Erstelle die Bildschirmfläche
	SDL_Surface *screen = NULL;
	screen = SDL_SetVideoMode( res.width, res.height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);

	

	SDL_ShowCursor(0);

	initializeSounds();
	startMusic();

	initializeClouds();

	int returnValue = drawMenu(screen, event);
	while (1)
	{
		switch (returnValue)
		{
			case MENU:
				returnValue = drawMenu(screen, event);
				break;

			case START_GAME:
				returnValue = LEVEL_OFFSET;
				break;

			case HIGHSCORES:
				drawHighscore(screen, font, event, highscore);
				returnValue = MENU;
				break;

			default:
				if (returnValue >= LEVEL_OFFSET && returnValue < LEVEL_OFFSET + MAX_LEVEL)
				{
					returnValue = addScore(screen, startGame (screen, event, res, returnValue - LEVEL_OFFSET), event, &highscore);
				}
				else if (returnValue <= EXIT_GAME)
				{
					printf("Spiel beenden\n");
					freeHighscore(highscore);
					SDL_Quit();
					Mix_CloseAudio();
					Mix_Quit();
					exit(returnValue - EXIT_GAME);
				}
				else
					returnValue = MENU;
		}
	}
	
	return 0;
}

