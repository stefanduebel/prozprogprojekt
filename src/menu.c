#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "menu.h"

#define MENU_ITEMS 5
#define MENU_PADDING 20

#define RESOLUTION_NUM 5

int drawMenu(SDL_Surface *screen, TTF_Font *font, SDL_Event event)
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
	} menuItem[MENU_ITEMS];

	// ausgewählter Menü-Eintrag
	short selectedItem = 0;

	// initialisiere Namen der Einträge
	menuItem[0].name = "Starte Spiel"; 
	menuItem[1].name = "Highscores";
	menuItem[2].name = "Level Auswahl";
	menuItem[3].name = "Einstellungen";
	menuItem[4].name = "Spiel beenden";

	// Gesamthöhe des Menüs, zunächst nur die Zwischenräume
	unsigned int menuHeight = (MENU_ITEMS - 1) * MENU_PADDING;

	// rendere die einzelnen Menüeinträge in ihre jeweiligen Surfaces und bestimme die Gesamthöhe des Menüs
	for (int i = 0; i < MENU_ITEMS; i++)
	{
		menuItem[i].surface = TTF_RenderText_Solid(font, menuItem[i].name, color[(i == selectedItem)]);
		menuHeight += menuItem[i].surface->clip_rect.h;
	}

	// bereits ausgegebene Menühöhe
	unsigned int printedHeight = 0;

	// bestimme die x und y Koordinaten der einzelnen Einträge, zeige diese dann auf dem screen an
	for (int i = 0; i < MENU_ITEMS; i++)
	{
		// position der einzelnen Menü-Einträge bestimmen
		menuItem[i].position.x = (screen->clip_rect.w / 2) - (menuItem[i].surface->clip_rect.w / 2);
		menuItem[i].position.y = (screen->clip_rect.h / 2) - (menuHeight / 2) + i * MENU_PADDING + printedHeight;
		printedHeight += menuItem[i].surface->clip_rect.h;

		SDL_BlitSurface(menuItem[i].surface, NULL, screen, &(menuItem[i].position));
	}


	// warte auf Events
	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			// Quit-Event
			case SDL_QUIT:
			{
				return -1;
				break;
			}

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
						selectedItem--;
						// wenn oberster Eintrag gehe zum untersten
						if (selectedItem < 0)
							selectedItem = MENU_ITEMS - 1;

						// zeichne alle Einträge neu
						for (int i = 0; i < MENU_ITEMS; i++)
						{
							menuItem[i].surface = TTF_RenderText_Solid(font, menuItem[i].name, color[(i == selectedItem)]);
							SDL_BlitSurface(menuItem[i].surface, NULL, screen, &(menuItem[i].position));
						}
						break;

					// Pfeil runter
					case SDLK_DOWN:
						selectedItem++;
						// wenn unterster Eintrag gehe zum obersten
						selectedItem %= MENU_ITEMS;

						// zeichne alle Einträge neu
						for (int i = 0; i < MENU_ITEMS; i++)
						{
							menuItem[i].surface = TTF_RenderText_Solid(font, menuItem[i].name, color[(i == selectedItem)]);
							SDL_BlitSurface(menuItem[i].surface, NULL, screen, &(menuItem[i].position));
						}
						break;

					// Bestätigen-Taste
					case SDLK_RETURN:
						// gebe ausgewählten Eintrag zurück
						return selectedItem;
						break;

					default:
						break;
				}
				break;
			}
		}
	}

	return -1;
}

