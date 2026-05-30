/**
 * @file engine_logic.h
 * @brief Logique métier avancée du Contrôleur : cartes, écologie et événements mondiaux.
 *
 * Ce module fait le lien entre les données brutes du Modèle (cartes piochées,
 * indices écologiques, événements) et les actions concrètes sur le GameContext.
 * Il est volontairement séparé de game_engine_console.h pour rester indépendant
 * de l'interface (console ou SDL2).
 *
 * @note Les fonctions de ce module nécessitent un GameContext initialisé et valide.
 */
#ifndef ENGINE_LOGIC_H
#define ENGINE_LOGIC_H

#include "game_state.h"
#include "plateau.h"
#include "../Model/prison.h"

/**
 * @brief Applique l'effet d'une carte Chance ou Communauté sur un joueur.
 *
 * Interprète le champ @c effet de la carte et déclenche l'action correspondante :
 * - @c CARTE_GAIN_ECTS / @c CARTE_PERTE_ECTS : transfert de crédits (avec multiplicateur événementiel).
 * - @c CARTE_TAXE_ECO : taxe pondérée par l'indice écologique du joueur.
 * - @c CARTE_DEPLACEMENT_RELATIF / @c CARTE_DEPLACEMENT_ABSOLU : déplacement du pion
 *   suivi d'une résolution de case immédiate (appel à engine_resolve_case).
 * - @c CARTE_BLOQUE_CONSTRUCTION : active le flag ctx->bloque_construction pour ce joueur.
 * - @c CARTE_SORTIE_PRISON : la carte reste en main du joueur.
 *
 * @param ctx        Contexte global de la partie.
 * @param p          Joueur qui a pioché la carte.
 * @param c          Carte piochée à appliquer.
 * @param plateau    Plateau de jeu (nécessaire pour les déplacements).
 * @param jail       Structure prison (nécessaire pour les déplacements).
 * @param all_players Tableau de tous les joueurs (pour les effets collectifs).
 * @param nb_joueurs  Nombre de joueurs actifs.
 */
void logic_apply_card_effect(GameContext *ctx, Player *p, Carte *c,
                             Plateau *plateau, Prison *jail,
                             Player *all_players[], int nb_joueurs);

/**
 * @brief Recalcule et met à jour l'indice écologique global dans le GameContext.
 *
 * Calcule la moyenne des @c eco_index de tous les joueurs encore en jeu
 * (crédit >= 0) et stocke le résultat dans @c ctx->indice_eco_global.
 * Cette valeur influence directement la probabilité de déclenchement
 * des événements mondiaux.
 *
 * @param ctx Contexte global de la partie.
 */
void logic_update_global_eco(GameContext *ctx);

/**
 * @brief Gère le cycle de vie des événements mondiaux en fin de round.
 *
 * Cette fonction doit être appelée une fois par round, après que tous les
 * joueurs ont joué. Elle effectue dans l'ordre :
 * -# Tick de l'événement actif (décrémente sa durée ; le termine s'il expire).
 * -# Tentative de déclenchement d'un nouvel événement, avec une probabilité
 *    de base de 5% augmentée selon l'indice éco global (plus la note est
 *    mauvaise, plus le risque est élevé).
 * -# Application des effets récurrents de l'événement actif (coût fixe par
 *    joueur pour la Canicule, affichage des multiplicateurs loyers/construction).
 *
 * @param ctx Contexte global de la partie.
 */
void logic_handle_global_events(GameContext *ctx);

#endif
