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

/** Makro zum löschen des gesamten Bildschirminhaltes einer Surface
 * @param surf Zeiger auf die SDL_Surface welche geleert werden soll
 */
#define CLEAR_SURFACE(surf) SDL_FillRect((surf), &(surf)->clip_rect, SDL_MapRGBA((surf)->format, 208, 244 , 247, 0))

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

/** Struktur für ein Menü.
 * In einer solchen Struktur können Metadaten über eine Menü gespeichert werden und auf den ersten Menüeintrag gezeigt werden
 */
typedef struct menu
{
	unsigned int w; /*!< Anzahl der nebeneinander liegenden Einträge */
	unsigned int h; /*!< Anzahl der übereinander liegenden Einträge */
	unsigned int items; /*!< Gesamtzahl der Einträge */
	int selected; /*!< Index des derzeit ausgewählten Eintrags */
	menuItem *next; /*!< Zeiger auf die Struktur des ersten Eintrages */
} menu;

/**
 * Erzeugt ein interaktives Menü und zeigt dieses an
 *
 * @param screen	Zeiger auf die SDL_Surface auf welche das Menü gezeichnet werden soll
 * @param event	SDL_Event zur Erkennung von Tastatur- und Timer-Ereignissen
 *
 * @return 			Index des ausgewählten Menüeintrags
 */
int drawMenu(SDL_Surface *screen, SDL_Event event);

/**
 * Initialisiert das Wolkenarray welches im Hintergrund der verschiedenen Menüs angezeigt wird mit zufälligen Werten.
 */
void initializeClouds(void);

/**
 * Initialisiert die verkettete Liste für das Hauptmenü
 *
 * @return Zeiger auf das Menüelement
 */
menu *initializeMainMenu();

/**
 * Initialisiert ein einfaches Listenmenü.
 *
 * Ein Listenmenü enthält keine Bilder sondern nur die untereinander stehenden Einträge. Diese werden zentriert bezogen zur übergebenen Auflösung angezeigt
 *
 * @param itemNames	Namen der einzelnen Einträge
 * @param items		Anzahl der Einträge
 *
 * @return Zeiger auf das Mneüelement
 */
menu *initializeListMenu(char *itemNames[], unsigned char items);

/**
 * Initialisiert die verkettete Liste für das Levelmenü.
 *
 * Bei diesem Menü wird für alle Leveldateien ein Menüeintrag angelegt.
 * Diese Menüeinträge werden als Grid mit zugehörigen Thumbnails dargestellt, wenn kein zugehöriges Thumbnail verfügbar ist wird ein Standardbild genutzt.
 * Außerdem wird aus jeder Leveldatei eine Beschreibung ausgelesen, welche unterhalb des Menüs angezeigt wird (werden soll)
 *
 * @return Zeiger auf das Menüelement
 */
menu *initializeLevelMenu();

/**
 * Rendert den nächsten Animationsschritt für die Wolken
 *
 * @param surface Zeiger auf die SDL_Surface auf welche die Wolken gerendert werden sollen
 */
void renderClouds(SDL_Surface *surface);

/**
 * Rendert die übergebene Menüliste
 *
 * @param surface			Zeiger auf die SDL_Surface auf welche das Menü gerendert werden soll
 * @param changeH			Änderung in horizontaler Richtung welche seit dem letzten Mal rendern ausgeführt wurd
 * @param changeV			Änderung in vertikaler Richtung welche seit dem letzten Mal rendern ausgeführt wurde
 * @param menu				Zeiger auf das Menüelement
 * @param descriptionY	Y-Position der Beschreibung des ausgewählten Menüeintrages, -1 falls dieser nicht angezeigt werden soll
 *
 * @return		Index des derzeit ausgewählten MenüEintrag
 */
int renderMenu(SDL_Surface *surface, int changeH, int changeV, menu *menu);

/**
 * Erzeugt ein Menüeintrag
 *
 * @param text 					Text welcher innerhalb des Menüitems zentriert angezeigt werden soll, wenn kein Text angezeigt werden soll NULL
 *
 * @return Zeiger auf den erzeugten MenüEintrag, NULL wenn dieser nicht erzeugt werden konnte
 */
menuItem *createItem (const char *text);

/**
 * Erzeugt ein Menü
 *
 * @param w				Breite des Menüs (Anzahl der Einträge in horizontaler Richtung)
 * @param h				Höhe des Menüs (Anzahl der Einträge in vertikaler Richtung)
 * @param items		Anzahl der Menüeinträge
 * @param firstItem	Zeiger auf das erste Menüelement
 *
 * @return	Zeiger auf das Menüelement
 */
menu *createMenu(unsigned int w, unsigned int h, unsigned int items, menuItem *firstItem);

/**
 * gibt den von Menüeinträgen belegten Speicher wieder frei
 *
 * @param item	Zeiger auf das Menü welches gelöscht werden soll
 */
void freeMenu(menu *item);

/**
 * Gibt den von einem Menüeintrag belegten Speicher wieder frei
 *
 * @param item	Zeiger auf den Menüeintrag
 */
void freeMenuItem(menuItem *item);

/**
 * Ändert Auflösung und Vollbild
 *
 * @param screen				Zeiger auf die Bildschirm-Surface
 * @param height				gewünschte Bildschirmhöhe, -1 wenn die Auflösung nicht geändert werden soll
 * @param width				gewünschte Bildschirmbreite, -1 wenn die Auflösung nicht geändert werden soll
 * @param toggleFullscreen	schaltet den Vollbildmodus um wenn ungleich 0
 */
void setGraphicsMode(SDL_Surface *screen, int height, int width, char toggleFullscreen);