void drawSettingsMenu(SDL_Surface *screen, TTF_Font *font, SDL_Event event, struct resolution *res)
{
	// Färbe den Hintergrund schwarz
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0, 0, 0));

	// Farben für die Schrift der Menüeinträge (standard und ausgewählt)
	SDL_Color color[2] = {{255,255,255,0},{0,255,0,0}};

	// Struktur für einen einzelnen Menüeintrag (Name, Render-Fläche, Position), einen Eintrag für jede Auflösung, einen für Fullscreen, einen für zurück
	struct menu
	{
		char *name;
		SDL_Surface *surface;
		SDL_Rect position;
	} menuItem[RESOLUTION_NUM + 2];

	// ausgewählter Menü-Eintrag
	short selectedItem = 0;

	// initialisiere Namen der Einträge
	menuItem[0].name = "1920 x 1080 (HD1080p)";
	menuItem[1].name = "1280 x 720 (HD720p)";
	menuItem[2].name = "848 x 480 (WGA)";
	menuItem[3].name = "640 x 480 (VGA)";
	menuItem[4].name = "320 x 240 (QVGA)";
	menuItem[5].name = "Fullscreen";
	menuItem[6].name = "zurueck";

	// Gesamthöhe des Menüs, zunächst nur die Zwischenräume
	unsigned int menuHeight = (RESOLUTION_NUM + 2) * MENU_PADDING;

	// rendere die einzelnen Menüeinträge in ihre jeweiligen Surfaces und bestimme die Gesamthöhe des Menüs
	for (int i = 0; i < RESOLUTION_NUM + 2; i++)
	{
		menuItem[i].surface = TTF_RenderText_Solid(font, menuItem[i].name, color[(i == selectedItem)]);
		menuHeight += menuItem[i].surface->clip_rect.h;
	}

	// bereits ausgegebene Menühöhe
	unsigned int printedHeight = 0;

	// bestimme die x und y Koordinaten der einzelnen Einträge, zeige diese dann auf dem screen an
	for (int i = 0; i < RESOLUTION_NUM + 2; i++)
	{
		// position der einzelnen Menü-Einträge bestimmen
		menuItem[i].position.x = (screen->clip_rect.w / 2) - (menuItem[i].surface->clip_rect.w / 2);
		menuItem[i].position.y = (screen->clip_rect.h / 2) - (menuHeight / 2) + i * MENU_PADDING + printedHeight;
		printedHeight += menuItem[i].surface->clip_rect.h;

		SDL_BlitSurface(menuItem[i].surface, NULL, screen, &(menuItem[i].position));
	}


	// warte auf Events
	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			// Quit-Event
			case SDL_QUIT:
			{
				return;
				break;
			}

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
						selectedItem--;
						// wenn oberster Eintrag gehe zum untersten
						if (selectedItem < 0)
							selectedItem = RESOLUTION_NUM + 2 - 1;

						// zeichne alle Einträge neu
						for (int i = 0; i < RESOLUTION_NUM + 2; i++)
						{
							menuItem[i].surface = TTF_RenderText_Solid(font, menuItem[i].name, color[(i == selectedItem)]);
							SDL_BlitSurface(menuItem[i].surface, NULL, screen, &(menuItem[i].position));
						}
						break;

					// Pfeil runter
					case SDLK_DOWN:
						selectedItem++;
						// wenn unterster Eintrag gehe zum obersten
						selectedItem %= RESOLUTION_NUM + 2;

						// zeichne alle Einträge neu
						for (int i = 0; i < RESOLUTION_NUM + 2; i++)
						{
							menuItem[i].surface = TTF_RenderText_Solid(font, menuItem[i].name, color[(i == selectedItem)]);
							SDL_BlitSurface(menuItem[i].surface, NULL, screen, &(menuItem[i].position));
						}
						break;

					// Bestätigen-Taste
					case SDLK_RETURN:
						if (selectedItem == 0)
						{
							res->width = 1920;
							res->height = 720;
							if (screen->flags & SDL_FULLSCREEN)
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
							else
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
						}
						if (selectedItem == 1)
						{
							res->width = 1280;
							res->height = 720;
							if (screen->flags & SDL_FULLSCREEN)
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
							else
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
							
						}
						if (selectedItem == 2)
						{
							res->width = 848;
							res->height = 480;
							if (screen->flags & SDL_FULLSCREEN)
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
							else
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
						}
						if (selectedItem == 3)
						{
							res->width = 640;
							res->height = 480;
							if (screen->flags & SDL_FULLSCREEN)
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
							else
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
						}
						if (selectedItem == 4)
						{
							res->width = 320;
							res->height = 240;
							if (screen->flags & SDL_FULLSCREEN)
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
							else
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
						}
						if (selectedItem == 5)
						{
							if (!(screen->flags & (SDL_FULLSCREEN)))
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
							else
								screen = SDL_SetVideoMode( res->width, res->height, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
						}
						if (selectedItem == 6)
							return;

						// bereits ausgegebene Menühöhe
						printedHeight = 0;

						// bestimme die x und y Koordinaten der einzelnen Einträge, zeige diese dann auf dem screen an
						for (int i = 0; i < RESOLUTION_NUM + 2; i++)
						{
							// position der einzelnen Menü-Einträge bestimmen
							menuItem[i].position.x = (screen->clip_rect.w / 2) - (menuItem[i].surface->clip_rect.w / 2);
							menuItem[i].position.y = (screen->clip_rect.h / 2) - (menuHeight / 2) + i * MENU_PADDING + printedHeight;
							printedHeight += menuItem[i].surface->clip_rect.h;

							SDL_BlitSurface(menuItem[i].surface, NULL, screen, &(menuItem[i].position));
						}
						break;

					// Escape-Taste
					case SDLK_ESCAPE:
						return;
						break;

					default:
						break;
				}
				break;
			}
		}
	}
}

