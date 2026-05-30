/**
 * @file rules.h
 * @brief Menu d'affichage du règlement du jeu.
 *
 * Charge @c data/rules.txt ligne par ligne et propose un défilement vertical.
 */

#ifndef RULES_H
#define RULES_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Struct.h"
#include "assets.h"

#define RULES_MAX_LINES 256 /**< Nombre maximum de lignes de texte chargées. */
#define RULES_VISIBLE    18 /**< Nombre de lignes visibles simultanément. */

/**
 * @brief État interne du menu règlement.
 */
typedef struct RulesMenuState {
    char lines[RULES_MAX_LINES][256]; /**< Lignes de texte chargées depuis rules.txt. */
    int  count;                       /**< Nombre de lignes effectivement chargées. */
    int  scroll;                      /**< Index de la première ligne visible. */
    Button btnBack;                   /**< Bouton "< Retour" vers le menu principal. */
} RulesMenuState;

/**
 * @brief Charge @c data/rules.txt et initialise les boutons.
 *
 * Doit être appelé une seule fois au démarrage.  Si le fichier est absent,
 * le menu s'affiche vide.
 *
 * @param rm État du menu à initialiser.
 */
void initRulesMenu(RulesMenuState* rm);

/**
 * @brief Met à l'échelle les boutons après un redimensionnement de fenêtre.
 * @param rm État du menu.
 * @param rx Ratio horizontal.
 * @param ry Ratio vertical.
 */
void scaleRulesMenu(RulesMenuState* rm, float rx, float ry);

/**
 * @brief Fait défiler le texte via la molette de la souris.
 * @param rm    État du menu.
 * @param delta Sens de défilement : positif = vers le haut, négatif = vers le bas.
 */
void handleRulesMenuScroll(RulesMenuState* rm, int delta);

/**
 * @brief Gère un clic dans le menu règlement.
 * @param rm    État du menu.
 * @param x     Coordonnée X du clic.
 * @param y     Coordonnée Y du clic.
 * @param state Etat de l'interface.
 */
void handleRulesMenuClick(RulesMenuState* rm, int x, int y, EtatJeu* state);

/**
 * @brief Dessine le panneau règlement complet.
 * @param renderer   Renderer SDL.
 * @param fontTitle  Police titre.
 * @param fontNormal Police corps.
 * @param fontBtn    Police boutons.
 * @param rm         État du menu.
 * @param tex        Pack de textures.
 */
void renderRulesMenu(SDL_Renderer* renderer,
                     TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontBtn,
                     const RulesMenuState* rm, const TexturesPack* tex);

#endif
