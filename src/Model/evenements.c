/**
 * @file evenements.c
 * @brief Implémentation du module des événements globaux.
 */

#include "evenements.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ------------------------------------------------------------------------- */
/* DEFINITIONS STATIQUES                                                     */
/* ------------------------------------------------------------------------- */

/**
 * Tableau constant contenant la définition de tous les événements.
 *
 * Les effets ne sont pas appliqués ici directement :
 * le moteur du jeu lira ces multiplicateurs / indicateurs et décidera
 * comment modifier les actions des joueurs.
 */
static const DefEvenement TABLE_EVENEMENTS[EVT_NB] = {
    /* EVT_AUCUN */
    {
        EVT_AUCUN,
        "Aucun evenement",
        0,
        1.0,   /* mult_cout_construction */
        1.0,   /* mult_loyers */
        1.0,   /* mult_gains_cartes */
        0,     /* interdit_construire */
        0,     /* autorise_arret_chez_soi */
        0,     /* utilise_zone_bloquee */
        0,     /* cout_fixe_tous */
        0      /* reduction_AB */
    },

    /* EVT_CRISE_ENERGETIQUE */
    {
        EVT_CRISE_ENERGETIQUE,
        "Crise energetique",
        2,
        1.2,   /* coût construction +20% */
        1.0,
        1.0,
        0,
        0,
        0,
        0,
        0
    },

    /* EVT_INFLATION */
    {
        EVT_INFLATION,
        "Inflation",
        3,
        1.0,
        1.15,  /* loyers +15% */
        0.9,   /* gains des cartes -10% */
        0,
        0,
        0,
        0,
        0
    },

    /* EVT_MOUVEMENT_ETUDIANT */
    {
        EVT_MOUVEMENT_ETUDIANT,
        "Mouvement etudiant",
        2,
        1.0,
        0.8,   /* loyers réduits / plafonnés côté moteur */
        1.0,
        1,     /* interdit de construire */
        0,
        0,
        0,
        0
    },

    /* EVT_CANICULE */
    {
        EVT_CANICULE,
        "Canicule",
        2,
        1.0,
        1.0,
        1.0,
        0,
        0,
        0,
        10,    /* coût fixe pour tous */
        1      /* A/B paient moitié */
    },

    /* EVT_INONDATIONS */
    {
        EVT_INONDATIONS,
        "Inondations",
        2,
        1.0,
        1.0,
        1.0,
        0,
        0,
        1,     /* utilise une zone bloquée */
        0,
        0
    },

    /* EVT_CONFINEMENT */
    {
        EVT_CONFINEMENT,
        "Confinement",
        2,
        1.0,
        2.0,   /* si on tombe chez quelqu'un : loyer doublé */
        1.0,
        0,
        1,     /* possibilité de s'arrêter chez soi */
        0,
        0,
        0
    }
};

/* ------------------------------------------------------------------------- */
/* OUTILS INTERNES                                                           */
/* ------------------------------------------------------------------------- */

/**
 * @brief Initialise le générateur aléatoire C une seule fois.
 */
static void init_hasard_once(void) {
    static int deja_init = 0;
    if (!deja_init) {
        srand((unsigned int)time(NULL));
        deja_init = 1;
    }
}

/**
 * @brief Tire un entier dans [min ; max].
 */
static int entier_aleatoire(int min, int max) {
    if (max < min) return min;
    return min + rand() % (max - min + 1);
}

/**
 * @brief Tire un réel uniforme approximatif dans [0 ; 1].
 */
static double reel_aleatoire_01(void) {
    return (double)rand() / (double)RAND_MAX;
}

/**
 * @brief Vérifie si un événement est actuellement actif.
 */
static int actif(const EtatEvenementGlobal *e) {
    return (e != NULL && e->id != EVT_AUCUN && e->tours_restants > 0);
}

/* ------------------------------------------------------------------------- */
/* INITIALISATION / DEFINITIONS                                              */
/* ------------------------------------------------------------------------- */

void evenements_init(EtatEvenementGlobal *e) {
    if (e == NULL) return;

    e->id = EVT_AUCUN;
    e->tours_restants = 0;
    e->zone_debut = -1;
    e->zone_longueur = 0;
}

const DefEvenement *evenements_def(IdEvenement id) {
    if (id < 0 || id >= EVT_NB) {
        return &TABLE_EVENEMENTS[EVT_AUCUN];
    }
    return &TABLE_EVENEMENTS[id];
}

