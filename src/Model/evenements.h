/**
 * @file evenements.h
 * @brief Gestion des événements globaux temporaires dans INSA'poly.
 *
 * Ce module modélise les événements qui affectent tous les joueurs
 * pendant plusieurs tours / rounds.
 *
 * Exemples :
 * - Crise énergétique
 * - Inflation
 * - Mouvement étudiant
 * - Canicule
 * - Inondations
 * - Confinement
 *
 * Le module ne modifie pas directement les joueurs ou les propriétés :
 * il fournit un état global et des multiplicateurs / effets que le moteur
 * du jeu pourra appliquer au bon moment.
 */

#ifndef EVENEMENTS_H
#define EVENEMENTS_H

#include "player.h"

/* ------------------------------------------------------------------------- */
/* ENUMERATIONS                                                              */
/* ------------------------------------------------------------------------- */

/**
 * @enum IdEvenement
 * @brief Identifiant d’un événement global.
 */
typedef enum {
    EVT_AUCUN = 0,
    EVT_CRISE_ENERGETIQUE,
    EVT_INFLATION,
    EVT_MOUVEMENT_ETUDIANT,
    EVT_CANICULE,
    EVT_INONDATIONS,
    EVT_CONFINEMENT,
    EVT_NB
} IdEvenement;

/* ------------------------------------------------------------------------- */
/* STRUCTURES                                                                */
/* ------------------------------------------------------------------------- */

/**
 * @struct DefEvenement
 * @brief Définition "statique" d’un type d’événement.
 *
 * Cette structure décrit les effets généraux appliqués pendant
 * toute la durée de l’événement.
 */
typedef struct {
    IdEvenement id;                     /**< Identifiant unique */
    char nom[64];                       /**< Nom de l'événement */
    int duree_par_defaut;               /**< Durée nominale en rounds */

    double mult_cout_construction;      /**< Multiplicateur sur les coûts de construction */
    double mult_loyers;                 /**< Multiplicateur sur les loyers */
    double mult_gains_cartes;           /**< Multiplicateur sur les gains des cartes */

    int interdit_construire;            /**< 1 si les constructions/rénovations sont interdites */
    int autorise_arret_chez_soi;        /**< 1 si le joueur peut choisir de s'arrêter chez lui */
    int utilise_zone_bloquee;           /**< 1 si une zone du plateau peut être bloquée */

    int cout_fixe_tous;                 /**< Coût fixe payé par tous les joueurs */
    int reduction_AB;                   /**< 1 si les joueurs A/B paient moitié pour cout_fixe_tous */
} DefEvenement;

/**
 * @struct EtatEvenementGlobal
 * @brief Représente l’événement actuellement actif dans la partie.
 *
 * - id = EVT_AUCUN signifie qu’aucun événement n’est en cours.
 * - tours_restants indique combien de rounds il reste avant la fin.
 * - zone_debut / zone_longueur servent pour l’événement Inondations.
 */
typedef struct {
    IdEvenement id;         /**< Identifiant de l'événement actif */
    int tours_restants;     /**< Nombre de rounds restants */
    int zone_debut;         /**< Début d'une zone bloquée (si concerné) */
    int zone_longueur;      /**< Longueur de la zone bloquée */
} EtatEvenementGlobal;

/* ------------------------------------------------------------------------- */
/* INITIALISATION / DEFINITIONS                                              */
/* ------------------------------------------------------------------------- */

/**
 * @brief Initialise l’état global des événements.
 * @param e Pointeur vers l'état à initialiser.
 */
void evenements_init(EtatEvenementGlobal *e);

/**
 * @brief Renvoie la définition associée à un événement.
 * @param id Identifiant de l'événement.
 * @return Pointeur vers la définition correspondante.
 */
const DefEvenement *evenements_def(IdEvenement id);

/* ------------------------------------------------------------------------- */
/* GESTION DU DECLENCHEMENT                                                  */
/* ------------------------------------------------------------------------- */

/**
 * @brief Tente de démarrer un nouvel événement à la fin d’un round complet.
 *
 * Cette fonction ne démarre un événement que si aucun événement n’est déjà actif.
 * La probabilité finale de déclenchement vaut :
 *
 *     proba_finale = proba_base + bonus_risque
 *
 * puis est bornée dans [0 ; 1].
 *
 * @param e Etat global des événements.
 * @param proba_base Probabilité de base (ex: 0.08 pour 8%).
 * @param bonus_risque Bonus lié à l’état du jeu.
 * @param taille_plateau Taille totale du plateau (utile pour Inondations).
 * @return 1 si un événement a été déclenché, 0 sinon.
 */
int evenements_tenter_demarrage(EtatEvenementGlobal *e,
                                double proba_base,
                                double bonus_risque,
                                int taille_plateau);

/**
 * @brief Fait avancer l'événement actif d'un round.
 *
 * Si l'événement arrive à son terme, il est réinitialisé.
 *
 * @param e Etat global de l’événement.
 * @return 1 si l’événement vient de se terminer, 0 sinon.
 */
int evenements_tick(EtatEvenementGlobal *e);

/* ------------------------------------------------------------------------- */
/* ACCESSEURS PRATIQUES                                                      */
/* ------------------------------------------------------------------------- */

/**
 * @brief Renvoie 1 si un événement est actif, 0 sinon.
 */
int evenements_est_actif(const EtatEvenementGlobal *e);

/**
 * @brief Renvoie le multiplicateur de coût de construction.
 */
double evenements_mult_cout_construction(const EtatEvenementGlobal *e);

/**
 * @brief Renvoie le multiplicateur de loyer.
 */
double evenements_mult_loyers(const EtatEvenementGlobal *e);

/**
 * @brief Renvoie le multiplicateur sur les gains de cartes.
 */
double evenements_mult_gains_cartes(const EtatEvenementGlobal *e);

/**
 * @brief Renvoie 1 si les constructions sont interdites.
 */
int evenements_interdit_construire(const EtatEvenementGlobal *e);

/**
 * @brief Renvoie 1 si le joueur peut choisir de s’arrêter sur sa propre propriété.
 */
int evenements_autorise_arret_chez_soi(const EtatEvenementGlobal *e);

/**
 * @brief Renvoie 1 si une zone est bloquée (cas des inondations).
 */
int evenements_zone_bloquee(const EtatEvenementGlobal *e);

/**
 * @brief Indique si une case appartient à la zone bloquée.
 *
 * @param e Etat global.
 * @param position Position à tester.
 * @return 1 si la case est bloquée, 0 sinon.
 */
int evenements_case_est_bloquee(const EtatEvenementGlobal *e, int position);

/* ------------------------------------------------------------------------- */
/* OUTILS DE RISQUE                                                          */
/* ------------------------------------------------------------------------- */

/**
 * @brief Calcule un bonus de risque en fonction d’un indice écologique global moyen.
 *
 * Convention actuelle :
 * - GRADE_A = 1 (meilleur)
 * - GRADE_F = 6 (pire)
 *
 * Plus la note écologique moyenne est mauvaise, plus le bonus de risque augmente.
 *
 * @param indice_global Indice écologique global moyen.
 * @return Bonus de probabilité conseillé.
 */
double evenements_bonus_risque_depuis_eco(EcoGrade indice_global);

#endif