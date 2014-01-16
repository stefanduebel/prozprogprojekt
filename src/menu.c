#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <string.h>
#include "menu.h"
#include "image.h"

cloud clouds[CLOUDS];

int drawMenu(SDL_Surface *screen, SDL_Event event)
{
	initializeClouds();

	int changeV = 0, changeH = 0;
	int selectedItem = 0;

	menu *firstItem = NULL;

	firstItem = initializeListMenu((char * []){"Spiel starten", "Highscores", "Level-Auswahl", "Einstellungen", "Beenden"}, 5);

	unsigned char menuType = MAIN_MENU;


	if (firstItem == NULL)
		return -3; // Fehler bei der initialisierung des Menüs

	SDL_Surface *menuSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->clip_rect.w, screen->clip_rect.h, SCREEN_BPP, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	selectedItem = renderMenu(menuSurface, 0, 0, firstItem, -1);

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
				// rendere den Bildschirm neu
				CLEAR_SURFACE(screen);
				renderClouds(screen);

				if (changeV || changeH) // es sind Änderungen am Menü geschehen, also neu rendern
					selectedItem = renderMenu(menuSurface, changeH, changeV, firstItem, -1);

				SDL_BlitSurface(menuSurface, NULL, screen, NULL);

				changeV = changeH = 0;

				SDL_Flip(screen);
				break;

			// behandle Tastendruck
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					// Pfeil hoch
					case SDLK_UP:
						changeV--;
						break;

					// Pfeil runter
					case SDLK_DOWN:
						changeV++;
						break;

					// Pfeil rechts
					case SDLK_RIGHT:
						changeH++;
						break;

					// Pfeil links
					case SDLK_LEFT:
						changeH--;
						break;

					// Bestätigen-Taste
					case SDLK_RETURN:
						// Lösche derzeitiges Menü (wenn eine Aktion durgeführt wird gibt es auf jeden Fall ein anderes)
						freeMenu(firstItem);
						// Unterscheide die Aktion je nach Menü-Typ
						switch (menuType)
						{
							case MAIN_MENU:
								switch (selectedItem)
								{
									case 0:
										printf("starte Spiel\n");
										return START_GAME;

									case 1:
										printf("Highscores\n");
										return HIGHSCORES;

									case 2:
										printf("Level-Auswahl\n");
										firstItem = initializeLevelMenu();
										selectedItem = renderMenu(menuSurface, 0, 0, firstItem, -1);
										menuType = LEVEL_MENU;
										break;

									case 3:
										printf("Einstellungs-Auswahl\n");
										firstItem = initializeListMenu((char * []){"HD-1080p", "HD-720p", "WGA", "VGA", "Fullscreen", "zurueck"}, 6);
										selectedItem = renderMenu(menuSurface, 0, 0, firstItem, -1);
										menuType = SETTINGS_MENU;
										break;

									default:
										return 0;
								}
								break;

							case SETTINGS_MENU:
								switch (selectedItem)
								{
									case 0:
										printf("1080p\n");
										return -1;

									case 1:
										printf("720p\n");
										return -1;

									case 2:
										printf("WGA\n");
										return -1;

									case 3:
										printf("VGA\n");
										return -1;

									case 4:
										printf("Fullscreen\n");
										return -1;

									case 5:
										firstItem = initializeListMenu((char * []){"Spiel starten", "Highscores", "Level-Auswahl", "Einstellungen", "Beenden"}, 5);
										selectedItem = renderMenu(menuSurface, 0, 0, firstItem, -1);
										menuType = MAIN_MENU;
										break;
								}
								break;

							case LEVEL_MENU:
								return selectedItem + 10;
						}
						break;

					case SDLK_ESCAPE:
						if (menuType == LEVEL_MENU || menuType == SETTINGS_MENU)
						{
							firstItem = initializeListMenu((char * []){"Spiel starten", "Highscores", "Level-Auswahl", "Einstellungen", "Beenden"}, 5);
							selectedItem = renderMenu(menuSurface, 0, 0, firstItem, -1);
							menuType = MAIN_MENU;
						}
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

void initializeClouds(void)
{
	for (int i = 0; i < CLOUDS; i++)
	{
		// initialisiere die einzelnen Wolken
		// TODO: randomize
		clouds[i].velocity = (double) (i%4 + 1) / 32;
		clouds[i].height = (i * 4);
		clouds[i].position = i%5 * 10;
		SDL_Surface *tmp = IMG_Load("resources/images/cloud.png");
		clouds[i].surface = shrinkSurface(tmp, 3*48, 48); // TODO: richtige Größe
		SDL_FreeSurface(tmp);
	}
}

menu *initializeLevelMenu()
{
	/* ---- Menüeinträge erzeugen ---- */

	int items = 0;
	char filepath[50];

	menuItem *firstItem, *lastItemGenerated;

	while (1)
	{
		/* ---- stelle fest ob Level existiert ---- */
		sprintf(filepath, "resources/maps/level%d.map", items);
		FILE *levelFile = fopen(filepath, "r");
		if (levelFile == NULL)
			break;

		/* ---- erzeuge Eintrag ---- */
		if (items == 0)
			lastItemGenerated = firstItem = createItem(0, 0, NULL, NULL, NULL, NULL, font);
		else
			lastItemGenerated = lastItemGenerated->next = createItem(0, 0, NULL, NULL, NULL, NULL, font);

		/* ---- lese Beschreibung aus ---- */
		size_t len = 0;
		getline(&(lastItemGenerated->description), &len, levelFile);

		/* ---- nutze Thumbnails wenn vorhanden ---- */
		// TODO: PNGs laden
		sprintf(filepath, "resources/images/thumbnails/level%dSelected.bmp", items);
		lastItemGenerated->selected = SDL_LoadBMP(filepath);//IMG_Load(filepath);
		sprintf(filepath, "resources/images/thumbnails/level%dUnselected.bmp", items);
		lastItemGenerated->unselected = SDL_LoadBMP(filepath);//IMG_Load(filepath);

		if (lastItemGenerated->selected == NULL)
			lastItemGenerated->selected = SDL_LoadBMP(MENU_BACKGROUND_SELECTED);
		if (lastItemGenerated->unselected == NULL)
			lastItemGenerated->unselected = SDL_LoadBMP(MENU_BACKGROUND);

		items++;
	}

	/* ---- Gesamtabmessunge des Menüs (füllt immer die Zeilen aus) ---- */
	unsigned int itemsPerRow = (res.width - (4 * MENU_PADDING)) / (48 + MENU_PADDING);
	unsigned int menuWidth = (itemsPerRow * 48) + ((itemsPerRow-1)* MENU_PADDING);
	unsigned int menuHeight = (((items / itemsPerRow) + 1) * 48) + ((items / itemsPerRow) * MENU_PADDING);

	/* ---- Positionen bestimmen ---- */
	lastItemGenerated = firstItem;
	for (int i = 0; i < items; i++)
	{
		lastItemGenerated->position.x = ((res.width - menuWidth) / 2) + ((i%itemsPerRow) * (48 + MENU_PADDING));
		lastItemGenerated->position.y = ((res.height - menuHeight) / 2) + ((i/itemsPerRow) * (48 + MENU_PADDING));
		lastItemGenerated = lastItemGenerated->next;
	}

	menu *newMenu = createMenu(itemsPerRow, (items/itemsPerRow), items, firstItem);

	return newMenu;
}

menu *initializeListMenu(char *itemNames[], unsigned char items)
{
	/* ---- Menüeinträge erzeugen ---- */

	menuItem *firstItem, *lastItemGenerated;

	lastItemGenerated = firstItem = createItem(0, 0, itemNames[0], NULL, NULL, NULL, font);

	for (int i = 1; i < items; i++)
		lastItemGenerated = lastItemGenerated->next = createItem(0, 0, itemNames[i], NULL, NULL, NULL, font);

	menu *mainMenu = createMenu(1, items, items, firstItem);

	/* ---- Positionen bestimmen ---- */
	// Gesamthöhe bestimmen
	unsigned int menuHeight = (mainMenu->items - 1) * MENU_PADDING; /*!< Gesamthöhe des Menüs*/
	menuItem *tmp1 = firstItem;
	do
	{
		menuHeight += tmp1->selected->clip_rect.h;
	} while ( (tmp1 = tmp1->next) != NULL);

	// Positionen der einzelnen Einträge bestimmen
	unsigned int printedHeight = 0; /*!< Höhe welche bereits vergeben ist */
	tmp1 = firstItem; int i = 0;
	do
	{
		tmp1->position.x = (res.width / 2) - (tmp1->selected->clip_rect.w / 2);
		tmp1->position.y = (res.height / 2) - (menuHeight / 2) + i * MENU_PADDING + printedHeight;
		printedHeight += tmp1->selected->clip_rect.h;
		i++;
	} while ( (tmp1 = tmp1->next) != NULL);
	return mainMenu;
}

void renderClouds(SDL_Surface *surface)
{
	for (int i = 0; i < CLOUDS; i++)
	{
		// bestimme die absolute Position am Himmel as den relativen Positionen der Wolke
		SDL_Rect offset;
		offset.x = clouds[i].position * surface->clip_rect.w / 100;
		offset.y = clouds[i].height * surface->clip_rect.h / 100;

		SDL_BlitSurface(clouds[i].surface, NULL, surface, &offset);

		// Wolke weiterziehen lassen
		clouds[i].position += clouds[i].velocity;
		if (clouds[i].position > 100)
		{
			// TODO: neue Zufallswerte bestimmen 
			clouds[i].position = 0 - ((double) surface->clip_rect.w / clouds[i].surface->clip_rect.w) - 10;
		}
	}
}

int renderMenu(SDL_Surface *surface, int changeH, int changeV, menu *menu, int descriptionY)
{
	CLEAR_SURFACE(surface);
	/* ---- ausgewählten Eintrag weiterbewegen ---- */

	// nach oben raus
	if (menu->selected < (int) menu->w && changeV < 0)
	{
		int x;
		if ((menu->items % menu->w) == 0)
			x = 1;
		else
			x = menu->items % menu->w;
		menu->selected += (menu->items - x);
		if (menu->selected >= (int) menu->items)
			menu->selected -= menu->w;
	}
	// normal weiter schieben (vertikal)
	else
		menu->selected += changeV * menu->w;

	// nach unten raus
	if (menu->selected >= (int) menu->items)
		menu->selected %= menu->w;

	// wenn Menü mehrere Einträge in der Breite hat
	if(menu->w > 1)
	{
		menu->selected += changeH;
		if (menu->selected >= (int) menu->items)
			menu->selected = 0;
		else if (menu->selected < 0)
			menu->selected = menu->items-1;
	}

	printf("rendere Menü\n");
	
	/* ---- Einträge rendern ---- */
	// TODO: wenn Beschreibung: diese auch rendern
	menuItem *item = menu->next;
	int i = 0;
	do
	{
		if (i == menu->selected)
		{
			SDL_BlitSurface(item->selected, NULL, surface, &item->position);
		}
		else
		{
			SDL_BlitSurface(item->unselected, NULL, surface, &item->position);
		}
		i++;
	} while ((item = item->next) != NULL);

	return menu->selected;
}

menuItem *createItem (int x, int y, const char *text, const char* description, const char* pathImageSelected, const char* pathImageUnselected, TTF_Font *font)
{
	menuItem *item = (menuItem *) malloc(sizeof(menuItem));
	if (!item)
	{
		printf("keinen Speicher bekommen!\n");
		exit(-1);
	}
	item->position.x = x;
	item->position.y = y;

	if (pathImageSelected != NULL)
		item->selected   = IMG_Load(pathImageSelected);
	if (pathImageUnselected != NULL)
		item->unselected = IMG_Load(pathImageUnselected);

	if (text != NULL)
	{
		SDL_Color color[2] = {{100,100,100,0},{255,255,255,0}};
		item->selected = TTF_RenderText_Solid(font, text, color[1]);
		item->unselected = TTF_RenderText_Solid(font, text, color[0]);
	}

	if (description != NULL)
		strcpy(item->description, description);

	item->next = NULL;

	return item;
}

menu *createMenu(unsigned int w, unsigned int h, unsigned int items, menuItem *firstItem)
{
	menu *newMenu = (menu *) malloc(sizeof(menu));
	if (!newMenu)
		return NULL;
	newMenu->w = w;
	newMenu->h = h;
	newMenu->items = items;
	newMenu->selected = 0;
	newMenu->next = firstItem;

	return newMenu;
}

void freeMenu(menu *item)
{
	if (!item->next)
		freeMenuItem(item->next);

	free(item);
}

void freeMenuItem(menuItem *item)
{
	if (!item->next)
		freeMenuItem(item->next);

	free(item);
}
