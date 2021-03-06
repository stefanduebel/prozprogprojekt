/** Struktur definieren für die Highscores */
struct highscoreItem 
{
	char *name;
	unsigned int points;
	struct highscoreItem *next;
} highscoreItem;

/**
 * Funktion zum Anhängen eines Eintrags an die Liste
 *
 * @param list Liste an welche das Element angehängt werden soll
 * @param name Name des Spielers
 * @param points Punktezahl
 */
void append(struct highscoreItem **list, char name[], unsigned int points);

/**
 * Anzeigen des Highscores
 *
 * @param screen			Blidschirmfläche
 * @param event			SDL-Events
 * @param highscoreList	Liste der anzuzeigenden Highscores
 */
void drawHighscore (SDL_Surface *screen, TTF_Font *font, SDL_Event event, struct highscoreItem *highscoreList);

/**
 * Lädt die Highscores aus einer Textdatei
 *
 * @return Zeiger auf das erste Element der Highscore-Liste
 */
struct highscoreItem *loadHighscore (void);

/**
 * schreibt die Highscore-Liste in eine Textdatei
 * @param highscoreList Zeiger auf das erste Element der Highscore-Liste
 */
void writeHighscore (struct highscoreItem *highscoreList);

/**
 * Fügt einen Eintrag an der richtigen Stelle in die Highscore-Liste ein
 *
 * @param highscoreList Zeiger auf das erste Element der Liste in welche das Element eingefügt werden soll
 * @param name 			Name des einzufügenden Spielers
 * @param points			Punktzahl des einzufügenden Spielers
 */
void insertHighscore (struct highscoreItem **highscoreList, char name[], unsigned int points);

/**
 * Fragt den Spieler nach seinem Namen und fügt den Highscore zur Liste hinzu
 *
 * @param screen		Zeiger auf die SDL_Surface auf welche der Eingabedialog gezeichnet werden soll
 * @param points		Punktzahl (Return-Code des Spieles)
 * @param event		SDL_Event für Timer und Tastenevents
 * @param highscore	Highscore Liste
 *
 * @return	Flags zum weiteren Verarbeiten
 */
int addScore(SDL_Surface *screen, int points, SDL_Event event, struct highscoreItem **highscore);

/**
 * Gibt den von der HihscoreListe belegten Speicher frei
 *
 * @param highscoreList	Zeiger auf das erste zu löschende Listenelement
 */
void freeHighscore(struct highscoreItem *highscoreList);

