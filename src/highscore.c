#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "highscore.h"

#define HIGHSCORE_PATH "resources/highscore.txt"
#define MAX 20


// Funktion für SDL-Ausgabe
void drawHighscore (SDL_Surface *screen, TTF_Font *font, SDL_Event event)
{
	
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

	char tempName[MAX];
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

			printf("Name: %s -> Points: %u\n",tempName, tempPoints);
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
				printf("1\n");
				if (newItem->points > currentItem->next->points)
				{
					printf("2\n");
					newItem->next = currentItem->next;
					currentItem->next = newItem;
					break;
				}
			}
			else
			{
				printf("3\n");
				currentItem->next = newItem;
				newItem->next = NULL;
				break;
			}
			currentItem = currentItem->next;
		}
	}
}
