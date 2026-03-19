#ifndef EVENEMENTS_H
#define EVENEMENTS_H

#include "alea.h"

typedef enum {
    EVT_AUCUN = 0,
    EVT_CRISE_ENERGETIQUE,
    EVT_INFLATION,
    EVT_MOUVEMENT_ETUDIANT,
    EVT_CANICULE,
    EVT_INONDATIONS,
    EVT_NB
} IdEvenement;

typedef struct {
    IdEvenement id;
    int tours_restants;  /* 0 => inactif */
    int zone_debut;      /* inondations : debut zone bloquee */
    int zone_longueur;   /* inondations : longueur zone bloquee */
} EtatEvenementGlobal;

/* Definition immutable d’un evenement (paramètres appliques tant qu'il est actif). */
typedef struct {
    IdEvenement id;
    const char *nom;
    int duree_par_defaut;

    double mult_cout_construction; /* ex: 1.2 */
    double mult_loyers;            /* ex: 1.15 */
    double mult_gains_cartes;      /* ex: 0.9 */
    int interdit_construire;       /* 1 => constructions/renovations interdites */

    int cout_fixe_tous;            /* ex: canicule : 10 ECTS */
    int reduction_AB;              /* 1 => note A/B paie moitie (à appliquer au niveau du jeu) */

    int utilise_zone_bloquee;      /* 1 => zone_debut/zone_longueur */
} DefEvenement;

const DefEvenement* evenements_def(IdEvenement id);

/* Initialise l'etat (aucun evenement). */
void evenements_init(EtatEvenementGlobal *e);

/* Tente de declencher un evenement (à appeler 1 fois par round complet).
   - proba_base : ex 0.08
   - bonus_risque : ajoute (ex selon indice eco global)
   - taille_plateau : necessaire pour choisir une zone bloquee (inondations)
   Renvoie 1 si un evenement demarre, sinon 0.
   Si un evenement est dejà actif, ne fait rien. */
int evenements_tenter_demarrage(EtatEvenementGlobal *e, Alea *a,
                                double proba_base, double bonus_risque,
                                int taille_plateau);

/* Decremente d'1 tour la duree de l'evenement. Renvoie 1 si l'evenement se termine. */
int evenements_tick(EtatEvenementGlobal *e);

/* Accesseurs (neutres si aucun evenement actif). */
double evenements_mult_cout_construction(const EtatEvenementGlobal *e);
double evenements_mult_loyers(const EtatEvenementGlobal *e);
double evenements_mult_gains_cartes(const EtatEvenementGlobal *e);
int    evenements_interdit_construire(const EtatEvenementGlobal *e);

#endif