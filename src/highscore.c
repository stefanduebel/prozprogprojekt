#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "highscore.h"
#ifndef MAIN_H
	#include "main.h"
	#define MAIN_H
#endif
#include "menu.h"

#define HIGHSCORE_PATH "resources/highscore.txt"
#define MAX_LENGTH 20
#define MAX(x,y) ((x)<=(y) ?(y) :(x))
#define HIGHSCORE_PADDING 20


// Funktion für SDL-Ausgabe
void drawHighscore (SDL_Surface *screen, TTF_Font *font, SDL_Event event, struct highscoreItem *highscoreList)
{
	CLEAR_SURFACE(screen);

	/* ---- Surfaces für Name und Punktzahl ---- */
	SDL_Surface *nameSurface[10];
	SDL_Surface *pointsSurface[10];

	SDL_Color color = {100,100,100,0};

	int height = 0;
	int entries = 0;
	int maxWidth = 0;

	/* ---- bestimmen der Anzahl der Einträge und der Gesamthöhe des Menüs ---- */
	for(entries = 0; entries < 10; entries++)
	{
		if(highscoreList == NULL)
		{
			break;
		}
		nameSurface[entries] = TTF_RenderText_Solid(font, highscoreList->name, color);
		char pointsString[7];
		sprintf(pointsString, "%d", highscoreList->points);
		pointsSurface[entries] = TTF_RenderText_Solid(font, pointsString, color);
		highscoreList = highscoreList->next;
		height += MAX(pointsSurface[entries]->clip_rect.h, nameSurface[entries]->clip_rect.h);
		maxWidth = MAX(maxWidth, nameSurface[entries]->clip_rect.w + pointsSurface[entries]->clip_rect.w);
	}
	// Zwischenräume hinzufügen
	height += (entries - 1) * HIGHSCORE_PADDING;

	int printedHeight = 0;
	int highscoreOffsetY = (screen->clip_rect.h - height) / 2;
	int highscoreWidth = maxWidth + 3*HIGHSCORE_PADDING;
	SDL_Surface *highscoreSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->clip_rect.w, screen->clip_rect.h, SCREEN_BPP, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

	/* ---- alle Einträge auf die Highscoresurface rendern ---- */
	for(int i = 0; i < entries; i++)
	{
		SDL_Rect position;
		position.x = (screen->clip_rect.w - highscoreWidth) / 2;
		position.y = highscoreOffsetY + printedHeight;
		SDL_BlitSurface(nameSurface[i], NULL, highscoreSurface, &position);

		position.x = screen->clip_rect.w - pointsSurface[i]->clip_rect.w - ((screen->clip_rect.w - highscoreWidth) / 2);
		SDL_BlitSurface(pointsSurface[i], NULL, highscoreSurface, &position);

		printedHeight += nameSurface[i]->clip_rect.h + HIGHSCORE_PADDING;
	}

	while (SDL_WaitEvent (&event))
	{
		switch (event.type)
		{
			case SDL_USEREVENT:
				// Hintergrund animieren und Highscores anzeigen
				CLEAR_SURFACE(screen);
				renderClouds(screen, 0);
				SDL_BlitSurface(highscoreSurface, NULL, screen, NULL);
				SDL_Flip(screen);
				break;

			case SDL_QUIT:
				SDL_Quit();
				exit(0);

			case SDL_KEYDOWN:
				return;
		}
	}
} 

void append(struct highscoreItem **list, char name[], unsigned int points)
{
	struct highscoreItem *newElement;
	struct highscoreItem *listTemp = *list;

	newElement = (struct highscoreItem*) malloc(sizeof(*newElement)); // erzeuge ein neues Element

	newElement->name = (char*) malloc(sizeof(name));
	strcpy(newElement->name, name);

	newElement->points = points;

	newElement->next = NULL; // Wichtig für das Erkennen des Listenendes

	if ( listTemp != NULL ) // sind Elemente vorhanden
	{
		while (listTemp->next != NULL ) // suche das letzte Element
		{
			listTemp=listTemp->next;
		}
		listTemp->next=newElement; // Hänge das Element hinten an
	}
	else // wenn die liste leer ist, ist das neue das erste Element
	{
		*list=newElement;
	}
}

// Funktion zum Laden des Highscores
struct highscoreItem *loadHighscore (void)
{
	FILE *highscoreFile = fopen (HIGHSCORE_PATH, "r");
	if (highscoreFile == NULL)
	{
		return NULL;
	}
	char *line = NULL;
	size_t len = 0;

