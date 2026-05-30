/**
 * @file pawn.h
 * @brief Gestion et rendu des pions joueurs sur le plateau.
 *
 * Les pions se déplacent case par case via une animation linéaire.
 * startPawnMove() initialise le déplacement ; updatePawns() avance les
 * animations chaque frame ; renderPawns() les dessine.
 */

#ifndef PAWN_H
#define PAWN_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "bridge.h"
#include "assets.h"

#define BOARD_SIZE 40           /**< Nombre de cases du plateau. */
#define MAX_PLAYERS 4           /**< Nombre maximum de joueurs. */
#define PAWN_REF_W 32           /**< Largeur de référence d'un pion (px). */
#define PAWN_REF_H 32           /**< Hauteur de référence d'un pion (px). */
#define PAWN_ANIM_SPEED 400.0f  /**< Vitesse de déplacement entre cases (px/s). */
#define PAWN_OFFSET_X 20        /**< Décalage X intra-case pour éviter la superposition. */
#define PAWN_OFFSET_Y 20        /**< Décalage Y intra-case pour éviter la superposition. */

/**
 * @brief État d'animation d'un pion en déplacement.
 */
typedef struct {
    bool  active;        /**< true si une animation est en cours. */
    float srcX, srcY;    /**< Coordonnées de départ (px). */
    float dstX, dstY;    /**< Coordonnées d'arrivée de la prochaine case (px). */
    float curX, curY;    /**< Position courante (px). */
    int   stepsRemaining; /**< Nombre de cases restant à parcourir. */
    int   nextCase;      /**< Index de la case cible pour ce pas. */
    int   direction;     /**< +1 = avance, -1 = recule. */
} PawnAnim;

/**
 * @brief Pion d'un joueur sur le plateau.
 */
typedef struct {
    int      playerIndex;  /**< Index du joueur propriétaire. */
    int      currentCase;  /**< Case sur laquelle se trouve le pion (0–39). */
    float    x, y;         /**< Position de rendu courante (px, mis à l'échelle). */
    PawnAnim anim;         /**< État d'animation en cours. */
} Pawn;

/**
 * @brief Initialise les pions aux coordonnées de la case de départ (case 0).
 * @param pawns     Tableau de pions à initialiser.
 * @param nbJoueurs Nombre de joueurs actifs (1–MAX_PLAYERS).
 */
void initPawns(Pawn pawns[MAX_PLAYERS], int nbJoueurs);

/**
 * @brief Dessine tous les pions actifs sur le plateau.
 * @param renderer  Renderer SDL.
 * @param tex       Pack de textures.
 * @param pawns     Tableau de pions.
 * @param nbJoueurs Nombre de joueurs actifs.
 * @param winW      Largeur courante de la fenêtre (px).
 * @param winH      Hauteur courante de la fenêtre (px).
 */
void renderPawns(SDL_Renderer* renderer, const TexturesPack* tex,
                 Pawn pawns[MAX_PLAYERS], int nbJoueurs, int winW, int winH);

/**
 * @brief Avance toutes les animations de pions d'un delta-temps.
 * @param pawns     Tableau de pions.
 * @param nbJoueurs Nombre de joueurs actifs.
 * @param deltaMs   Temps écoulé depuis le dernier appel (ms).
 */
void updatePawns(Pawn pawns[MAX_PLAYERS], int nbJoueurs, Uint32 deltaMs);

/**
 * @brief Lance l'animation de déplacement d'un pion vers l'avant.
 * @param pawn      Pion à animer.
 * @param fromCase  Case de départ.
 * @param steps     Nombre de cases à avancer.
 */
void startPawnMove(Pawn* pawn, int fromCase, int steps);

/**
 * @brief Lance l'animation de déplacement suite à l'effet d'une carte.
 *
 * Peut aller vers l'avant ou l'arrière selon la position cible.
 * @param pawn      Pion à animer.
 * @param fromCase  Case de départ.
 * @param steps     Nombre de cases (signé : positif = avance, négatif = recule).
 */
void startPawnMoveCard(Pawn* pawn, int fromCase, int steps);

/**
 * @brief Indique si un pion est actuellement en déplacement.
 * @param pawn Pion à tester.
 * @return true si une animation est en cours.
 */
bool isPawnMoving(const Pawn* pawn);

/**
 * @brief Téléporte instantanément un pion sur une case donnée.
 *
 * Recalcule la position de rendu en tenant compte des autres pions sur
 * la même case pour éviter la superposition.
 *
 * @param pawn      Pion à déplacer.
 * @param caseIndex Case de destination (0–39).
 * @param pawns     Tableau complet (pour calcul du décalage).
 * @param nbJoueurs Nombre de joueurs actifs.
 */
void snapPawnToCase(Pawn* pawn, int caseIndex, Pawn pawns[MAX_PLAYERS], int nbJoueurs);

/**
 * @brief Retourne les coordonnées de rendu du centre d'une case.
 * @param caseIndex Index de la case (0–39).
 * @param outX      Coordonnée X résultante (px, résolution de référence).
 * @param outY      Coordonnée Y résultante (px, résolution de référence).
 */
void getPawnCasePos(int caseIndex, float* outX, float* outY);

#endif
