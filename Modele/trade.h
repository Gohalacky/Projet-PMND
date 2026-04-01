/**
 * @file trade.h
 * @brief Modèle de gestion du systèmes d'échanges entre les joueurs.
 * * Ce fichier gère les fonctions ces échanges.
 */

#ifndef TRADE_H
#define TRADE_H
#include "player.h"
#include "tool.h"
#include "property.h"
#include "cartes.h"

/**
 * @struct TradeOffer
 * @brief Représente le contenu d'une offre faite par un joueur lors d'un échange une tradeOffer contre une autre tradeOffer.
 * Une tradeOffer et ce qu'un joueur offre en échange d'une autre tradeoffer
 */
typedef struct {

    Player * proposant;                    /**<Dit qui propose cette offre */
    int ects;                              /**< Nombre de crédits proposé */
    
    Carte * car[4];                        /**< Les cartes proposé, forcément des cartes sorties de prison */ 
    int nb_cartes;                         /**< le nombre de carte proposé */
    
    Property *props[NB_PROPRIETE];         /**< les propriété proposé pour l'échange */
    int nb_props;                          /**< Compteur de propriétés*/
    
    Gares *gares[NB_GARE];                 /**< Les gares proposées */
    int nb_gares;                          /**< Compteur de gares */
    
    Compagnies *comps[NB_COMPAGNIE];       /**< Les compagnies proposées */
    int nb_comps;                          /**< Compteur de compagnies */
} TradeOffer;


/**
 * @brief Initialise une offre vide et alloue l'espace mémoire qui est free dans  executre_trade.
 */
TradeOffer * init_empty_offer(Player *p);

/**
 * @brief Vérifie si un échange est légal selon les règles.
 * @return 1 si l'échange est valide, 0 si une règle est violée (ex: triche, constructions présentes).
 */
int is_trade_valid(TradeOffer *off_p1, TradeOffer *off_p2);

/**
 * @brief Transfère les titres de propriété et l'argent entre les deux joueurs.
 * @pre La fonction is_trade_valid DOIT avoir retourné 1 avant d'appeler celle-ci, de plus elle free l'espace mémoire alloué au trade offer.
 */
void execute_trade(TradeOffer *off_p1, TradeOffer *off_p2);

#endif