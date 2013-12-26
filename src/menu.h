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
int draw_menu(SDL_Surface *screen, TTF_Font *font, SDL_Event event);

void settingsMenu(SDL_Surface *screen, TTF_Font *font, SDL_Event event, struct resolution *res);
