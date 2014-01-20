#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "highscore.h"
#include "main.h"

#define HIGHSCORE_PATH "resources/highscore.txt"
#define MAX_LENGTH 20
#define MAX(x,y) ((x)<=(y) ?(y) :(x))
#define HIGHSCORE_PADDING 20


// Funktion für SDL-Ausgabe
void drawHighscore (SDL_Surface *screen, TTF_Font *font, SDL_Event event, struct highscoreItem *highscoreList)
{
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGBA(screen->format, 208, 244, 247, 0));
	//Name anzeigen
	SDL_Surface *nameSurface[10];
	SDL_Surface *pointsSurface[10];

	SDL_Color color = {100,100,100,0};

	int height = 0;
	int entries = 0;

	for(entries = 0; entries < 10; entries++)
	{
		if(highscoreList == NULL)
		{
			entries--;
			break;
		}
		nameSurface[entries] = TTF_RenderText_Solid(font, highscoreList->name, color);
		char pointsString[7];
		sprintf(pointsString, "%d", highscoreList->points);
		pointsSurface[entries] = TTF_RenderText_Solid(font, pointsString, color);
		highscoreList = highscoreList->next;
		height += MAX(pointsSurface[entries]->clip_rect.h, nameSurface[entries]->clip_rect.h);
	}
	height += (entries - 1) * HIGHSCORE_PADDING;

	int printedHeight = 0;
	int menuOffset = (screen->clip_rect.h - height) / 2;
	for(int i = 0; i < entries; i++)
	{
		SDL_Rect position;
		position.x = 2 * HIGHSCORE_PADDING;
		position.y = menuOffset + printedHeight;
		SDL_BlitSurface(nameSurface[i], NULL, screen, &position);

		position.x = screen->clip_rect.w - pointsSurface[i]->clip_rect.w - 2*HIGHSCORE_PADDING;
		SDL_BlitSurface(pointsSurface[i], NULL, screen, &position);

		printedHeight += nameSurface[i]->clip_rect.h + HIGHSCORE_PADDING;
	}
	SDL_Flip(screen);
	while (SDL_WaitEvent (&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				SDL_Quit();
				exit(0);

			case SDL_KEYDOWN:
				return;
		}
	}
} 

// Funktion zum Anhängen eines Eintrags an die Liste
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
					break;
				}
			}
			else
			{
				currentItem->next = newItem;
				newItem->next = NULL;
				break;
			}
			currentItem = currentItem->next;
		}
	}
}
