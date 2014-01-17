#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#   include <SDL/SDL.h>
#else
#   include "SDL.h"
#endif

#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#include "game.h"
#include "image.h"

#define BLOCK_SET_WIDTH   10
#define BLOCK_SET_HEIGHT  10
#define BLOCK_SET_SIZE    (BLOCK_SET_WIDTH * BLOCK_SET_HEIGHT)


// Strukturen
struct object
{
	int type;                //Typ des Objekt (0 = Spieler; 1 = NPC; 2 = Statisch)
	SDL_Surface *sprite;     //Pointer auf SDL-Surface, welches die Spritemap enthält
	int posX;                //X-Position
	int posY;                //Y-Position
	int sizeX;               //Breite
	int sizeY;               //Höhe
	double v;                //Vertikal-Geschindigkeit
	short moveDir;           //Bewegungsrichtung (-1 = Links; 1 = Rechts)
	unsigned int moveSpeed;  //Bewegungsgeschwindigkeit
	int frame;               //Aktueller Animationsframe
};


struct objectListElement
{
	struct object *object;           //Pointer auf das Objekt
	struct objectListElement *next;  //Pointer auf das nächste Element
};


// Globlale Variablen
int worldSizeX;
int worldSizeY;
int blockSize;
double a;
struct {int x;} camPosition;
int score;

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

		// Block auf den Screen zeichnen
		SDL_BlitSurface(bitmap, &source, screen, &destination);
	}
}


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
	score = 0;

	// ============================== HINTERGRUND ==============================
	Uint32 color = SDL_MapRGB( screen->format, 208, 244, 247 );


	// ============================== WELT ==============================
	char filepath[50];
	sprintf(filepath, "resources/maps/level%d.map", level);
	FILE *worldFile = fopen(filepath, "r");
	if (worldFile == NULL)
		return -1;
	char *line = NULL;
	size_t len = 0;

	// erste Zeile verwerfen
	getline(&line, &len, worldFile);
	line = NULL;
	//zweite Zeile: Größe des Levels
	getline(&line, &len, worldFile);
	char *tok = strtok(line, ",");
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
	struct objectListElement *objectList;
	objectList = NULL;

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

	objectAppend(&objectList, &player);

	short playMoveLeft = 0;
	short playMoveRight = 0;
	unsigned int playerBlockX = 0;
	unsigned int playerBlockY = 0;

	// ============================== GEGNER ==============================
	struct object *enemy;

	int counter = 0;
	for(counter = 0; counter < 1; counter++)
	{
		enemy = (struct object*) malloc(sizeof(*enemy));

		enemy->type      = 1;
		enemy->posX      = blockSize * (counter + 2);
		enemy->posY      = 0;
		enemy->sizeX     = (int) ((double) 36 / 48 * blockSize);
		enemy->sizeY     = blockSize;
		enemy->v         = 0;
		enemy->moveDir   = 1;
		enemy->moveSpeed = 3;
		enemy->frame     = 0;

		enemy->sprite = player.sprite;

		objectAppend(&objectList, enemy);
	}


	// ============================== KAMERA ==============================
	// Variable ist Global
	camPosition.x = 0;

	unsigned char hitbox;
	// Main-Loop
	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			// Quit-Event: quit-Flag setzen
			case SDL_QUIT:
			{
				printf("Beende Spiel\n");
				SDL_Quit();
				exit(0);
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
				{return -1;}

				// Blocklogik
				playerBlockX = (player.posX + player.sizeX / 2) / blockSize;
				playerBlockY = (player.posY + player.sizeY / 2) / blockSize;
				switch(world[playerBlockY][playerBlockX])
				{
					// Goldene Münze
					case 50:
						world[playerBlockY][playerBlockX] = 255;
						score += 3;
						printf("Score: %u\n", score);
						break;

					// Silberne Münze
					case 51:
						world[playerBlockY][playerBlockX] = 255;
						score += 2;
						printf("Score: %u\n", score);
						break;

					// Bronzefarbene Münze
					case 52:
						world[playerBlockY][playerBlockX] = 255;
						score += 1;
						printf("Score: %u\n", score);
						break;

					// Roter Schalter
					case 53:
						if(player.v > 0)
						{world[playerBlockY][playerBlockX] = 54;}
						break;

					// Levelausgang
					case 55:
						return score;
						break;

					// Sprungfeder (Oben)
					case 56:
						if(player.v > 0)
						{world[playerBlockY][playerBlockX] = 57;}
						break;

					// Sprungfeder (Unten)
					case 57:
						player.v = 2 * -9 * ((double) blockSize / 48);
						world[playerBlockY][playerBlockX] = 56;
						break;
				}

				// Kollision zwischen Spieler und Objekten
				liste=objectList->next;
				while (liste != NULL)
				{
					hitbox = objectCollisionToObject(&player, liste->object);

					if(hitbox) 
					{
						if((hitbox == 4 || hitbox == 8 || hitbox == 12) && player.v != 0)
						{
							player.v = -9 * ((double) blockSize / 48);

							struct objectListElement *tmp = liste->next;
							printf("Gegner %p besiegt!!\n", liste->object);
							objectDelete(&objectList, &liste->object);
							liste = tmp;
						}
						else
						{
							return -1;
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

					// Pfeil hoch
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

					// Escape
					case SDLK_ESCAPE:
						return 0;
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

	return 0;
}
