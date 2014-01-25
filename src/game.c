#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#ifndef MAIN_H
#include "main.h"
#define MAIN_H
#endif

#include "game.h"
#include "image.h"
#include "menu.h"
#include "sound.h"

// Schaltet die Debugfunktionen ein
//#define DEBUG

#define BLOCK_SET_WIDTH   10
#define BLOCK_SET_HEIGHT  10
#define BLOCK_SET_SIZE    (BLOCK_SET_WIDTH * BLOCK_SET_HEIGHT)


/** 
 * Struktur für Objekte, wie den Spieler und die Gegner
 */
struct object
{
	int type;                /*!< Typ des Objekt (0 = Spieler; 1 = NPC; 2 = Statisch) */
	SDL_Surface *sprite;     /*!< Pointer auf SDL-Surface, welches die Spritemap enthält */
	int posX;                /*!< X-Position */
	int posY;                /*!< Y-Position */
	int sizeX;               /*!< Breite */
	int sizeY;               /*!< Höhe */
	double v;                /*!< Vertikal-Geschwindigkeit */
	short moveDir;           /*!< Bewegungsrichtung (-1 = Links; 1 = Rechts) */
	unsigned int moveSpeed;  /*!< Bewegungsgeschwindigkeit */
	int frame;               /*!< Aktueller Animationsframe */
};


/** 
 * Struktur für ein Listenelement der Objektliste
 */
struct objectListElement
{
	struct object *object;           /*!< Pointer auf das Objekt */
	struct objectListElement *next;  /*!< Pointer auf das nächste Element */
};


/** 
 * Struktur für Trigger
 * Beim Auslösen eines Triggers auf einem Block, wird ein andere modifiziert.
 */
struct trigger
{
	unsigned int triggerX;      /*!< X-Position des Triggers */
	unsigned int triggerY;      /*!< Y-Position des Triggers */
	unsigned int targetX;       /*!< X-Position des Targets */
	unsigned int targetY;       /*!< Y-Position des Targets */
	unsigned char targetBlock;  /*!< Zielblock des Targets */
	struct trigger *next;       /*!< Pointer auf das nächste Element */
};


// Globlale Variablen
int worldSizeX;
int worldSizeY;
int blockSize;
double a;
struct {int x;} camPosition;
int score;
int exitCode;

/**
 * \brief Zeichne einen Block auf den Bildschirm
 *
 * Diese Funktion zeichnet einen Block aus der Spritemap an einen definierten Ort auf den Bildschirm
 * @param *screen Pointer auf die SDL_Surface des Bildschirms
 * @param *bitmap Pointer auf die SDL_Surface, welche die Spritemap enthält
 * @param x X-Position des Blocks, der gezeichnet werden soll
 * @param y Y-Position des Blocks, der gezeichnet werden soll
 * @param blocktype Typ des Blocks, der gezeichnet werden soll
 */
void blockDraw(SDL_Surface *screen, SDL_Surface *bitmap, int x, int y, int blocktype)
{
	if(blocktype >= 0 && blocktype <= BLOCK_SET_SIZE)
	{
		// Ausschnitt aus der Blöckebitmap welche einen Block beinhaltet
		SDL_Rect source;
		source.x = blocktype % BLOCK_SET_WIDTH * blockSize;
		source.y = blocktype / BLOCK_SET_HEIGHT * blockSize;
		source.w = blockSize;
		source.h = blockSize;

		// Ausschnitt aus dem Bildschirminhalt wo der Block gezeichnet werden soll
		SDL_Rect destination;
		destination.x = x*blockSize - camPosition.x;
		destination.y = y*blockSize;
		destination.w = blockSize;
		destination.h = blockSize;

		// Block auf den Bildschirm zeichnen
		SDL_BlitSurface(bitmap, &source, screen, &destination);
	}
}


/**
 * \brief Rufe den Typ eines Blocks in der Welt ab
 *
 * Diese Funktion gibt den Typ eines Blocks in der Welt zurück.
 * @param *world Pointer auf das Welt-Array
 * @param x X-Position des Blocks
 * @param y Y-Position des Blocks
 * @return Typ des Blocks
 */
