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
unsigned short blockSize;
double a;
struct {int x;} camPosition;


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


int blockGet(int *world, int y, int x)
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


void objectCollisionAndGravity(struct object *object, int *world)
{
	// Bewegung
	if(object->type == 0 || (object->type == 1 && object->v == 0))
	{object->posX = object->posX + object->moveDir * (int) object->moveSpeed * ((double) blockSize / 48);}

	// Kontrollieren ob Welt verlassen wurde (in horizontaler Richtung)
	if(object->posX < 0)
	{object->posX = 0;}
	else if(object->posX > (worldSizeX-1)*blockSize)
	{object->posX = (worldSizeX-1)*blockSize;}

	// X-Kollision
	if(blockGet(world, object->posY/blockSize, object->posX/blockSize) < 128)
	{object->posX = object->posX-object->posX%blockSize+blockSize; if(object->type){object->moveDir = -object->moveDir;}}
	// verhindere das setzen auf einen Block wenn man gegen diesen springt
	if(object->posY%blockSize && blockGet(world, object->posY/blockSize+1, object->posX/blockSize) < 128)
	{object->posX = object->posX-object->posX%blockSize+blockSize; if(object->type){object->moveDir = -object->moveDir;}}

	// X-Kollision von der anderen Seite
	if(blockGet(world,object->posY/blockSize, object->posX/blockSize+1) < 128)
	{object->posX = object->posX-object->posX%blockSize; if(object->type){object->moveDir = -object->moveDir;}}
	// verhindere das setzen auf einen Block wenn man gegen diesen springt
	if(object->posY%blockSize && blockGet(world, object->posY/blockSize+1, object->posX/blockSize+1) < 128)
	{object->posX = object->posX-object->posX%blockSize; if(object->type){object->moveDir = -object->moveDir;}}

	// Gravitation
	if(object->posY <= worldSizeY*blockSize)
	{
		object->v += a;
		object->posY += object->v;
	}

	// Y-Kollision: nicht durch Blöcke hindurchfallen
	if(blockGet(world, object->posY/blockSize+1, object->posX/blockSize) < 128)
	{object->posY = object->posY-object->posY%blockSize;  object->v = 0;}
	if(object->posX%blockSize && blockGet(world, object->posY/blockSize+1, object->posX/blockSize+1) < 128)
	{object->posY = object->posY-object->posY%blockSize; object->v = 0;}

	if(blockGet(world, object->posY/blockSize, object->posX/blockSize) < 128)
	{object->posY = object->posY-object->posY%blockSize+blockSize;  object->v = 0;}
	if(object->posX%blockSize && blockGet(world, object->posY/blockSize, object->posX/blockSize+1) < 128)
	{object->posY = object->posY-object->posY%blockSize+blockSize; object->v = 0;}
}


unsigned short objectCollisionToObject(struct object *object1, struct object *object2)
{
	unsigned short hitbox = 0;

	if( object1->posX                  >= object2->posX                  &&
	    object1->posX                  <= object2->posX + object2->sizeX &&
	    object1->posY                  >= object2->posY                  &&
	    object1->posY                  <= object2->posY + object2->sizeY    )
	{hitbox += 1;}

	if( object1->posX + object1->sizeX >= object2->posX                  &&
	    object1->posX + object1->sizeX <= object2->posX + object2->sizeX &&
	    object1->posY                  >= object2->posY                  &&
	    object1->posY                  <= object2->posY + object2->sizeY    )
	{hitbox += 2;}

	if( object1->posX                  >= object2->posX                  &&
	    object1->posX                  <= object2->posX + object2->sizeX &&
	    object1->posY + object1->sizeY >= object2->posY                  &&
	    object1->posY + object1->sizeY <= object2->posY + object2->sizeY    )
	{hitbox += 4;}

	if( object1->posX + object1->sizeX >= object2->posX                  &&
	    object1->posX + object1->sizeX <= object2->posX + object2->sizeX &&
	    object1->posY + object1->sizeY >= object2->posY                  &&
	    object1->posY + object1->sizeY <= object2->posY + object2->sizeY    )
	{hitbox += 8;}

	return hitbox;
}


int startGame(SDL_Surface *screen, SDL_Event event, struct resolution res, int level)
{
	int quit = 0;


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

	int world[worldSizeY][worldSizeX];
	int column = 0;
	int row = 0;

	// lese restliche Zeilen: das eigentliche Level
	while (row < worldSizeY)
	{
		getline(&line, &len, worldFile);
		printf("Zeile: %s\n", line);
		
		tok = strtok(line, ",");
		column = 0;
		// lese einzelnen Elemente ein
		while (tok)
		{
			printf("Token: %s\n", tok);
			sscanf(tok, "%d", &world[row][column]);
			
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
	sprintf(filepath, "resources/images/blocks_%d.png", blockSize);
	SDL_Surface *blockset = IMG_Load(filepath);


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
	player.sizeX     = 36;
	player.sizeY     = 48;
	player.v         = 0;
	player.moveDir   = 0;
	player.moveSpeed = 0;
	player.frame     = 0;

	// lade Grafik für den Spieler
	player.sprite = IMG_Load("resources/images/player.png");

	objectAppend(&objectList, &player);

	short playMoveLeft = 0;
	short playMoveRight = 0;

	// ============================== GEGNER ==============================
	struct object *enemy;

	int counter = 0;
	for(counter = 0; counter < 1; counter++)
	{
		enemy = (struct object*) malloc(sizeof(*enemy));

		enemy->type      = 1;
		enemy->posX      = blockSize * (counter + 2);
		enemy->posY      = 0;
		enemy->sizeX     = 36;
		enemy->sizeY     = 48;
		enemy->v         = 0;
		enemy->moveDir   = 1;
		enemy->moveSpeed = 3;
		enemy->frame     = 0;

		// lade Grafik für den Spieler
		enemy->sprite = IMG_Load("resources/images/player.png");

		objectAppend(&objectList, enemy);
	}


	// ============================== KAMERA ==============================
	// Variable ist Global
	camPosition.x = 0;

	short hitbox;
	// Main-Loop
	while(SDL_WaitEvent (&event))
	{
		switch(event.type)
		{
			// Quit-Event: quit-Flag setzen
			case SDL_QUIT:
			{
				quit = 1;
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
				objectCollisionAndGravity( liste->object, &world);
				while (liste->next != NULL)
				{
					liste=liste->next;
					objectCollisionAndGravity( liste->object, &world);
				}

				// Spieler in vertikaler Richtung aus der Welt
				if(player.posY < 0-blockSize || player.posY > worldSizeY*blockSize)
				{return 1;}

				// Kollision zwischen Spieler und Objekt
				liste=objectList;
				while (liste->next != NULL)
				{
					liste=liste->next;

					hitbox = objectCollisionToObject(&player, liste->object);

					if(hitbox) 
					{
						printf("%d\n", hitbox);
						liste->object->moveDir = -liste->object->moveDir;
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
						blockDraw(screen, blockset, x, y, blockGet(&world[0][0], y, x));
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
					// Leertaster
					case SDLK_SPACE:
						if(player.v == 0)
							{player.v = -9 * ((double) blockSize / 48);}
						break;
					// Pfeil hoch
					case SDLK_UP:
						a *= -1;
						break;
					// Escape
					case SDLK_ESCAPE:
						quit = 1;
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

		// wenn das quit-Flag gesetzt ist beenden
		if(quit){return 0;}
	}

	return 1;
}
