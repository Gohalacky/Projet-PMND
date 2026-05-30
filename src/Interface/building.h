/**
 * @file building.h
 * @brief Rendu des bâtiments construits sur le plateau.
 */

#ifndef BUILDINGS_H
#define BUILDINGS_H

#include <SDL2/SDL.h>
#include "assets.h"
#include "bridge.h"
#include "view.h"

/**
 * @brief Dessine tous les bâtiments présents sur le plateau.
 *
 * Parcourt les propriétés du contexte de jeu et superpose la texture
 * de bâtiment appropriée (maison / immeuble / parc) sur la case correspondante.
 *
 * @param renderer Renderer SDL.
 * @param ctx      Contexte de jeu (propriétés et niveaux de construction).
 * @param tex      Pack de textures contenant les images de bâtiments.
 * @param winW     Largeur courante de la fenêtre (px).
 * @param winH     Hauteur courante de la fenêtre (px).
 */
void renderBuildings(SDL_Renderer* renderer, const ContexteJeu* ctx,
                     const TexturesPack* tex, int winW, int winH);

#endif
