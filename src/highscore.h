// Struktur definieren für die Highscores
struct highscoreItem 
{
	char *name;
	unsigned int points;
	struct highscoreItem *next;
} highscoreItem;

void drawHighscore (SDL_Surface *screen, TTF_Font *font, SDL_Event event);
struct highscoreItem *loadHighscore (void);
struct highscoreItem *writeHighscore (void);