unsigned char blockGet(unsigned char *world, int x, int y)
{
	// wenn existierender Block -> den Typ zurückgeben
	if(x >= 0 && x < worldSizeX && y >= 0 && y < worldSizeY)
	{
		return world[worldSizeX*y+x];
	}
	else
	{
		return 255;
	}
}


/**
 * \brief Hänge einen neuen Trigger an die Triggerliste an
 *
 * @param **list Pointer auf einen Pointer auf das erste Element der Liste
 * @param triggerX X-Position des Triggers
 * @param triggerY Y-Position des Triggers
 * @param targetX X-Position des Targets
 * @param targetY Y-Position des Targets
 * @param targetBlock Zielblock des Targets
 */
void triggerAppend(struct trigger **lst, unsigned int triggerX, unsigned int triggerY, unsigned int targetX, unsigned int targetY, unsigned char targetBlock)
{
	struct trigger *newElement;
	struct trigger *lst_iter = *lst;

	newElement = (struct trigger*) malloc(sizeof(*newElement)); // erzeuge ein neues Element
	newElement->triggerX = triggerX;
	newElement->triggerY = triggerY;
	newElement->targetX = targetX;
	newElement->targetY = targetY;
	newElement->targetBlock = targetBlock;
	newElement->next = NULL; // Wichtig für das Erkennen des Listenendes

	if ( lst_iter != NULL )// sind Elemente vorhanden
	{
		while (lst_iter->next != NULL ) // suche das letzte Element
		{
			lst_iter=lst_iter->next;
		}
		lst_iter->next=newElement; // Hänge das Element hinten an
	}
	else // wenn die liste leer ist, bin ich das erste Element
	{
		*lst=newElement;
	}
}


/**
 * \brief Hänge einen neuen Trigger an die Triggerliste an
 *
 * @param **list Pointer auf einen Pointer auf das erste Element der Liste
 * @param triggerX X-Position des Triggers
 * @param triggerY Y-Position des Triggers
 * @param targetX X-Position des Targets
 * @param targetY Y-Position des Targets
 * @param targetBlock Zielblock des Targets
 */
void triggerRun(struct trigger *triggerList, unsigned char *world, unsigned int playerBlockX, unsigned int playerBlockY)
{
	struct trigger *triggerListTemp = triggerList;

	if(triggerListTemp->triggerX == playerBlockX && triggerListTemp->triggerY == playerBlockY)
	{
		world[worldSizeX*triggerListTemp->targetY+triggerListTemp->targetX] = triggerListTemp->targetBlock;
	}
	while (triggerListTemp->next != NULL)
	{
		triggerListTemp=triggerListTemp->next;

		if(triggerListTemp->triggerX == playerBlockX && triggerListTemp->triggerY == playerBlockY)
		{
			world[worldSizeX*triggerListTemp->targetY+triggerListTemp->targetX] = triggerListTemp->targetBlock;
		}
	}
}


void objectAppend(struct objectListElement **lst, struct object *object)
{
	struct objectListElement *newElement;
	struct objectListElement *lst_iter = *lst;

	newElement = (struct objectListElement*) malloc(sizeof(*newElement)); // erzeuge ein neues Element
	newElement->object = object;
	newElement->next = NULL; // Wichtig für das Erkennen des Listenendes

	if ( lst_iter != NULL )// sind Elemente vorhanden
	{
		while (lst_iter->next != NULL ) // suche das letzte Element
		{
			lst_iter=lst_iter->next;
		}
		lst_iter->next=newElement; // Hänge das Element hinten an
	}
	else // wenn die liste leer ist, bin ich das erste Element
	{
		*lst=newElement;
	}
}


void objectDelete(struct objectListElement **list, struct objectListElement *deleteElement)
{
	struct objectListElement *prevElement = *list;
	struct objectListElement *nextElement;

	if(prevElement == deleteElement)
	{
		nextElement = deleteElement->next;
		free(deleteElement);
		*list = nextElement;
	}
	else
	{
		while(prevElement->next != deleteElement)
		{
			prevElement = prevElement->next;
		}
		prevElement->next = deleteElement->next;
		free(deleteElement);
	}
}


