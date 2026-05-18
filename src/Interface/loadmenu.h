/**
 * @file loadmenu.h
 * @brief Menu de chargement de parties sauvegardées.
 *
 * Affiche la liste des fichiers présents dans @c saves/, avec défilement
 * et boutons "Charger" / "Supprimer" par ligne.
 */

#ifndef LOADMENU_H
#define LOADMENU_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "Struct.h"
#include "savegame.h"
#include "assets.h"
#include "pawn.h"

#define LOAD_MAX_VISIBLE 6 /**< Nombre maximum de sauvegardes visibles simultanément. */

/**
 * @brief État interne du menu de chargement.
 */
typedef struct LoadMenuState {
    SaveInfo saves[MAX_SAVES]; /**< Liste des sauvegardes disponibles. */
    int      count;            /**< Nombre de sauvegardes chargées dans @c saves. */
    int      selected;         /**< Index de la ligne sélectionnée (-1 si aucune). */
    int      scroll;           /**< Index de la première ligne visible. */
    Button   btnScrollUp;      /**< Bouton de défilement vers le haut. */
    Button   btnScrollDown;    /**< Bouton de défilement vers le bas. */
    Button   btnBack;          /**< Bouton "< Retour" vers le menu principal. */
} LoadMenuState;

/**
 * @brief Initialise l'état du menu et scanne le répertoire de sauvegardes.
 * @param lm État du menu à initialiser.
 */
void initLoadMenu(LoadMenuState* lm);

/**
 * @brief Met à l'échelle les boutons du menu après un redimensionnement.
 * @param lm Etat du menu.
 * @param rx Ratio horizontal.
 * @param ry Ratio vertical.
 */
void scaleLoadMenu(LoadMenuState* lm, float rx, float ry);

/**
 * @brief Gère un clic dans le menu de chargement.
 *
 * Charge la partie si "Charger" est cliqué, supprime si "Supprimer",
 * fait défiler si les boutons flèche sont actionnés, retourne au menu principal
 * si "Retour" est cliqué.  En cas de chargement réussi, met à jour @p state,
 * @p partie, @p ctx et repositionne les pions.
 *
 * @param lm     État du menu.
 * @param x      Coordonnée X du clic.
 * @param y      Coordonnée Y du clic.
 * @param state  Etat de la partie.
 * @param partie État de la partie à reconstruire lors du chargement.
 * @param ctx    Contexte de jeu à reconstruire lors du chargement.
 * @param pawns  Tableau de pions à repositionner après chargement.
 */
void handleLoadMenuClick(LoadMenuState* lm, int x, int y,
                         EtatJeu* state, EtatPartie* partie,
                         ContexteJeu* ctx, Pawn pawns[MAX_PLAYERS]);

/**
 * @brief Dessine le menu de chargement complet.
 * @param renderer   Renderer SDL.
 * @param fontTitle  Police titre.
 * @param fontNormal Police corps.
 * @param fontBtn    Police boutons.
 * @param lm         État du menu.
 * @param tex        Pack de textures.
 */
void renderLoadMenu(SDL_Renderer* renderer,
                    TTF_Font* fontTitle, TTF_Font* fontNormal, TTF_Font* fontBtn,
                    const LoadMenuState* lm, const TexturesPack* tex);

#endif
