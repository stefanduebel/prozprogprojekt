#include <stdlib.h>
#include <stdio.h>


#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#include <SDL/SDL_ttf.h>

#include "menu.h"
#include "game.h"

#define SCREEN_BPP      		32

#define FONT_SIZE 16

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

	if (argc > 1)
	{
		res.width = 1280;
		res.height = 720;
	}
	else
	{
		res.width = 640;
		res.height = 480;
	}

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
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) < 0 ) {
		fprintf( stderr, "SDL konnte nicht initialisiert werden:  %s\n", SDL_GetError() );
		return 1;
	}

	// Timer einrichten
	SDL_AddTimer (16, generate_userevent, NULL);

	// Event-System initialisieren
	SDL_Event event;

	// Erstelle die Bildschirmfläche
	SDL_Surface *screen = NULL;
	screen = SDL_SetVideoMode( res.width, res.height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);

	while (1)
	{
		switch (draw_menu(screen, font, event))
		{
			case 0:
				printf("Starte Spiel\n");
				if(start_game (screen, event, res))
					printf("Verloren!\n");
				break;
			case 1:
				printf("Zeige Highscores\n");
				break;
			case 2:
				printf("Levelauswahl\n");
				break;
			default:
				printf("Beende Spiel\n");
				SDL_Quit();
				exit(0);
		}
	}
	
	return 0;
}

