/**
 * @file prison.h
 * @author your name (you@domain.com)
 * @brief  Ce module s'occupe de la gestion des fonction de la case prison et des mécaniques liés , la case prison est la case 10 et la case allé en prison est la case 30.
 * @version 0.1
 * @date 2026-04-09
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #ifndef PRISON_H
 #define PRISON_H
 #define NB_J_MAX 4
 #include "player.h"
 #include "tool.h"
/**
 *@struct Prison 
 *@brief Note le nombre de personne étant en prison et pour chacun d'entre elle depuis combien de temps
 * 
 */
typedef struct{
    Player * liste[NB_J_MAX];   /**Liste de pointeur de joueur présent en prison */
    int comptage[NB_J_MAX];     /**Compte pour chaque joueur du même indice le nombre de tour ou il est en prison */
    int nb_j;                   /**Nombre de joueur dans la partie */
}Prison ;

/**
 * @brief initialise la prison en début de partie
 * 
 * @param nb_j le nombre de joueur en prison
 * @return Prison* la prison vide
 */
Prison * init_prison();

/**
 * @brief envoie un joeur en prison
 * 
 * @param p  le joueur à envoyé en prison
 * @param jail la prison en question
 */
void go_to_jail(Player * p, Prison * jail);

/**
 * @brief libère un joueur de la prison
 * 
 * @param p le joueur en question
 * @param jail la prison en question
 */
void get_out_of_jail(Player * p,Prison * jail);

/**
 * @brief renvoie 1 si le joueur en question est en prison
 * 
 * @param p le joueur en question
 * @param jail la prison en question
 * @return int 
 */
int is_in_jail(Player * p, Prison * jail);

/**
 * @brief libère un joueur en le faisant payer 50 crédit, paye automatiquement après 3tour pour sortir
 * 
 * @param p le joueur en question
 * @param jail la prison en question
 */
void pay_to_get_out(Player * p, Prison * jail);


/**
 * @brief Pour 1 joueur en prison, ajoute 1 au nombre de tour passé en prison
 * @param p le joueur en question
 * @param jail la prison en questoin
 */
void add_tour_jail(Player * p, Prison * jail);

/**
 * @brief renvoie 1 si la personne à passé 3 tour en prison
 * 
 * @param p le joueur en question
 * @param jail la prison
 * @return int 
 */
int istreetour_in_jail(Player * p, Prison * jail);
#endif