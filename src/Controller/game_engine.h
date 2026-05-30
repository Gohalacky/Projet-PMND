#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "../Model/player.h"
#include "../Model/property.h"
#include "plateau.h"
#include "../Model/prison.h"
#include "../Model/trade.h"  
#include "../Model/hypo.h" 

/* --- Dés --- */
int engine_roll_dice(int *compteur_doubles);
int engine_roll_and_move(Player* p, const Plateau* plateau, int *compteur_doubles);

/* --- Tour complet (version console) --- */
void engine_play_turn(Player *p, Player *all_players[], Property *tableau_prop[],
                      Gares *tableau_gare[], Compagnies *tableau_comp[], 
                      Plateau *plateau, Prison *jail, int nb_joueurs);

/* --- Résolution de case --- */
void engine_resolve_case(Player *p, Player *all_players[], Property *tableau_prop[], 
                         Gares *tableau_gare[], Compagnies *tableau_comp[], 
                         Plateau *plateau, Prison *jail, int total_des);

int engine_resolve_case_logic(Player *p, Player *all_players[], Property *tableau_prop[], 
                              Gares *tableau_gare[], Compagnies *tableau_comp[], 
                              Plateau *plateau, Prison *jail, int total_des);

/* --- Achat / Construction / Vente --- */
int engine_buy_asset(Player *p, void *asset, int type);
int engine_build_property(Player *p, Property *tableau_prop[], Property *prop, int type_batiment);
int engine_sell_building(Player *p, Property *tableau_prop[], Property *prop);
int engine_group_has_buildings(Property *tableau_prop[], Color couleur);

/* --- Hypothèques --- */
int engine_mortgage_prop(Player *p, Property *tableau_prop[], Property *prop);
int engine_unmortgage_prop(Player *p, Property *prop);

/* --- Dette / Banqueroute --- */
int  engine_has_assets_left(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]);
void engine_handle_bankruptcy(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]);
int  engine_check_bankruptcy_status(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]);

/* --- Valorisation --- */
int engine_get_player_total_value(Player *p, Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[]);

/** Alloue une offre vide liée au joueur p. À libérer avec engine_trade_free(). */
TradeOffer *engine_trade_init_offer(Player *p);

/** Libère la mémoire d'une offre. */
void engine_trade_free(TradeOffer *off);

/** Ajoute/retire un terrain (niveau 0 requis). Retourne 1 si succès. */
int engine_trade_add_prop(TradeOffer *off, Property *prop);
int engine_trade_remove_prop(TradeOffer *off, Property *prop);

/** Ajoute/retire une gare. Retourne 1 si succès. */
int engine_trade_add_gare(TradeOffer *off, Gares *gare);
int engine_trade_remove_gare(TradeOffer *off, Gares *gare);

/** Ajoute/retire une compagnie. Retourne 1 si succès. */
int engine_trade_add_comp(TradeOffer *off, Compagnies *comp);
int engine_trade_remove_comp(TradeOffer *off, Compagnies *comp);

/** Fixe la compensation financière de l'offre. */
void engine_trade_set_credits(TradeOffer *off, int amount);

/**
 * Vérifie la légalité de l'échange (propriétaire correct, pas de bâtiments, fonds suffisants).
 * @return 1 si valide, 0 sinon.
 */
int engine_trade_validate(TradeOffer *off_actif, TradeOffer *off_cible, Property *tableau_prop[]);

/**
 * Exécute l'échange (transferts de biens et d'argent).
 * Appeler uniquement après engine_trade_validate() == 1 ET acceptation du joueur cible.
 */
void engine_trade_execute(TradeOffer *off_actif, TradeOffer *off_cible, Property *tableau_prop[]);

const char* engine_get_case_joueur_actif(int joueurActif, Player* joueurs[], const Plateau* plateau);

#endif