int drawLevelMenu(SDL_Surface *screen, TTF_Font *font, SDL_Event event)
{
	// Färbe den Hintergrund schwarz
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0, 0, 0));

	// Farben für die Schrift der Menüeinträge (standard und ausgewählt)
	SDL_Color color[2] = {{0,0,0,0},{255,255,255,0}};

	int items = 0;
	char filepath[50];
	FILE *levelFile;

	// bestimme Anzahl der Level
	while (1)
	{
		sprintf(filepath, "resources/maps/level%d.map", items);
		levelFile = fopen(filepath, "r");
		if (levelFile == NULL)
		{
			break;
		}
		items++;
	}

	// Struktur für einen einzelnen Menüeintrag (Name, Render-Fläche, Position, (zusätzlich könnte noch eine Beschreibung hinzugefügt werden))
	struct menu
	{
		char * description;
		SDL_Surface *selected;
		SDL_Surface *unselected;
		SDL_Rect position;
	} menuItem[items];

	// ausgewählter Menü-Eintrag
	unsigned short selectedItem = 0;

	SDL_Surface *itemBackgroundUnselected = SDL_LoadBMP(MENU_BACKGROUND);
	SDL_Surface *itemBackgroundSelected = SDL_LoadBMP(MENU_BACKGROUND_SELECTED);
	SDL_Rect descriptionPosition;
	SDL_Surface *descriptionSurface;

	// Gesamtabmessunge des Menüs (füllt immer die Zeilen aus)
	unsigned int itemsPerRow = (screen->clip_rect.w - (4 * MENU_PADDING)) / (itemBackgroundUnselected->clip_rect.w + MENU_PADDING);
	unsigned int menuWidth = (itemsPerRow * itemBackgroundUnselected->clip_rect.w) + ((itemsPerRow-1)* MENU_PADDING);
	unsigned int menuHeight = (((items / itemsPerRow) + 1) * itemBackgroundUnselected->clip_rect.h) + ((items / itemsPerRow) * MENU_PADDING);

	// rendere die einzelnen Menüeinträge in ihre jeweiligen Surfaces und bestimme die Gesamthöhe des Menüs
	for (int i = 0; i < items; i++)
	{
		// Level-Beschreibung laden
		sprintf(filepath, "resources/maps/level%d.map", i);
		FILE *levelFile = fopen(filepath, "r");
		size_t len = 0;
		getline(&(menuItem[i].description), &len, levelFile);
		if (menuItem[i].description[strlen(menuItem[i].description)-1] == '\n')
			menuItem[i].description[strlen(menuItem[i].description)-1] = '\0';

		// Hintergrundgrafik
		sprintf(filepath, "resources/images/thumbnails/level%dSelected.png", i);
		menuItem[i].selected = IMG_Load(filepath);
		sprintf(filepath, "resources/images/thumbnails/level%dUnselected.png", i);
		menuItem[i].unselected = IMG_Load(filepath);
		if ((!menuItem[i].unselected) || (!menuItem[i].selected))
		{
			menuItem[i].selected = SDL_ConvertSurface(itemBackgroundSelected, itemBackgroundSelected->format, SDL_SWSURFACE);
			menuItem[i].unselected = SDL_ConvertSurface(itemBackgroundUnselected, itemBackgroundUnselected->format, SDL_SWSURFACE);
		}

		menuItem[i].position.x = ((screen->clip_rect.w - menuWidth) / 2) + ((i%itemsPerRow) * (itemBackgroundUnselected->clip_rect.w + MENU_PADDING));
		menuItem[i].position.y = ((screen->clip_rect.h - menuHeight) / 2) + ((i/itemsPerRow) * (itemBackgroundUnselected->clip_rect.h + MENU_PADDING));
	}
	// warte auf Events
	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			// Quit-Event
			case SDL_QUIT:
			{
				return -1;
				break;
			}

			// behandle Timer-Event
			case SDL_USEREVENT:
				// zeichne alle Einträge neu
				SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0, 0, 0));
				for (int i = 0; i < items; i++)
				{
					if (i == selectedItem)
						SDL_BlitSurface(menuItem[i].selected, NULL, screen, &(menuItem[i].position));
					else
						SDL_BlitSurface(menuItem[i].unselected, NULL, screen, &(menuItem[i].position));
				}
				descriptionSurface = TTF_RenderText_Solid(font, menuItem[selectedItem].description, color[1]);
				descriptionPosition.x = (screen->clip_rect.w / 2) - (descriptionSurface->clip_rect.w / 2);
				descriptionPosition.y = (screen->clip_rect.h) - descriptionSurface->clip_rect.h - 10;
				SDL_BlitSurface(descriptionSurface, NULL, screen, &descriptionPosition);
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
						
						// wenn oberster Eintrag gehe zum untersten
						if (selectedItem < itemsPerRow)
							selectedItem += (items - (items%itemsPerRow));
						else
							selectedItem -= itemsPerRow;
						if (selectedItem >= items)
							selectedItem -= itemsPerRow;
						break;

					// Pfeil runter
					case SDLK_DOWN:
						selectedItem += itemsPerRow;
						// wenn unterster Eintrag gehe zum obersten
						if (selectedItem >= items)
							selectedItem %= itemsPerRow;
						break;

					// Pfeil rechts
					case SDLK_RIGHT:
						
						// wenn unterster Eintrag gehe zum obersten
						if ((selectedItem % itemsPerRow == itemsPerRow-1) || (selectedItem > items - 2))
							selectedItem = (selectedItem / itemsPerRow) * itemsPerRow;
						else
							selectedItem++;
						break;

					// Pfeil links
					case SDLK_LEFT:
						// wenn unterster Eintrag gehe zum obersten
						if ((selectedItem % itemsPerRow == 0))
							selectedItem += itemsPerRow;

						if (selectedItem > items)
							selectedItem = items;

						selectedItem--;
						break;

					// Escape-Taste
					case SDLK_ESCAPE:
						return -1;
						break;

					// Bestätigen-Taste
					case SDLK_RETURN:
						// gebe ausgewählten Eintrag zurück
						return selectedItem;
						break;

					default:
						break;
				}
				break;
			}
		}
	}

	return -1;
}
