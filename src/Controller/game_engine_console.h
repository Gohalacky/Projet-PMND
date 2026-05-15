#ifndef GAME_ENGINE_CONSOLE_H
#define GAME_ENGINE_CONSOLE_H

#include "../Model/player.h"
#include "../Model/property.h"
#include "plateau.h"
#include "game_state.h"
#include "../Model/prison.h" 
#include "../Model/trade.h"
#include "../Model/hypo.h"

/**
 * @brief Simule un lancer de dés avec gestion du mode Admin.
 * @param admin_mode Si 1, demande la valeur à l'utilisateur.
 * @param compteur_doubles Pointeur pour suivre les doubles consécutifs.
 * @return int Somme des deux dés.
 */
int engine_roll_dice(int admin_mode, int *compteur_doubles);

/**
 * @brief Gère l'intégralité du tour d'un joueur en mode console.
 * Inclut les dialogues printf/scanf pour les actions.
 * @param ctx Contexte global du jeu (pioches, événements, flags).
 */
void engine_play_turn(Player *p, Player *all_players[], Property *tableau_prop[],
                      Gares *tableau_gare[], Compagnies *tableau_comp[],
                      Plateau *plateau, Prison *jail, int admin_mode, int nb_joueurs,
                      GameContext *ctx);

/**
 * @brief Analyse la case d'arrivée et propose les options d'achat/loyer/construction.
 * @param ctx Contexte global (événements actifs pour multiplicateurs de loyer).
 */
void engine_resolve_case(Player *p, Player *all_players[], Property *tableau_prop[],
                         Gares *tableau_gare[], Compagnies *tableau_comp[],
                         Plateau *plateau, Prison *jail, int total_des,
                         GameContext *ctx);

// Actions et Affichage
void display_player_status(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[], GameContext *ctx);
void engine_handle_purchase(Player *p, void *asset, int type, Player *all_players[], int nb_joueurs);
void engine_handle_trade(Player *p_actif, Player *all_players[], int nb_joueurs, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]);
void engine_manage_assets(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[], GameContext *ctx);
void engine_handle_bankruptcy(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]);
int engine_get_player_total_value(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]);
void engine_resolve_debt(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]);
int engine_has_assets_left(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]);
void vider_buffer();
int engine_group_has_buildings(Property *tableau_prop[], Color couleur);

#endif