/**
 * @file ui/textbox.h
 * @brief Champ de saisie avec curseur clignotant.
 */

#ifndef UI_TEXTBOX_H
#define UI_TEXTBOX_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../Struct.h"

/**
 * @brief Ensemble des champs de saisie de l'écran "Nouvelle partie".
 */
typedef struct {
    TextBox initialMoney; /**< Mise initiale en crédits. */
    TextBox numbTurns;    /**< Nombre de tours maximum. */
    TextBox eventFreq;    /**< Fréquence d'apparition des événements. */
} TextBoxPack;

/**
 * @brief Initialise une TextBox avec sa position de référence.
 * @param tb   TextBox à initialiser (contenu vide, curseur à 0, inactive).
 * @param refX Coordonnée X de référence à 1280×720.
 * @param refY Coordonnée Y de référence à 1280×720.
 * @param refW Largeur de référence.
 * @param refH Hauteur de référence.
 */
void initTextBox(TextBox* tb, int refX, int refY, int refW, int refH);

/**
 * @brief Met à l'échelle une TextBox après un redimensionnement.
 * @param tb     TextBox à mettre à l'échelle.
 * @param ratioX Ratio horizontal.
 * @param ratioY Ratio vertical.
 */
void ScaleTextBox(TextBox* tb, float ratioX, float ratioY);

/**
 * @brief Met à l'échelle toutes les TextBox du pack.
 * @param tboxes Pack à mettre à l'échelle.
 * @param ratioX Ratio horizontal.
 * @param ratioY Ratio vertical.
 */
void ScaleAllTextBoxes(TextBoxPack* tboxes, float ratioX, float ratioY);

/**
 * @brief Active la TextBox cliquée et désactive les autres.
 *
 * Lance SDL_StartTextInput() si une TextBox est activée,
 * SDL_StopTextInput() si aucune n'est active.
 *
 * @param tboxes  Pack de TextBox.
 * @param mouseX  Coordonnée X du clic.
 * @param mouseY  Coordonnée Y du clic.
 */
void handleTextBoxClick(TextBoxPack* tboxes, int mouseX, int mouseY);

/**
 * @brief Traite un événement SDL_TEXTINPUT ou SDL_KEYDOWN pour une TextBox active.
 *
 * Gère la saisie de caractères, Backspace, Delete, flèches gauche/droite, Home, End.
 *
 * @param event Événement SDL à traiter.
 * @param tb    TextBox active à mettre à jour.
 */
void handleTextBoxEvent(SDL_Event* event, TextBox* tb);

/**
 * @brief Dessine une TextBox avec fond, bordure et curseur clignotant.
 * @param renderer Renderer SDL.
 * @param font     Police à utiliser pour afficher le texte saisi.
 * @param tb       TextBox à dessiner.
 */
void renderTextBox(SDL_Renderer* renderer, TTF_Font* font, TextBox* tb);

#endif
