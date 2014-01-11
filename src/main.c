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
	struct resolution res;

	res.width = 1280;
	res.height = 720;

	// Load a font
	TTF_Font *font;

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

	// Erstelle die Bildschirmfläche
	SDL_Surface *screen = NULL;
	screen = SDL_SetVideoMode( res.width, res.height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);

	SDL_ShowCursor(0);

	// Audio
	int audioRate = 22050;
	Uint16 audioFormat = AUDIO_S16SYS;
	int audioChannels = 2;
	int audioBuffers = 4096;
 
	if(Mix_OpenAudio(audioRate, audioFormat, audioChannels, audioBuffers) != 0) {
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		exit(1);
	}

	Mix_Chunk *sound = NULL;
 
	sound = Mix_LoadWAV("resources/jeopardy.wav");
	if(sound == NULL) {
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
	}

	int channel;
 
	channel = Mix_PlayChannel(-1, sound, -1);
	if(channel == -1) {
		fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	}

	while (1)
	{
		int levelNo;
		switch (drawMenu(screen, font, event))
		{
			case 0:
				printf("Starte Spiel\n");
				if(startGame (screen, event, res, 0))
					printf("Verloren!\n");
				break;
			case 1:
				printf("Zeige Highscores\n");
				break;
			case 2:
				levelNo = drawLevelMenu(screen, font, event);
				if (levelNo != -1)
					startGame(screen, event, res, levelNo);
				break;
			case 3:
				drawSettingsMenu(screen, font, event, &res);
				break;
			default:
				printf("Beende Spiel\n");
				SDL_Quit();
				exit(0);
		}
	}
	
	return 0;
}

