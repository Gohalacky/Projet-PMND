/**
 * @file cartes.h
 * @brief Gestion des cartes Chance et Caisse Communauté pour INSA'poly.
 *
 * Ce module modélise les cartes du jeu, leur pioche, leur état
 * (dans la pioche, en main d'un joueur, utilisée) ainsi que
 * les fonctions de tirage et de remise en jeu.
 *
 * Le cas particulier de la carte "Sortie de prison" est géré :
 * lorsqu'elle est tirée, elle reste dans la main du joueur et
 * n'est plus disponible dans la pioche tant qu'elle n'a pas été utilisée
 * puis remise en circulation.
 */

#ifndef CARTES_H
#define CARTES_H

#include "player.h"

/* ------------------------------------------------------------------------- */
/* CONSTANTES                                                                */
/* ------------------------------------------------------------------------- */

#define NB_CARTES_CHANCE 11
#define NB_CARTES_COMMUNAUTE 8

/* ------------------------------------------------------------------------- */
/* ENUMERATIONS                                                              */
/* ------------------------------------------------------------------------- */

/**
 * @enum TypePaquet
 * @brief Indique si une carte appartient au paquet Chance ou Communauté.
 */
typedef enum {
    PAQUET_CHANCE = 0,
    PAQUET_COMMUNAUTE = 1
} TypePaquet;

/**
 * @enum TypeEffetCarte
 * @brief Représente l'effet principal d'une carte.
 */
typedef enum {
    CARTE_GAIN_ECTS = 0,            /**< Le joueur gagne des crédits */
    CARTE_PERTE_ECTS,               /**< Le joueur perd des crédits */
    CARTE_TAXE_ECO,                 /**< Taxe calculée selon l'indice écologique */
    CARTE_DEPLACEMENT_ABSOLU,       /**< Déplacement vers une case précise */
    CARTE_DEPLACEMENT_RELATIF,      /**< Déplacement de +n ou -n cases */
    CARTE_BLOQUE_CONSTRUCTION,      /**< Interdiction temporaire de construire */
    CARTE_SORTIE_PRISON             /**< Carte "Sortie de prison" conservée par le joueur */
} TypeEffetCarte;

/**
 * @enum EtatCarte
 * @brief Indique où se trouve actuellement une carte.
 */
typedef enum {
    ETAT_CARTE_DANS_PAQUET = 0,     /**< Disponible dans la pioche */
    ETAT_CARTE_EN_MAIN,             /**< Conservée par un joueur */
    ETAT_CARTE_UTILISEE             /**< Utilisée et en attente de retour dans le paquet */
} EtatCarte;

/* ------------------------------------------------------------------------- */
/* STRUCTURES                                                                */
/* ------------------------------------------------------------------------- */

/**
 * @struct Carte
 * @brief Représente une carte du jeu.
 *
 * owner_number :
 * - 0  -> aucune possession (carte en pioche ou défaussée)
 * - >0 -> numéro du joueur qui possède actuellement la carte
 */
typedef struct {
    char titre[64];                 /**< Nom court de la carte */
    char description[128];          /**< Texte descriptif affichable */
    TypePaquet paquet;              /**< Chance ou Communauté */
    TypeEffetCarte effet;           /**< Type principal d'effet */
    int valeur;                     /**< Montant ECTS, case cible, déplacement, taxe de base... */
    EtatCarte etat;                 /**< État courant de la carte */
    int owner_number;               /**< Numéro du joueur propriétaire si carte en main */
} Carte;

/**
 * @struct Pioche
 * @brief Représente une pioche de cartes.
 *
 * Le tableau 'cartes' contient toutes les cartes du paquet.
 * Le tableau 'ordre' contient les indices mélangés pour piocher.
 * 'sommet' indique la prochaine carte à examiner.
 */
typedef struct {
    Carte cartes[NB_CARTES_CHANCE]; /**< Utilisé uniquement pour les pioche Chance */
    int ordre[NB_CARTES_CHANCE];
    int nb_cartes;
    int sommet;
} PiocheChance;

/**
 * @struct PiocheCommunaute
 * @brief Version dédiée à la pioche Communauté.
 */
