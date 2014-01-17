// Struktur definieren für die Highscores
struct highscoreItem 
{
	char *name;
	unsigned int points;
	struct highscoreItem *next;
} highscoreItem;

void append(struct highscoreItem **list, char name[], unsigned int points);
void drawHighscore (SDL_Surface *screen, TTF_Font *font, SDL_Event event);
struct highscoreItem *loadHighscore (void);
void writeHighscore (struct highscoreItem *highscoreList);
void insertHighscore (struct highscoreItem **highscoreList, char name[], unsigned int points);
