/** Flags für die Rükgabewerte */
#define FAILURE -4
#define EXIT_GAME -3
#define EXIT_LEVEL -2
#define GAME_OVER -1
#define MENU 1
#define START_GAME 2
#define HIGHSCORES 3

#define LEVEL_OFFSET 20
#define MAX_LEVEL 80

struct resolution {unsigned short width; unsigned short height;};
#define SCREEN_BPP      		32

struct resolution res;
TTF_Font *font;