	char tempName[MAX_LENGTH];
	unsigned int tempPoints;
	struct highscoreItem *highscoreList = NULL;

	while(1)
	{
		if(getline (&line, &len, highscoreFile) != -1)
		{
			char *tok = strtok(line, ",");
			strcpy(tempName, tok);

			tok = strtok(NULL, ",");
			sscanf(tok, "%u", &tempPoints);

			append(&highscoreList, tempName, tempPoints);
		}
		else
		{
			break;
		}
	}
	fclose(highscoreFile);
	return highscoreList;
}

// Funktion zum Schreiben in die Highscoreliste
void writeHighscore (struct highscoreItem *highscoreList)
{
	FILE *highscoreFile = fopen (HIGHSCORE_PATH, "w");

	do
	{
		fprintf(highscoreFile, "%s,%u\n", highscoreList->name, highscoreList->points);

		highscoreList = highscoreList->next;
	} while (highscoreList != NULL);

	fclose(highscoreFile);
}

//Funktion zum Einfügen eines Elementes in die Liste
void insertHighscore (struct highscoreItem **highscoreList, char name[], unsigned int points)
{
	struct highscoreItem *currentItem;
	struct highscoreItem *newItem = (struct highscoreItem*) malloc(sizeof(struct highscoreItem));
	if (newItem == NULL)
	{
		exit(-1);
	}

	currentItem = *highscoreList;

	newItem->name = (char *) malloc(sizeof(name));
	if (newItem->name == NULL)
	{
		exit(-1);
	}
	strcpy(newItem->name, name);
	newItem->points = points;

	if (newItem->points > currentItem->points)
	{
		newItem->next = *highscoreList;
		*highscoreList = newItem;
		printf("Highscore an erster Stelle eingefügt");
	}
	else
	{
		while (1)
		{
			if (currentItem->next != NULL)
			{
				if (newItem->points > currentItem->next->points)
				{
					newItem->next = currentItem->next;
					currentItem->next = newItem;
					printf("Highscore an mittiger Stelle eingefügt");
					break;
				}
			}
			else
			{
				currentItem->next = newItem;
				newItem->next = NULL;
				printf("Highscore an letzter Stelle eingefügt (%s, %d)\n", newItem->name, newItem->points);
				break;
			}
			currentItem = currentItem->next;
		}
	}

	writeHighscore(*highscoreList);
}

void addScore(SDL_Surface *screen, unsigned int points, SDL_Event event, struct highscoreItem **highscore)
{
	if (points > 0)
	{
		char name[11]; name[0] = 0;
		char nextLetter = 'A';
		int currentLetter = 0;

		SDL_Surface *askNameSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->clip_rect.w, screen->clip_rect.h, SCREEN_BPP, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

		while (SDL_WaitEvent (&event))
		{
			switch (event.type)
			{
				case SDL_USEREVENT:
					// Hintergrund animieren und Highscores anzeigen
					CLEAR_SURFACE(screen);
					renderClouds(screen, 0);
					SDL_BlitSurface(askNameSurface, NULL, screen, NULL);
					SDL_Flip(screen);
					break;

				case SDL_QUIT:
					SDL_Quit();
					exit(0);

				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						// Pfeil hoch
						case SDLK_UP:
							if (nextLetter != 0)
							{
								nextLetter--;
								if (nextLetter < 'A')
									nextLetter = 'Z';
							}
							break;

						// Pfeil runter
						case SDLK_DOWN:
							if (nextLetter != 0)
							{
								nextLetter++;
								if (nextLetter > 'Z')
									nextLetter = 'A';
							}
							break;

						case SDLK_RIGHT:
							nextLetter = 0;
							break;

						case SDLK_LEFT:
							nextLetter = name[currentLetter-1];
							break;

						case SDLK_RETURN:
							if (nextLetter != 0)
							{
								name[currentLetter] = nextLetter;
								name[currentLetter + 1] = 0;
								if (currentLetter < 9)
									currentLetter++;
								printf("%s, %c, %d\n", name, nextLetter, currentLetter);
							}
							else
							{
								printf("highscore speichern\n");
								insertHighscore(highscore, name, points);
								return;
							}
							break;

						case SDLK_ESCAPE:
							return;

						default:
							break;
					}
			}
		}
	}
	else
		printf("verloren\n");
}
