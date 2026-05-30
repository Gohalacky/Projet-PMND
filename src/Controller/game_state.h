/**
 * @file game_state.h
 * @brief Définition du contexte global de la partie (GameContext).
 *
 * Ce module centralise l'intégralité de l'état d'une partie en une seule
 * structure : joueurs, propriétés, pioches de cartes, événement mondial actif
 * et flags de blocage par joueur. Le GameContext est passé par pointeur à tous
 * les modules du Contrôleur, garantissant un état cohérent et évitant toute
 * variable globale.
 */
#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "../Model/player.h"
#include "../Model/property.h"
#include "../Model/cartes.h"
#include "../Model/evenements.h"

/** @brief Nombre maximum de joueurs dans une partie. */
#define MAX_PLAYER 4

/**
 * @struct GameContext
 * @brief Contexte global regroupant tout l'état d'une partie d'INSA'poly.
 *
 * Cette structure est le point d'entrée unique pour accéder à l'état du jeu.
 * Elle est initialisée dans main() et passée à toutes les fonctions du
 * Contrôleur qui ont besoin d'accéder aux pioches, aux événements ou aux
 * flags de blocage.
 */
typedef struct {
    Player *players[MAX_PLAYER]; /**< Tableau des joueurs participants (NULL si slot vide) */
    int nb_players;              /**< Nombre de joueurs effectifs dans la partie */

    /* Références au plateau */
    Property    *properties[NB_PROPRIETE]; /**< Tableau de tous les terrains du plateau */
    Gares       *gares[NB_GARE];           /**< Tableau de toutes les gares du plateau */
    Compagnies  *compagnies[NB_COMPAGNIE]; /**< Tableau de toutes les compagnies du plateau */

    /* Systèmes de cartes et d'événements */
    PiocheChance         pioche_chance;      /**< Paquet Chance mélangé, avec gestion de l'état des cartes */
    PiocheCommunaute     pioche_communaute;  /**< Paquet Caisse de Communauté mélangé */
    EtatEvenementGlobal  evenement_mondial;  /**< État de l'événement climatique/social en cours */

    EcoGrade indice_eco_global; /**< Note écologique globale (A–F), moyenne des indices joueurs */

    /**
     * @brief Flags de blocage de construction, indexés par (player->number - 1).
     *
     * Mis à 1 par la carte "Semaine de partiels" ou par un événement mondial
     * interdisant les constructions. Remis à 0 automatiquement en fin de tour
     * du joueur concerné.
     */
    int bloque_construction[MAX_PLAYER];

    int numero_round; /**< Compteur de rounds, incrémenté après que tous les joueurs ont joué */
} GameContext;

#endif
