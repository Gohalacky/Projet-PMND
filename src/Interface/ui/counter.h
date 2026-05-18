/**
 * @file ui/counter.h
 * @brief Compteur numérique borné avec boutons + et -.
 */

#ifndef UI_COUNTER_H
#define UI_COUNTER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../Struct.h"

/**
 * @brief Initialise un Counter avec ses bornes, sa valeur de départ et ses positions.
 *
 * @param c          Counter à initialiser.
 * @param min        Valeur minimale.
 * @param max        Valeur maximale.
 * @param startValue Valeur initiale (doit être dans [min, max]).
 * @param xPlus      Coordonnée X du bouton +.
 * @param yPlus      Coordonnée Y du bouton +.
 * @param xMoins     Coordonnée X du bouton -.
 * @param yMoins     Coordonnée Y du bouton -.
 * @param w          Largeur des boutons.
 * @param h          Hauteur des boutons.
 * @param xText      Coordonnée X du label de valeur.
 * @param yText      Coordonnée Y du label de valeur.
 */
void initCounter(Counter* c, int min, int max, int startValue,
                 int xPlus, int yPlus, int xMoins, int yMoins,
                 int w, int h, int xText, int yText);

/**
 * @brief Incrémente ou décrémente la valeur selon le bouton cliqué.
 *
 * Respecte les bornes min et max.
 *
 * @param c      Counter à mettre à jour.
 * @param mouseX Coordonnée X du clic.
 * @param mouseY Coordonnée Y du clic.
 */
void handleCounter(Counter* c, int mouseX, int mouseY);

/**
 * @brief Dessine les boutons + / - et la valeur courante du compteur.
 * @param renderer Renderer SDL.
 * @param c        Counter à dessiner.
 * @param font     Police utilisée pour les libellés.
 */
void renderCounter(SDL_Renderer* renderer, Counter* c, TTF_Font* font);

#endif
