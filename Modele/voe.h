/**
 * @file voe.h
 * @brief s'occupe du système de vente au enchere pour les propriété, les gares et les compagnies si un joueur tombe sur la case à vendre sans l'acheter, le joueur qui à refusé ne participe pas
 * 
 */

#ifndef VOE_H
#define VOE_H
#include "player.h"
#include "tool.h"
#include "property.h"

#define MAX_PLAYERS 4 // 4 joueus max pour les ventes au enchères

/**
 *@struct AuctionState
 *@brief structure d'une vente au enchere pour une propriété d'une gare ou d'une compagnie, seul une des trois possible à la fois , les autres seront des pointeurs null
 * 
 */
typedef struct {
    Property *prop_item;                /**< La propriété mise aux enchères*/ 
    Gares *gar_item;                    /**< La gare mise aux enchères*/ 
    Compagnies *comp_item;              /**< La compagnie mise aux enchères*/ 
    int current_bid;                    /**< Le montant de la meilleure offre actuelle */ 
    Player *highest_bidder;             /**< Le joueur qui mène l'enchère */ 
    Player * in_auction[MAX_PLAYERS];   /**< tableau des joueurs encore participant à l'enchere */ 
    int players_left;                   /**< Combien de joueurs n'ont pas encore passé leur tour */ 
} AuctionState;


/**
 * @brief initialise la mise en enchere d'une propriété, c'est le même schéma pour les compagnies et les gares
 * @return la mise en enchere en question avec la structure rempli et les 2 autres type de case à NULL
 * @param prop la proprieté mise en enchere
 * @param p le joueur qui n'a pas acheté en tombant sur la case
 */
AuctionState* init_auction_prop(Property *prop, Player *all_players[], int nb_joueurs_total);

/**
 * @brief initialise la mise en enchere d'une gare
 * @return la mise en enchere en question avec la structure rempli et les 2 autres type de case à NULL
 * @param g la gare mise en enchere
 * @param p le joueur qui n'a pas acheté en tombant sur la case
 */
AuctionState* init_auction_gar(Gares *g, Player *all_players[], int nb_joueurs_total);

/**
 * @brief initialise la mise en enchere d'une compagnie
 * @return la mise en enchere en question avec la structure rempli et les 2 autres type de case à NULL
 * @param c la compagnie mise en enchere
 * @param p le joueur qui n'a pas acheté en tombant sur la case
 */
AuctionState* init_auction_comp(Compagnies *c, Player *all_players[], int nb_joueurs_total);

/**
 * @brief Vérifie si l'enchère d'un joueur est en règle
 * @return 1 si oui 0 sinon
 */
int process_bid(AuctionState *auction, Player *p, int amount);

/**
 * @brief fait qu'un joueur se couche, il ne peux plus participer à l'enchere
 * 
 * @param auction 
 * @param p 
 */
void fold_player(AuctionState *auction, Player *p);

/**
 * @brief finalise l'enchere, une fois qu'il ne reste qu'une personne elle gagne l'enchere, paye et gagne le lot
 * 
 * @param auction 
 */
void finalize_auction(AuctionState *auction);
#endif