void objectDraw(SDL_Surface *screen, struct object *object)
{
	if(object->moveSpeed && object->v == 0)
	{object->frame++;}
	else
	{object->frame = 39;}

	if(object->frame >= 35 && object->frame != 39)
	{object->frame = 0;}


	SDL_Rect source;
	source.x = object->sizeX * (object->frame / 5);
	source.w = object->sizeX;
	source.h = object->sizeY;

	if(object->moveDir > 0)
	{source.y = 0;}
	else if(object->moveDir < 0)
	{source.y = object->sizeY;}

	SDL_Rect destination;
	destination.x = object->posX - camPosition.x;
	destination.y = object->posY;
	destination.w = object->sizeX;
	destination.h = object->sizeY;


	SDL_BlitSurface(object->sprite, &source, screen, &destination);
}


void objectCollisionToWorldX(struct object *object, unsigned char *world)
{
	// Kollision nach links
	if( blockGet( world, (object->posX                    ) / blockSize,
	                     (object->posY                 + 1) / blockSize  ) < 50 ||
	    blockGet( world, (object->posX                    ) / blockSize,
	                     (object->posY + object->sizeY - 1) / blockSize  ) < 50 ||
	    object->posX < 0                                                            )
	{
		if(object->posX >= 0)
		{object->posX +=  blockSize -         (object->posX % blockSize);}
		else
		{object->posX = 0;}

		if(object->type == 1)
		{object->moveDir = -object->moveDir;}
	}

	// Kollision nach rechts
	if( blockGet( world, (object->posX + object->sizeX    ) / blockSize,
	                     (object->posY                 + 1) / blockSize  ) < 50 ||
	    blockGet( world, (object->posX + object->sizeX    ) / blockSize,
	                     (object->posY + object->sizeY - 1) / blockSize  ) < 50 ||
	    object->posX + object->sizeX > worldSizeX * blockSize                       )
	{
		if(object->posX + object->sizeX <= worldSizeX * blockSize)
		{object->posX -= (object->posX + object->sizeX    ) % blockSize;}
		else
		{object->posX = worldSizeX * blockSize - object->sizeX;}

		if(object->type == 1)
		{object->moveDir = -object->moveDir;}
	}
}


void objectCollisionToWorldY(struct object *object, unsigned char *world)
{
	// Kollsion nach oben
	if( blockGet( world, (object->posX                 + 1) / blockSize,
	                     (object->posY                    ) / blockSize  ) < 50 ||
	    blockGet( world, (object->posX + object->sizeX - 1) / blockSize,
	                     (object->posY                    ) / blockSize  ) < 50    )
	{
		object->posY +=   blockSize -         (object->posY % blockSize);
		object->v = 0;
	}

	// Kollision nach unten
	if( blockGet( world, (object->posX                 + 1) / blockSize,
	                     (object->posY + object->sizeY    ) / blockSize  ) < 50 ||
	    blockGet( world, (object->posX + object->sizeX - 1) / blockSize,
	                     (object->posY + object->sizeY    ) / blockSize  ) < 50    )
	{
		object->posY -=  (object->posY + object->sizeY    ) % blockSize;
		object->v = 0;
	}
}


unsigned char objectCollisionToObject(struct object *object1, struct object *object2)
{
	unsigned char hitbox = 0;

	// Oben-Links
	if( object1->posX                  +1 >= object2->posX                  &&
	    object1->posX                  +1 <= object2->posX + object2->sizeX &&
	    object1->posY                  +1 >= object2->posY                  &&
	    object1->posY                  +1 <= object2->posY + object2->sizeY    )
	{hitbox += 1;}

	// Oben-Rechts
	if( object1->posX + object1->sizeX -1 >= object2->posX                  &&
	    object1->posX + object1->sizeX -1 <= object2->posX + object2->sizeX &&
	    object1->posY                  +1 >= object2->posY                  &&
	    object1->posY                  +1 <= object2->posY + object2->sizeY    )
	{hitbox += 2;}

	// Unten-Links
	if( object1->posX                  +1 >= object2->posX                  &&
	    object1->posX                  +1 <= object2->posX + object2->sizeX &&
	    object1->posY + object1->sizeY -1 >= object2->posY                  &&
	    object1->posY + object1->sizeY -1 <= object2->posY + object2->sizeY    )
	{hitbox += 4;}

	// Unten-Rechts
	if( object1->posX + object1->sizeX -1 >= object2->posX                  &&
	    object1->posX + object1->sizeX -1 <= object2->posX + object2->sizeX &&
	    object1->posY + object1->sizeY -1 >= object2->posY                  &&
	    object1->posY + object1->sizeY -1 <= object2->posY + object2->sizeY    )
	{hitbox += 8;}

	return hitbox;
}


