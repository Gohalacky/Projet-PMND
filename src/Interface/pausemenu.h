/**
 * @file pausemenu.h
 * @brief Menu pause : bouton  et overlay Reprendre / Sauvegarder & Quitter.
 *
 * Le bouton (pauseMenu) est toujours visible sur le plateau.
 * Quand l'état passe à PAUSE, renderPauseMenu() affiche l'overlay complet.
 */

#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Struct.h"
#include "view.h"
#include "assets.h"

/**
 * @brief Initialise les boutons du menu pause aux coordonnées de référence 1280×720.
 * @param btn Pack de boutons principal à compléter (pauseMenu, pauseResume, pauseSaveQuit).
 */
void initPauseButtons(ButtonsPack* btn);

/**
 * @brief Met à l'échelle les boutons du menu pause selon le ratio de redimensionnement.
 * @param btn    Pack de boutons.
 * @param ratioX Ratio horizontal (fenêtre_courante_W / 1280).
 * @param ratioY Ratio vertical   (fenêtre_courante_H / 720).
 */
void scalePauseButtons(ButtonsPack* btn, float ratioX, float ratioY);

/**
 * @brief Dessine le bouton (toujours affiché sur le plateau).
 * @param renderer Renderer SDL.
 * @param btn      Pack de boutons (utilise btn->pauseMenu).
 */
void renderPauseButton(SDL_Renderer* renderer, const ButtonsPack* btn);

/**
 * @brief Dessine l'overlay du menu pause (uniquement en état PAUSE).
 *
 * Superpose un fond semi-transparent, un panneau centré et les deux boutons
 * "Reprendre" et "Sauvegarder & Quitter".
 *
 * @param renderer Renderer SDL.
 * @param font     Police utilisée pour les libellés des boutons.
 * @param btn      Pack de boutons.
 */
void renderPauseMenu(SDL_Renderer* renderer, TTF_Font* font, const ButtonsPack* btn);

#endif
