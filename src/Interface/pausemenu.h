#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Struct.h"
#include "view.h"
#include "assets.h"

/**
 * @file pausemenu.h
 * @brief Menu pause : bouton hamburger en haut à droite du plateau
 *        et overlay avec les actions Reprendre / Sauvegarder & Quitter.
 */

/** Initialise les boutons du menu pause (coordonnées de référence 1280×720). */
void initPauseButtons(ButtonsPack* btn);

/** Met à l'échelle les boutons du menu pause selon le ratio fenêtre. */
void scalePauseButtons(ButtonsPack* btn, float ratioX, float ratioY);

/**
 * @brief Dessine le bouton ☰ sur le plateau (toujours visible).
 * @param renderer  Renderer SDL.
 * @param font      Police à utiliser pour l'icône.
 * @param btn       Pack de boutons.
 */
void renderPauseButton(SDL_Renderer* renderer, TTF_Font* font, const ButtonsPack* btn);

/**
 * @brief Dessine l'overlay du menu pause (uniquement en état PAUSE).
 * @param renderer  Renderer SDL.
 * @param font      Police normale.
 * @param btn       Pack de boutons.
 */
void renderPauseMenu(SDL_Renderer* renderer, TTF_Font* font, const ButtonsPack* btn);

#endif 