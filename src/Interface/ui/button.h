/**
 * @file ui/button.h
 * @brief Fonctions utilitaires pour les boutons.
 */

#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <stdbool.h>
#include "../Struct.h"

/**
 * @brief Recalcule button->rect à partir de button->ref et des ratios de mise à l'échelle.
 * @param btn    Bouton à mettre à l'échelle.
 * @param ratioX Ratio horizontal (fenêtre_W / REF_W).
 * @param ratioY Ratio vertical   (fenêtre_H / REF_H).
 */
void ScaleButton(Button* btn, float ratioX, float ratioY);

/**
 * @brief Teste si un clic tombe dans le rectangle d'un bouton actif.
 * @param btn Bouton cible.
 * @param x   Coordonnée X du clic.
 * @param y   Coordonnée Y du clic.
 * @return true si (x, y) est dans btn->rect et btn->disabled == 0.
 */
bool isClicked(const Button* btn, int x, int y);

/**
 * @brief Superpose un masque semi-transparent sur un bouton désactivé.
 *
 * N'affiche rien si btn->disabled == 0.
 *
 * @param renderer Renderer SDL.
 * @param btn      Bouton à griser.
 */
void renderButtonDisabled(SDL_Renderer* renderer, const Button* btn);

#endif