typedef struct {
    Carte cartes[NB_CARTES_COMMUNAUTE];
    int ordre[NB_CARTES_COMMUNAUTE];
    int nb_cartes;
    int sommet;
} PiocheCommunaute;

/* ------------------------------------------------------------------------- */
/* INITIALISATION                                                            */
/* ------------------------------------------------------------------------- */

/**
 * @brief Initialise toutes les cartes du paquet Chance et mélange la pioche.
 * @param pioche Pointeur vers la pioche à initialiser.
 */
void init_pioche_chance(PiocheChance *pioche);

/**
 * @brief Initialise toutes les cartes du paquet Communauté et mélange la pioche.
 * @param pioche Pointeur vers la pioche à initialiser.
 */
void init_pioche_communaute(PiocheCommunaute *pioche);

/* ------------------------------------------------------------------------- */
/* MELANGE ET PIOCHE                                                         */
/* ------------------------------------------------------------------------- */

/**
 * @brief Mélange la pioche Chance.
 * @param pioche Pointeur vers la pioche.
 */
void melanger_pioche_chance(PiocheChance *pioche);

/**
 * @brief Mélange la pioche Communauté.
 * @param pioche Pointeur vers la pioche.
 */
void melanger_pioche_communaute(PiocheCommunaute *pioche);

/**
 * @brief Pioche une carte Chance disponible.
 *
 * Si une carte "Sortie de prison" est déjà en main chez un joueur,
 * elle n'est pas repiochable tant qu'elle n'a pas été remise dans le paquet.
 *
 * @param pioche Pointeur vers la pioche Chance.
 * @param numero_joueur Numéro du joueur qui pioche.
 * @return Pointeur vers la carte piochée, ou NULL si aucune carte disponible.
 */
Carte *piocher_carte_chance(PiocheChance *pioche, int numero_joueur);

/**
 * @brief Pioche une carte Communauté disponible.
 * @param pioche Pointeur vers la pioche Communauté.
 * @param numero_joueur Numéro du joueur qui pioche.
 * @return Pointeur vers la carte piochée, ou NULL si aucune carte disponible.
 */
Carte *piocher_carte_communaute(PiocheCommunaute *pioche, int numero_joueur);

/* ------------------------------------------------------------------------- */
/* GESTION DES CARTES POSSEDEES                                              */
/* ------------------------------------------------------------------------- */

/**
 * @brief Indique si une carte est une carte "Sortie de prison".
 * @param carte Carte à tester.
 * @return 1 si oui, 0 sinon.
 */
int est_carte_sortie_prison(const Carte *carte);

/**
 * @brief Attribue une carte à un joueur.
 *
 * Utilisé en particulier pour la carte "Sortie de prison".
 *
 * @param carte Carte concernée.
 * @param numero_joueur Numéro du joueur.
 */
void donner_carte_a_joueur(Carte *carte, int numero_joueur);

/**
 * @brief Marque une carte comme utilisée.
 * @param carte Carte concernée.
 */
void utiliser_carte(Carte *carte);

/**
 * @brief Remet une carte dans le paquet.
 *
 * Typiquement utilisée après avoir consommé une carte "Sortie de prison".
 *
 * @param carte Carte concernée.
 */
void remettre_carte_dans_paquet(Carte *carte);

/**
 * @brief Vérifie si un joueur possède une carte "Sortie de prison" dans une pioche donnée.
 * @param cartes Tableau de cartes.
 * @param nb_cartes Nombre total de cartes.
 * @param numero_joueur Numéro du joueur.
 * @return 1 si le joueur possède au moins une carte Sortie de prison, 0 sinon.
 */
int joueur_possede_sortie_prison(const Carte *cartes, int nb_cartes, int numero_joueur);

/* ------------------------------------------------------------------------- */
/* OUTILS                                                                    */
/* ------------------------------------------------------------------------- */

/**
 * @brief Calcule une taxe écologique à partir d'une base et de l'indice du joueur.
 *
 * Convention actuelle de votre projet :
 * A=1, B=2, ..., F=6.
 * On applique un coefficient croissant avec la mauvaise note.
 *
 * @param base Montant de base de la taxe.
 * @param indice Note écologique du joueur.
 * @return Montant final de la taxe.
 */
int calcul_taxe_ecologique(int base, EcoGrade indice);

#endif