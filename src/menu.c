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
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0, 0, 0));

	SDL_Color color[2] = {{255,255,255},{0,255,0}};

	// Struktur für einen einzelnen Menüeintrag (Name, Render-Fläche, Position, Status)
	struct menu
	{
		char *name;
		SDL_Surface *surface;
		SDL_Rect position;
	} menu_item[MENU_ITEMS];

	short selected_item = 0;

	// initialisiere Namen der Einträge
	menu_item[0].name = "Starte Spiel";
	menu_item[1].name = "Highscores";
	menu_item[2].name = "Level Auswahl";
	menu_item[3].name = "Spiel beenden";

	unsigned int menu_height = (MENU_ITEMS - 1) * MENU_PADDING;
	
	for (int i = 0; i < MENU_ITEMS; i++)
	{
		// rendere die Schrift
		menu_item[i].surface = TTF_RenderText_Solid(font, menu_item[i].name, color[(i == selected_item)]);
		menu_height += menu_item[i].surface->clip_rect.h;
	}

	unsigned int printed_height = 0;
	for (int i = 0; i < MENU_ITEMS; i++)
	{
		// position der einzelnen Menü-Einträge bestimmen
		menu_item[i].position.x = (screen->clip_rect.w / 2) - (menu_item[i].surface->clip_rect.w / 2);
		menu_item[i].position.y = (screen->clip_rect.h / 2) - (menu_height / 2) + i * MENU_PADDING + printed_height;
		printed_height += menu_item[i].surface->clip_rect.h;

		SDL_BlitSurface(menu_item[i].surface, NULL, screen, &(menu_item[i].position));
	}




	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			case SDL_USEREVENT:
				SDL_Flip(screen);
				break;

			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_UP:
						selected_item--;
						if (selected_item < 0)
							selected_item = MENU_ITEMS - 1;
						for (int i = 0; i < MENU_ITEMS; i++)
						{
							menu_item[i].surface = TTF_RenderText_Solid(font, menu_item[i].name, color[(i == selected_item)]);
							SDL_BlitSurface(menu_item[i].surface, NULL, screen, &(menu_item[i].position));
						}
						break;

					case SDLK_DOWN:
						selected_item++;
						selected_item %= MENU_ITEMS;
						for (int i = 0; i < MENU_ITEMS; i++)
						{
							menu_item[i].surface = TTF_RenderText_Solid(font, menu_item[i].name, color[(i == selected_item)]);
							SDL_BlitSurface(menu_item[i].surface, NULL, screen, &(menu_item[i].position));
						}
						break;

					case SDLK_RETURN:
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
