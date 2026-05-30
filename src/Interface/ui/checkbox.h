/**
 * @file ui/checkbox.h
 * @brief Case à cocher avec label.
 */

#ifndef UI_CHECKBOX_H
#define UI_CHECKBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "../Struct.h"

/**
 * @brief Case à cocher avec coordonnées de référence et état coché/décoché.
 */
typedef struct {
    SDL_Rect rect;    /**< Rectangle mis à l'échelle. */
    SDL_Rect ref;     /**< Rectangle de référence à 1280×720. */
    int      checked; /**< 1 = cochée, 0 = décochée. */
} Checkbox;

/**
 * @brief Met à l'échelle une Checkbox après un redimensionnement de fenêtre.
 * @param cb     Checkbox à mettre à l'échelle.
 * @param ratioX Ratio horizontal.
 * @param ratioY Ratio vertical.
 */
void ScaleCheckbox(Checkbox* cb, float ratioX, float ratioY);

/**
 * @brief Teste si un clic tombe dans la zone de la Checkbox.
 * @param cb Checkbox cible.
 * @param x  Coordonnée X du clic.
 * @param y  Coordonnée Y du clic.
 * @return true si (x, y) est dans cb->rect.
 */
bool isChecked(const Checkbox* cb, int x, int y);

/**
 * @brief Inverse l'état coché/décoché de la Checkbox.
 * @param cb Checkbox à basculer.
 */
void toggleCheckbox(Checkbox* cb);

/**
 * @brief Dessine la Checkbox et son label optionnel.
 *
 * Dessine un carré vide ou rempli selon cb->checked, suivi du label
 * à droite si @p font et @p label sont non-NULL.
 *
 * @param renderer Renderer SDL.
 * @param font     Police pour le label (peut être NULL).
 * @param cb       Checkbox à dessiner.
 * @param label    Texte affiché à droite de la case (peut être NULL).
 * @param color    Couleur RGBA de la case et du label.
 */
void renderCheckbox(SDL_Renderer* renderer, TTF_Font* font,
                    const Checkbox* cb, const char* label, SDL_Color color);

#endif
