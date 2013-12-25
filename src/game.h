/**
 * Diese Funktion zeichnet eine Block aus einer Grafik welche alle möglichen Blöcke beinhaltet auf den Bildschirm
 *
 * @param screen		Pointer auf die Bildschirmfläche
 * @param bitmap		Pointer auf die Grafik welche die Blöcke enthält
 * @param x				X-Position des Blocks auf dem Screen
 * @param y				Y-Position des Blocks auf dem Screen
 * @param shift		Verschiebung des Bildschirmes in x-Richtung
 * @param blocktype	Art des Blockes (daraus folgt die Position des Blockes in bitmap
 */
void drawBlock(SDL_Surface *screen, SDL_Surface *bitmap, int x, int y, int shift, int blocktype);

/**
 * Diese Funktion gibt den Typ eines Blockes aus
 *
 * @param world		Pointer auf das zweidimensionale Array welches die Welt beinhaltet
 * @param worldSizeX	Anzahl der Blöcke in X-Richtung
 * @param worldSizeY	Anzahl der Blöcke in Y-Richtung
 * @param y				y-Position des gewünschten Blockes
 * @param x				x-Position des gewünschten Blockes
 *
 * @return Art des Blockes
 */
int getBlock(int *world, int worldSizeX, int worldSizeY, int y, int x);

/**
 * Diese Funktion beinhaltet das eigentliche Spiel
 *
 * @param screen	Pointer auf die Surface auf welcher das Menü gezeigt werden soll
 * @param event	SDL-Event für Timer und Tastaturereignisse
 *
 * @return 
 */
int start_game(SDL_Surface *screen, SDL_Event event);
