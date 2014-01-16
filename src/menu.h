#ifndef MAIN_H
#include "main.h"
#define MAIN_H
#endif



/** Abstand zwischen den einzelnen Menüeinträgen */
#define MENU_PADDING 20

/** Anzahl der verfügbaren Auflösungen */
#define RESOLUTION_NUM 5

/** Anzahl der Wolken welche im Menühintergrund animiert werden */
#define CLOUDS 10

/** Datei welche als Standardhintergrundgrafik für unausgewählte Level im Levelauswahlmenü verwendet wird (wenn kein Thumbnail existiert) */
#define MENU_BACKGROUND "resources/images/menuBackground.bmp"

/** Datei welche als Standardhintergrundgrafik für ausgewählte Level im Levelauswahlmenü verwendet wird (wenn kein Thumbnail existiert) */
#define MENU_BACKGROUND_SELECTED "resources/images/menuBackgroundSelected.bmp"

/** Flags für die Menütypen */
#define MAIN_MENU 0
#define SETTINGS_MENU 1
#define LEVEL_MENU 2

/** Flags für die Rükgabewerte */
#define START_GAME 1
#define HIGHSCORES 2

#define CLEAR_SURFACE(surf) SDL_FillRect((surf), &(surf)->clip_rect, SDL_MapRGBA((surf)->format, 0, 0, 0, 0))

/** Struktur für die animierten Wolken im Menühintergrund */
typedef struct cloud
{
	double velocity; /*!< Geschwindigkeit der Wolke */
	unsigned short height; /*!< Höhe der Wolke (relative x-Position)*/
	double position; /*!< derzeitige Y-Postion der Wolke */
	SDL_Surface *surface; /*!< Zeiger auf die Surface der Wolke */
} cloud;

/** Struktur für ein Menüeintrag, verkettete Liste */
typedef struct menuItem
{
	SDL_Rect position; /*!< Position und Größe des Menüitems */
	SDL_Surface *selected; /*!< Surface welche anzuzeigen ist wenn dieses Item ausgewählt ist */
	SDL_Surface *unselected; /*!< Surface welche anzuzeigen ist wenn das Item nicht ausgewählt ist */
	char *description; /*!< Beschreibungstext des Menüeintrages */
	struct menuItem *next; /*!< Zeiger auf das nächste Menü-Item */
} menuItem;

typedef struct menu
{
	unsigned int w;
	unsigned int h;
	unsigned int items;
	int selected;
	menuItem *next;
} menu;

/**
 * Erzeugt ein interaktives Menü und zeigt dieses an
 *
 * @param screen	Zeiger auf die SDL_Surface auf welche das Menü gezeichnet werden soll
 * @param font		Zeiger auf die TTF-Schriftart mit welcher die Menüeinträge geschrieben werden sollen
 * @param event	SDL_Event zur Erkennung von Tastatur- und Timer-Ereignissen
 *
 * @return 			ausgewählter Menüeintrag
 */
int drawMenu(SDL_Surface *screen, TTF_Font *font, SDL_Event event, struct resolution res);

/**
 * Initialisiert das Wolkenarray welches im Hintergrund der verschiedenen Menüs angezeigt wird mit zufälligen Werten.
 */
void initializeClouds(void);

/**
 * Initialisiert die verkettete Liste für das Hauptmenü
 *
 * @return Zeiger auf das erste Element der verketetten Liste der Menüeinträge
 */
menu *initializeMainMenu(struct resolution res, TTF_Font *font);

/**
 * Initialisiert die verkettete Liste für das Einstellungsmenü
 *
 * @return Zeiger auf das erste Element der verketteten Liste der Menüeinträge
 */
menu *initializeSettingsMenu(struct resolution res, TTF_Font *font);

menu *initializeListMenu(struct resolution res, TTF_Font *font, char *itemNames[], unsigned char items);

/**
 * Initialisiert die verkettete Liste für das Levelmenü
 *
 * @return Zeiger auf das erste Element der verketteten Liste der Menüeinträge
 */
menu *initializeLevelMenu(TTF_Font *font, struct resolution res);

/**
 * Rendert den nächsten Animationsschritt für die Wolken
 *
 * @param surface Zeiger auf die SDL_Surface auf welche die Wolken gerendert werden sollen
 */
void renderClouds(SDL_Surface *surface);

/**
 * Rendert das Hauptmenü
 *
 * @param surface			Zeiger auf die SDL_Surface auf welche das Menü gerendert werden soll
 * @param change			Änderungen welche seit dem letzten Mal rendern ausgeführt wurden
 * @param menu				Zeiger auf das erste Element der Menüliste
 * @param descriptionY	Y-Position der Beschreibung des ausgewählten Menüeintrages, -1 falls dieser nicht angezeigt werden soll
 * @param layout			Layout des Menüs (liste oder grid)
 *
 * @return		Index des derzeit ausgewählten MenüEintrag
 */
int renderMenu(SDL_Surface *surface, int changeH, int changeV, menu *menu, int descriptionY);

/**
 * Erzeugt ein Menüeintrag
 *
 * @param x 						X-Koordinate auf dem Bildschirm
 * @param y 						Y-Koordinate auf dem Bildschirm
 * @param text 					Text welcher innerhalb des Menüitems zentriert angezeigt werden soll, wenn kein Text angezeigt werden soll NULL
 * @param description			Text welcher in einer evtl. vorhandenen zustäzlichen Beschreibungsfläche angezeigt werden soll, wenn kein Text  angezeigt werden soll NULL
 * @param pathImageSelected	Pfad zur Bilddatei welche angezeigt werden soll wenn dieser Eintrag ausgewählt ist (PNG), wenn keine Grafik angezeigt werden soll NULL
 * @param pathImageUnselected	Pfad zur Bilddatei welche angezeigt werden soll wenn dieser Eintrag nicht ausgewählt ist (PNG), wenn keine Grafik angezeigt werden soll NULL
 *
 * @return Zeiger auf den erzeugten MenüEintrag, NULL wenn dieser nicht erzeugt werden konnte
 */
menuItem *createItem (int x, int y, const char *text, const char *description, const char* pathImageSelected, const char* pathImageUnselected, TTF_Font *font);

menu *createMenu(unsigned int w, unsigned int h, unsigned int items, menuItem *firstItem);

/**
 * gibt den von Menüeinträgen belegten Speicher wieder frei
 */
void freeMenu(menu *item);
void freeMenuItem(menuItem *item);
