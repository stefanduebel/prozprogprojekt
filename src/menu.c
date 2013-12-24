#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#include <SDL/SDL_ttf.h>
#include "menu.h"

#define MENU_ITEMS 4
#define MENU_PADDING 20

int draw_menu(SDL_Surface *screen, TTF_Font *font, SDL_Event event)
{
	// Färbe den Hintergrund schwarz
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0, 0, 0));

	// Farben für die Schrift der Menüeinträge (standard und ausgewählt)
	SDL_Color color[2] = {{255,255,255,0},{0,255,0,0}};

	// Struktur für einen einzelnen Menüeintrag (Name, Render-Fläche, Position)
	struct menu
	{
		char *name;
		SDL_Surface *surface;
		SDL_Rect position;
	} menu_item[MENU_ITEMS];

	// ausgewählter Menü-Eintrag
	short selected_item = 0;

	// initialisiere Namen der Einträge
	menu_item[0].name = "Starte Spiel"; 
	menu_item[1].name = "Highscores";
	menu_item[2].name = "Level Auswahl";
	menu_item[3].name = "Spiel beenden";

	// Gesamthöhe des Menüs, zunächst nur die Zwischenräume
	unsigned int menu_height = (MENU_ITEMS - 1) * MENU_PADDING;

	// rendere die einzelnen Menüeinträge in ihre jeweiligen Surfaces und bestimme die Gesamthöhe des Menüs
	for (int i = 0; i < MENU_ITEMS; i++)
	{
		menu_item[i].surface = TTF_RenderText_Solid(font, menu_item[i].name, color[(i == selected_item)]);
		menu_height += menu_item[i].surface->clip_rect.h;
	}

	// bereits ausgegebene Menühöhe
	unsigned int printed_height = 0;

	// bestimme die x und y Koordinaten der einzelnen Einträge, zeige diese dann auf dem screen an
	for (int i = 0; i < MENU_ITEMS; i++)
	{
		// position der einzelnen Menü-Einträge bestimmen
		menu_item[i].position.x = (screen->clip_rect.w / 2) - (menu_item[i].surface->clip_rect.w / 2);
		menu_item[i].position.y = (screen->clip_rect.h / 2) - (menu_height / 2) + i * MENU_PADDING + printed_height;
		printed_height += menu_item[i].surface->clip_rect.h;

		SDL_BlitSurface(menu_item[i].surface, NULL, screen, &(menu_item[i].position));
	}


	// warte auf Events
	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			// behandle Timer-Event
			case SDL_USEREVENT:
				// rendere den Bildschirm neu (Hintergrundanimation wäre möglich)
				SDL_Flip(screen);
				break;

			// behandle Tastendruck
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					// Pfeil hoch
					case SDLK_UP:
						selected_item--;
						// wenn oberster Eintrag gehe zum untersten
						if (selected_item < 0)
							selected_item = MENU_ITEMS - 1;

						// zeichne alle Einträge neu
						for (int i = 0; i < MENU_ITEMS; i++)
						{
							menu_item[i].surface = TTF_RenderText_Solid(font, menu_item[i].name, color[(i == selected_item)]);
							SDL_BlitSurface(menu_item[i].surface, NULL, screen, &(menu_item[i].position));
						}
						break;

					// Pfeil runter
					case SDLK_DOWN:
						selected_item++;
						// wenn unterster Eintrag gehe zum obersten
						selected_item %= MENU_ITEMS;

						// zeichne alle Einträge neu
						for (int i = 0; i < MENU_ITEMS; i++)
						{
							menu_item[i].surface = TTF_RenderText_Solid(font, menu_item[i].name, color[(i == selected_item)]);
							SDL_BlitSurface(menu_item[i].surface, NULL, screen, &(menu_item[i].position));
						}
						break;

					// Bestätigen-Taste
					case SDLK_RETURN:
						// gebe ausgewählten Eintrag zurück
						return selected_item;

					default:
						break;
				}
				break;
			}
		}
	}

	return -1;
}
