#ifndef MAIN_H
#include "main.h"
#define MAIN_H
#endif

/**
 *	Diese Funktion zeigt ein Menü an
 *
 * @param screen	Pointer auf die Surface auf welcher das Menü gezeigt werden soll
 * @param font		Pointer auf die TTFont mit welcher die Menüeinträge angezeigt werden sollen
 * @param event	SDL-Event für Timer und Tastaturereignisse
 *
 * @return ausgewähltes Item beim besätigen
 */
int drawMenu(SDL_Surface *screen, TTF_Font *font, SDL_Event event);

/**
 * Diese Funktion zeigt ein Menü mit Einstellungsmöglichkeiten für die Grafik an und wendet die geänderten Einstellungen an
 *
 * @param screen	Pointer auf die Surface auf welcher das Menü gezeigt werden soll
 * @param font		Pointer auf die TTFont mit welcher die Menüeinträge angezeigt werden sollen
 * @param event	SDL-event für Timer und Tastatureingabe
 * @param res		Pointer auf Bildschirmauflösungs-Struktur
 */
void drawSettingsMenu(SDL_Surface *screen, TTF_Font *font, SDL_Event event, struct resolution *res);

/**
 * Diese Funktion zeigt ein Menü zur Auswahl des gewünschten Levels an
 *
 * @param screen	Pointer auf die Surface auf welcher das Menü gezeigt werden soll
 * @param font		Pointer auf die TTFont mit welcher die Menüeinträge angezeigt werden sollen
 * @param event	SDL-event für Timer und Tastatureingabe
 *
 * @return Nummer des Levels welches gewünscht ist
 */
int drawLevelMenu(SDL_Surface *screen, TTF_Font *font, SDL_Event event);
