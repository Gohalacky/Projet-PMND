/**
 * @file ui/playercard.h
 * @brief Cartes d'information joueur affichées en bordure du plateau.
 *
 * Chaque joueur dispose d'une carte affichant son numéro, son solde en crédits
 * et sa note d'éco.  Les positions de référence varient selon le nombre de joueurs.
 */

#ifndef UI_PLAYERCARD_H
#define UI_PLAYERCARD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "../Struct.h"

/**
 * @brief Position et dimensions de référence d'une carte joueur.
 */
typedef struct {
    int x, y; /**< Coordonnées du coin supérieur gauche. */
    int w, h; /**< Dimensions de la carte. */
} CardRef;

extern const CardRef CARDS_2J[2]; /**< Positions des cartes pour 2 joueurs. */
extern const CardRef CARDS_3J[3]; /**< Positions des cartes pour 3 joueurs. */
extern const CardRef CARDS_4J[4]; /**< Positions des cartes pour 4 joueurs. */

/**
 * @brief Dessine la carte d'un joueur dans le rectangle @p rect.
 *
 * Affiche : "Etudiant N", "CR : X" et "Eco : Y" (lettre A–F selon noteEco).
 * La police est celle passée en paramètre.
 *
 * @param renderer Renderer SDL.
 * @param font     Police à utiliser.
 * @param j        Données du joueur.
 * @param numero   Numéro du joueur affiché (1-based).
 * @param rect     Rectangle de destination mis à l'échelle.
 */
void renderPlayerCard(SDL_Renderer* renderer, TTF_Font* font,
                      const Joueur* j, int numero, SDL_Rect rect);

#endif
