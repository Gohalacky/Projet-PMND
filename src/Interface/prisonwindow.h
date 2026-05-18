/**
 * @file prisonwindow.h
 * @brief Fenêtre de gestion du tour en prison.
 *
 * Affiché en état PRISON (superposé au plateau), ce panneau permet au joueur
 * de tenter un double pour se libérer, ou d'utiliser une carte "Sortie de prison".
 */

#ifndef PRISONWINDOW_H
#define PRISONWINDOW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Struct.h"
#include "bridge.h"
#include "pawn.h"

/*Déclaration anticipée*/
struct ButtonsPack;
typedef struct ButtonsPack ButtonsPack;

/**
 * @brief Pack de boutons de la fenêtre prison.
 */
typedef struct PrisonButtonsPack {
    Button btnTenterDouble;  /**< Tente un double pour sortir de prison. */
    Button btnUtiliserCarte; /**< Utilise une carte "Sortie de prison" (grisé si absente). */
} PrisonButtonsPack;

/**
 * @brief Initialise les boutons aux coordonnées de référence 1280×720.
 * @param pb Pack à initialiser.
 */
void initPrisonButtons(PrisonButtonsPack* pb);

/**
 * @brief Met à l'échelle les boutons après un redimensionnement.
 * @param pb Pack de boutons.
 * @param rx Ratio horizontal.
 * @param ry Ratio vertical.
 */
void scalePrisonButtons(PrisonButtonsPack* pb, float rx, float ry);

/**
 * @brief Dessine la fenêtre prison (superposée au plateau).
 *
 * Affiche : nombre de tours passés en prison, tentatives restantes,
 * et les deux boutons d'action.  Le bouton "Utiliser carte" est grisé
 * si le joueur n'en possède pas.
 *
 * @param renderer   Renderer SDL.
 * @param fontTitle  Police titre.
 * @param fontNormal Police corps.
 * @param ctx        Contexte de jeu.
 * @param partie     État de la partie.
 * @param pb         Pack de boutons.
 * @param winW       Largeur courante de la fenêtre (px).
 * @param winH       Hauteur courante de la fenêtre (px).
 */
void renderPrisonWindow(SDL_Renderer* renderer, TTF_Font* fontTitle, TTF_Font* fontNormal,
                        const ContexteJeu* ctx, const EtatPartie* partie,
                        PrisonButtonsPack* pb, int winW, int winH);

/**
 * @brief Gère un clic dans la fenêtre prison.
 *
 * - "Tenter un double" : lance bridge_jail_try_double().
 *   - Résultat 1 ou 2 : libéré, animation + bridge_play_turn(), état → LANCER.
 *   - Résultat 0 : tour raté, état → PLATEAU*J.
 * - "Utiliser carte" : appelle bridge_jail_use_card() si disponible, état → PLATEAU*J.
 *
 * @param x      Coordonnée X du clic.
 * @param y      Coordonnée Y du clic.
 * @param state  Etat de l'interface.
 * @param pb     Pack de boutons prison.
 * @param btn    Pack de boutons principal.
 * @param ctx    Contexte de jeu.
 * @param partie État de la partie.
 * @param pawns  Tableau des pions (pour startPawnMove).
 */
void handlePrisonClick(int x, int y, EtatJeu* state, PrisonButtonsPack* pb, ButtonsPack* btn,
                       ContexteJeu* ctx, EtatPartie* partie, Pawn pawns[MAX_PLAYERS]);

#endif
