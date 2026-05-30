/**
 * @file plateau.h
 * @brief Définition du plateau de jeu d'INSA'poly.
 *
 * Ce module décrit la structure du plateau : les 40 cases qui le composent,
 * leur type, et les données associées (propriété, gare, compagnie ou NULL
 * pour les cases spéciales). Il fournit également la fonction d'initialisation
 * qui positionne tous les biens aux bons indices.
 */
#ifndef PLATEAU_H
#define PLATEAU_H

#include "../Model/property.h"

/**
 * @enum TypeCase
 * @brief Identifie la nature d'une case du plateau.
 *
 * Chaque valeur correspond à un comportement distinct lors de la résolution
 * de case dans le moteur de jeu.
 */
typedef enum {
    CASE_DEPART,      /**< Case départ : octroie 200 ECTS au passage */
    CASE_TERRAIN,     /**< Terrain achetable/constructible */
    CASE_GARE,        /**< Gare achetable, loyer progressif selon le nombre possédé */
    CASE_COMPAGNIE,   /**< Compagnie achetable, loyer basé sur le résultat des dés */
    CASE_SPECIAL,     /**< Case neutre (Parc Gratuit, Visite prison…) */
    CASE_CHANCE,      /**< Pioche dans le paquet Chance */
    CASE_COMMUNAUTE,  /**< Pioche dans le paquet Caisse de Communauté */
    CASE_PRISON,      /**< Case prison (visite simple ou détention) */
    CASE_TAXE,        /**< Taxe fixe prélevée par la banque */
    CASE_ALLEZ_PRISON /**< Envoie immédiatement le joueur en prison */
} TypeCase;

/**
 * @struct Case
 * @brief Représente une case individuelle du plateau.
 */
typedef struct {
    char nom_affichage[50]; /**< Nom affiché dans l'interface (console ou SDL2) */
    TypeCase type;          /**< Nature de la case, détermine le comportement à l'arrivée */
    void *data;             /**< Pointeur vers Property, Gares ou Compagnies ; NULL si sans bien */
} Case;

/**
 * @struct Plateau
 * @brief Le plateau de jeu, composé de 40 cases disposées en boucle.
 */
typedef struct {
    Case cases[40]; /**< Tableau des 40 cases dans l'ordre du plateau */
} Plateau;

/**
 * @brief Initialise le plateau en affectant chaque case à son type et à son bien.
 *
 * Place les propriétés, gares et compagnies aux indices officiels du plateau
 * INSA'poly, et configure les cases spéciales (Départ, Prison, Chance,
 * Communauté, Taxe, Allez en Prison, Parc Gratuit).
 *
 * @param plt      Pointeur vers le Plateau à initialiser.
 * @param t_prop   Tableau de pointeurs vers les propriétés chargées depuis le fichier.
 * @param t_gar    Tableau de pointeurs vers les gares chargées depuis le fichier.
 * @param t_comp   Tableau de pointeurs vers les compagnies chargées depuis le fichier.
 */
void init_plateau(Plateau *plt, Property *t_prop[], Gares *t_gar[], Compagnies *t_comp[]);

#endif