void objectCollisionAndGravity(struct object *object, unsigned char *world)
{
	// Bewegung
	if(object->type == 0 || (object->type == 1 && object->v == 0))
	{object->posX = object->posX + object->moveDir * (int) object->moveSpeed * ((double) blockSize / 48);}

	objectCollisionToWorldX(object, world);

	// Gravitation
	if(object->posY <= worldSizeY*blockSize)
	{
		object->v += a;
		object->posY += object->v;
	}

	objectCollisionToWorldY(object, world);
}


int startGame(SDL_Surface *screen, SDL_Event event, struct resolution res, int level)
{
	// ============================== LEVEL-DATEI ==============================
	char filepath[50];
	sprintf(filepath, "resources/maps/level%d.map", level);
	FILE *worldFile = fopen(filepath, "r");
	if (worldFile == NULL)
		return FAILURE;
	char *line = NULL;
	size_t len = 0;
	char *tok = NULL;


	// ============================== HINTERGRUND ==============================
	Uint32 color = SDL_MapRGB( screen->format, 208, 244, 247 );


	// ============================== WELT ==============================
	// LEVEL-Block suchen
	do
	{getline(&line, &len, worldFile);}
	while(strcmp(line, "==LEVEL==\n") != 0);

	// erste Zeile: Größe des Levels
	getline(&line, &len, worldFile);
	tok = strtok(line, ",");
	sscanf(tok, "%d", &worldSizeY);
	tok = strtok(NULL, ",");
	sscanf(tok, "%d", &worldSizeX);

	unsigned char world[worldSizeY][worldSizeX];
	int column = 0;
	int row = 0;

	// lese restliche Zeilen: das eigentliche Level
	while (row < worldSizeY)
	{
		getline(&line, &len, worldFile);
		tok = strtok(line, ",");
		column = 0;
		// lese einzelnen Elemente ein
		while (tok)
		{
			sscanf(tok, "%d", (int *)&world[row][column]);
			
			column++;
			tok = strtok(NULL, ",");
		}
		row++;
	}

	// berechne Blockgröße
	blockSize = res.height / worldSizeY;

	// Gravitation
	a = 0.6 * ((double) blockSize / 48);

	// Counter zum Zeichnen der Welt
	int x;
	int y;

	// Lade Grafik für die Blöcke
	SDL_Surface *tmp = IMG_Load("resources/images/blockset.png");
	if (tmp == NULL)
	{
		printf("Surface nicht geladen!\n");
		printf("%s\n", SDL_GetError());
		exit(-1);
	}
	else
		printf("Surface wurde geladen\n");
	// skaliere Grafik
	SDL_Surface *blockset = shrinkSurface(tmp, blockSize * BLOCK_SET_WIDTH, blockSize * BLOCK_SET_HEIGHT);
	SDL_FreeSurface(tmp);

	// ============================== OBJEKTE ==============================
	struct objectListElement *objectList = NULL;

	// Variable zum Bearbeiten der Liste
	struct objectListElement *liste;


	// ============================== SPIELER ==============================
	struct object player;
	player.type      = 0;
	player.posX      = 0;
	player.posY      = 0;
	player.sizeX     = ((double) 36/48) * blockSize;
	player.sizeY     = blockSize;
	player.v         = 0;
	player.moveDir   = 0;
	player.moveSpeed = 0;
	player.frame     = 0;

	// lade Grafik für den Spieler
	SDL_Surface *tmp1 = IMG_Load("resources/images/player_final.png");
	player.sprite = shrinkSurface(tmp1, (int)((double) 36/48 * blockSize * 10), blockSize*2);
	SDL_FreeSurface(tmp1);

	// ===== Position aus Leveldatei laden =====
	// PLAYER-Block suchen
	do
	{getline(&line, &len, worldFile);}
	while(strcmp(line, "==PLAYER==\n") != 0);

	// erste Zeile: Position des Spielers
	getline(&line, &len, worldFile);

	tok = strtok(line, ",");
	sscanf(tok, "%d", &player.posX);
	player.posX *= blockSize;

	tok = strtok(NULL, ",");
	sscanf(tok, "%d", &player.posY);
	player.posY *= blockSize;

	objectAppend(&objectList, &player);

	short playMoveLeft = 0;
	short playMoveRight = 0;
	unsigned int playerBlockX = 0;
	unsigned int playerBlockY = 0;
	unsigned int playerBlockXold = 0;
	unsigned int playerBlockYold = 0;
	unsigned char hitbox = 0;


	// ============================== GEGNER ==============================
	struct object *enemy;

	// Lade Grafik für die Gegner
	SDL_Surface *tmp2 = IMG_Load("resources/images/enemy.png");
	SDL_Surface *enemySprite = shrinkSurface(tmp2, (int)((double) 36/48 * blockSize * 10), blockSize*2);
	SDL_FreeSurface(tmp2);

	// ENEMYS-Block suchen
	do
	{getline(&line, &len, worldFile);}
	while(strcmp(line, "==ENEMYS==\n") != 0);

	getline(&line, &len, worldFile);
	while(strcmp(line, "\n") != 0)
	{
		enemy = (struct object*) malloc(sizeof(*enemy));

		enemy->type      = 1;
		enemy->sizeX     = (int) ((double) 36 / 48 * blockSize);
		enemy->sizeY     = blockSize;
		enemy->v         = 0;
		enemy->moveDir   = 1;
		enemy->moveSpeed = 3;
		enemy->frame     = 0;

		enemy->sprite = enemySprite;

		// Zeile: Position des Gegners
		tok = strtok(line, ",");
		sscanf(tok, "%d", &enemy->posX);
		enemy->posX *= blockSize;

		tok = strtok(NULL, ",");
		sscanf(tok, "%d", &enemy->posY);
		enemy->posY *= blockSize;

		objectAppend(&objectList, enemy);

		getline(&line, &len, worldFile);
	}


	// ============================== TRIGGER ==============================
	struct trigger *triggerList = NULL;

	// TRIGGERS-Block suchen
	do
	{getline(&line, &len, worldFile);}
	while(strcmp(line, "==TRIGGERS==\n") != 0);

	getline(&line, &len, worldFile);
	while(strcmp(line, "\n") != 0)
	{
		unsigned int triggerX;
		unsigned int triggerY;
		unsigned int targetX;
		unsigned int targetY;
		unsigned int targetBlock;

		tok = strtok(line, ",");
		sscanf(tok, "%u", &triggerX);

		tok = strtok(NULL, ",");
		sscanf(tok, "%u", &triggerY);

		tok = strtok(NULL, ",");
		sscanf(tok, "%u", &targetX);

		tok = strtok(NULL, ",");
		sscanf(tok, "%u", &targetY);

		tok = strtok(NULL, ",");
		sscanf(tok, "%u", &targetBlock);

		triggerAppend(&triggerList, triggerX, triggerY, targetX, targetY, (unsigned char) targetBlock);

		getline(&line, &len, worldFile);
	}


	// ============================== KAMERA ==============================
	// Variable ist Global
	camPosition.x = 0;


	// ============================== PUNKTE ==============================
	score = 0;
	int scoreRender = 0;

	SDL_Color scoreColor = {100,100,100,0};
	char scoreString[7];
	SDL_Surface *scoreText;
	SDL_Rect scorePosition;
	scorePosition.y = blockSize/2;
	scorePosition.x = blockSize/2;


	// Main-Loop
	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			// Quit-Event: quit-Flag setzen
			case SDL_QUIT:
			{
				exitCode = EXIT_GAME;
				goto cleanGame;
				break;
			}

			// Timer-Event: Bildschirm neu zeichnen
			case SDL_USEREVENT:
			{
				// ============================== LOGIK ==============================
				// Spielersteuerung
				if(playMoveLeft && playMoveRight == 0)
				{player.moveDir = -1; player.moveSpeed = 3; }
				else if(playMoveRight && playMoveLeft == 0)
				{player.moveDir = 1; player.moveSpeed = 3;}
				else
				{player.moveSpeed = 0;}

				// Bewegung für alle Objekte
				liste=objectList;
				objectCollisionAndGravity( liste->object, &world[0][0]);
				while (liste->next != NULL)
				{
					liste=liste->next;
					objectCollisionAndGravity( liste->object, &world[0][0]);
				}

				// Spieler in vertikaler Richtung aus der Welt
				if(player.posY < 0-blockSize || player.posY > worldSizeY*blockSize)
				{
					exitCode =  GAME_OVER;
					goto cleanGame;
				}

				// Blocklogik
				// Körpermitte
				playerBlockX = (player.posX + player.sizeX / 2) / blockSize;
				playerBlockY = (player.posY + player.sizeY / 2) / blockSize;

				switch(world[playerBlockY][playerBlockX])
				{
					// Goldene Münze
					case 50:
						world[playerBlockY][playerBlockX] = 255;
						playCoinSound(0);
						score += 50;
						break;

					// Silberne Münze
					case 51:
						world[playerBlockY][playerBlockX] = 255;
						playCoinSound(1);
						score += 30;
						break;

					// Bronzefarbene Münze
					case 52:
						world[playerBlockY][playerBlockX] = 255;
						playCoinSound(2);
						score += 15;
						break;

					// Roter Schalter
					case 53:
						if(player.v > 0)
						{
							world[playerBlockY][playerBlockX] = 54;

							triggerRun(triggerList, &world[0][0], playerBlockX, playerBlockY);

							score += 10;
						}
						break;

					// Levelausgang
					case 55:
						exitCode = score;
						goto cleanGame;
						break;

					// Sprungfeder (Unten)
					case 57:
						player.v = 2 * -9 * ((double) blockSize / 48);
						world[playerBlockY][playerBlockX] = 56;
						break;

					// Leere Blöcke
					case 255:
						if(playerBlockX != playerBlockXold || playerBlockY != playerBlockYold)
						{
							triggerRun(triggerList, &world[0][0], playerBlockX, playerBlockY);
						}
						break;
				}

				playerBlockXold = playerBlockX;
				playerBlockYold = playerBlockY;

				// Füße
				playerBlockX = (player.posX + player.sizeX / 2) / blockSize;
				playerBlockY = (player.posY + player.sizeY - 1)    / blockSize;
				switch(world[playerBlockY][playerBlockX])
				{ 
					// Sprungfeder (Oben)
					case 56:
						if(player.v > 0)
						{world[playerBlockY][playerBlockX] = 57;}
						break;
				}

				// Kollision zwischen Spieler und Objekten
				liste=objectList->next;
				while (liste != NULL)
				{
					hitbox = objectCollisionToObject(&player, liste->object);

					if(hitbox) 
					{
						if((hitbox == 4 || hitbox == 8 || hitbox == 12) && player.v > 0)
						{
							player.v = -9 * ((double) blockSize / 48);

							struct objectListElement *tmp = liste->next;
							objectDelete(&objectList, liste);
							liste = tmp;

							score += 25;
						}
						else
						{
							exitCode = GAME_OVER;
							goto cleanGame;
						}
					}
					else
					{
						liste=liste->next;
					}
				}

				// Kamera
				camPosition.x += ((player.posX - res.width / 2 + blockSize / 2) - camPosition.x) / 10;

				if(camPosition.x < 0)
				{camPosition.x = 0;}
				else if(camPosition.x > worldSizeX*blockSize-res.width)
				{camPosition.x = worldSizeX*blockSize-res.width;}


				// ============================== RENDERN ==============================
				// Lösche den Hintergrund
				SDL_FillRect( screen, NULL, color );

				// Wolken, TODO: Kamerabewegung
				renderClouds(screen, 0);


				// Zeichne alle Blöcke
				for(y = 0; y < res.height/blockSize; y++)
				{
					for(x = camPosition.x/blockSize; x < camPosition.x/blockSize+res.width/blockSize+2; x++)
					{
						blockDraw(screen, blockset, x, y, blockGet(&world[0][0], x, y));
					}
				}

				// Zeichne alle Objekte
				liste=objectList;
				objectDraw( screen, liste->object );
				while (liste->next != NULL)
				{
					liste=liste->next;
					objectDraw( screen, liste->object );
				}

				// Zeichne die Punktzahl
				// Punktzahl einzeln Hochzählen 
				if(scoreRender < score)
				{scoreRender++;}

				sprintf(scoreString, "%06d", scoreRender);
				scoreText = TTF_RenderText_Solid(font, scoreString, scoreColor);
				SDL_BlitSurface(scoreText, NULL, screen, &scorePosition);

				// Zeichne das berechnete Bild
				SDL_Flip( screen );
				break;
			}
			// Tasten gedrückt
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					// Pfeil-Links
					case SDLK_LEFT:
						playMoveLeft = 1;
						break;
					// Pfeil-Rechts
					case SDLK_RIGHT:
						playMoveRight = 1;
						break;
					// Springen
					case SDLK_SPACE:
						if(player.v == 0)
							{player.v = -9 * ((double) blockSize / 48);}
						break;
					case SDLK_UP:
						if(player.v == 0)
							{player.v = -9 * ((double) blockSize / 48);}
						break;

					#ifdef DEBUG
						// Schwerkraft umkehren
						case SDLK_q:
							a *= -1;
							break;
						// W
						case SDLK_w:
							if(world[(player.posY + player.sizeY / 2) / blockSize - 1][(player.posX + player.sizeX / 2) / blockSize    ] < 50)
							{  world[(player.posY + player.sizeY / 2) / blockSize - 1][(player.posX + player.sizeX / 2) / blockSize    ] = 255;}
							else
							{  world[(player.posY + player.sizeY / 2) / blockSize - 1][(player.posX + player.sizeX / 2) / blockSize    ] = 4;}
							break;
						// S
						case SDLK_s:
							if(world[(player.posY + player.sizeY / 2) / blockSize + 1][(player.posX + player.sizeX / 2) / blockSize    ] < 50)
							{  world[(player.posY + player.sizeY / 2) / blockSize + 1][(player.posX + player.sizeX / 2) / blockSize    ] = 255;}
							else
							{  world[(player.posY + player.sizeY / 2) / blockSize + 1][(player.posX + player.sizeX / 2) / blockSize    ] = 4;}
							break;
						// A
						case SDLK_a:
							if(world[(player.posY + player.sizeY / 2) / blockSize    ][(player.posX + player.sizeX / 2) / blockSize - 1] < 50)
							{  world[(player.posY + player.sizeY / 2) / blockSize    ][(player.posX + player.sizeX / 2) / blockSize - 1] = 255;}
							else
							{  world[(player.posY + player.sizeY / 2) / blockSize    ][(player.posX + player.sizeX / 2) / blockSize - 1] = 4;}
							break;
						// D
						case SDLK_d:
							if(world[(player.posY + player.sizeY / 2) / blockSize    ][(player.posX + player.sizeX / 2) / blockSize + 1] < 50)
							{  world[(player.posY + player.sizeY / 2) / blockSize    ][(player.posX + player.sizeX / 2) / blockSize + 1] = 255;}
							else
							{  world[(player.posY + player.sizeY / 2) / blockSize    ][(player.posX + player.sizeX / 2) / blockSize + 1] = 4;}
							break;
					#endif

					// Escape
					case SDLK_ESCAPE:
						exitCode = EXIT_LEVEL;
						goto cleanGame;
						break;
					// alles andere ignorieren
					default:
						break;
				}
				break;
			}
			// Taste losgelassen
			case SDL_KEYUP:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_LEFT:
						playMoveLeft = 0;
						break;
					case SDLK_RIGHT:
						playMoveRight = 0;
						break;
					default:
						break;
				}
				break;
			}
		}
	}

	cleanGame:
	// ============================== AUFRÄUMEN ==============================
	printf("Gebe Speicher frei ...");
	SDL_FreeSurface(blockset);
	SDL_FreeSurface(player.sprite);
	SDL_FreeSurface(enemySprite);
	printf(" done\n");

	return exitCode;
}