/* ------------------------------------------------------------------------- */
/* DECLENCHEMENT                                                             */
/* ------------------------------------------------------------------------- */

int evenements_tenter_demarrage(EtatEvenementGlobal *e,
                                double proba_base,
                                double bonus_risque,
                                int taille_plateau)
{
    if (e == NULL) return 0;

    /* On ne déclenche rien si un événement est déjà actif */
    if (actif(e)) return 0;

    init_hasard_once();

    double proba_finale = proba_base + bonus_risque;

    if (proba_finale < 0.0) proba_finale = 0.0;
    if (proba_finale > 1.0) proba_finale = 1.0;

    if (reel_aleatoire_01() >= proba_finale) {
        return 0;
    }

    /*
     * On choisit aléatoirement un événement parmi ceux disponibles
     * (hors EVT_AUCUN).
     */
    IdEvenement choix = (IdEvenement)entier_aleatoire(1, EVT_NB - 1);

    e->id = choix;
    e->tours_restants = evenements_def(choix)->duree_par_defaut;
    e->zone_debut = -1;
    e->zone_longueur = 0;

    /*
     * Particularité des inondations :
     * on bloque une petite zone de 3 cases du plateau.
     */
    if (choix == EVT_INONDATIONS && taille_plateau > 0) {
        int longueur = 3;

        if (taille_plateau < 3) {
            longueur = 1;
        }

        e->zone_longueur = longueur;
        e->zone_debut = entier_aleatoire(0, taille_plateau - longueur);
    }

    /*
     * On peut faire varier légèrement la durée pour certains événements.
     */
    if (choix == EVT_CRISE_ENERGETIQUE) {
        e->tours_restants = entier_aleatoire(2, 3);
    }
    if (choix == EVT_INONDATIONS) {
        e->tours_restants = entier_aleatoire(1, 2);
    }
    if (choix == EVT_CONFINEMENT) {
        e->tours_restants = entier_aleatoire(2, 3);
    }

    return 1;
}

int evenements_tick(EtatEvenementGlobal *e) {
    if (!actif(e)) return 0;

    e->tours_restants--;

    if (e->tours_restants <= 0) {
        evenements_init(e);
        return 1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* ACCESSEURS                                                                */
/* ------------------------------------------------------------------------- */

int evenements_est_actif(const EtatEvenementGlobal *e) {
    return actif(e);
}

double evenements_mult_cout_construction(const EtatEvenementGlobal *e) {
    if (!actif(e)) return 1.0;
    return evenements_def(e->id)->mult_cout_construction;
}

double evenements_mult_loyers(const EtatEvenementGlobal *e) {
    if (!actif(e)) return 1.0;
    return evenements_def(e->id)->mult_loyers;
}

double evenements_mult_gains_cartes(const EtatEvenementGlobal *e) {
    if (!actif(e)) return 1.0;
    return evenements_def(e->id)->mult_gains_cartes;
}

int evenements_interdit_construire(const EtatEvenementGlobal *e) {
    if (!actif(e)) return 0;
    return evenements_def(e->id)->interdit_construire;
}

int evenements_autorise_arret_chez_soi(const EtatEvenementGlobal *e) {
    if (!actif(e)) return 0;
    return evenements_def(e->id)->autorise_arret_chez_soi;
}

int evenements_zone_bloquee(const EtatEvenementGlobal *e) {
    if (!actif(e)) return 0;
    return evenements_def(e->id)->utilise_zone_bloquee;
}

int evenements_case_est_bloquee(const EtatEvenementGlobal *e, int position) {
    if (!actif(e)) return 0;
    if (!evenements_zone_bloquee(e)) return 0;
    if (e->zone_debut < 0 || e->zone_longueur <= 0) return 0;

    return (position >= e->zone_debut &&
            position < e->zone_debut + e->zone_longueur);
}

/* ------------------------------------------------------------------------- */
/* BONUS DE RISQUE                                                           */
/* ------------------------------------------------------------------------- */

double evenements_bonus_risque_depuis_eco(EcoGrade indice_global) {
    switch (indice_global) {
        case GRADE_A: return 0.00;
        case GRADE_B: return 0.01;
        case GRADE_C: return 0.03;
        case GRADE_D: return 0.05;
        case GRADE_E: return 0.08;
        case GRADE_F: return 0.12;
        default:      return 0.00;
    }
}