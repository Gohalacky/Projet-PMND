#ifndef GAME_ENGINE_CONSOLE_H
#define GAME_ENGINE_CONSOLE_H

#include "../Modele/player.h"
#include "../Modele/property.h"
#include "plateau.h"
#include "../Modele/prison.h"
#include "../Modele/trade.h" 

/**
 * @brief Simule un lancer de dés avec gestion du mode Admin.
 * * @param admin_mode Si 1, demande la valeur à l'utilisateur.
 * @param compteur_doubles Pointeur pour suivre les doubles consécutifs.
 * @return int Somme des deux dés.
 */
int engine_roll_dice(int admin_mode, int *compteur_doubles);

/**
 * @brief Gère l'intégralité du tour d'un joueur en mode console.
 * Inclut les dialogues printf/scanf pour les actions.
 */
void engine_play_turn(Player *p, Player *all_players[], Property *tableau_prop[], 
                      Gares *tableau_gare[], Compagnies *tableau_comp[], 
                      Plateau *plateau, Prison *jail, int admin_mode, int nb_joueurs);

/**
 * @brief Analyse la case d'arrivée et propose les options d'achat/loyer/construction.
 */
void engine_resolve_case(Player *p, Player *all_players[], Property *tableau_prop[], 
                         Gares *tableau_gare[], Compagnies *tableau_comp[], 
                         Plateau *plateau, Prison *jail, int total_des);

// Actions et Affichage
void display_player_status(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]);
void engine_handle_purchase(Player *p, void *asset, int type);
void engine_handle_trade(Player *p_actif, Player *all_players[], int nb_joueurs);
void engine_manage_assets(Player *p, Property *tableau_prop[]);

#endif