#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "evenements.h"

static const DefEvenement DEFS[] = {
    { EVT_AUCUN, "Aucun", 0, 1.0, 1.0, 1.0, 0, 0, 0, 0 },

    /* Crise energetique (2–3 tours) : construction +20% */
    { EVT_CRISE_ENERGETIQUE, "Crise energetique", 2, 1.2, 1.0, 1.0, 0, 0, 0, 0 },

    /* Inflation (3 tours) : loyers +15%, gains cartes -10% */
    { EVT_INFLATION, "Inflation", 3, 1.0, 1.15, 0.9, 0, 0, 0, 0 },

    /* Mouvement etudiant (2 tours) : interdiction de construire, loyers -20% */
    { EVT_MOUVEMENT_ETUDIANT, "Mouvement etudiant", 2, 1.0, 0.8, 1.0, 1, 0, 0, 0 },

    /* Canicule (2 tours) : coût fixe pour tous, A/B paie moitie (à appliquer au niveau du jeu) */
    { EVT_CANICULE, "Canicule", 2, 1.0, 1.0, 1.0, 0, 10, 1, 0 },

    /* Inondations (1–2 tours) : zone bloquee */
    { EVT_INONDATIONS, "Inondations", 2, 1.0, 1.0, 1.0, 0, 0, 0, 1 },
};

const DefEvenement* evenements_def(IdEvenement id) {
    if (id < 0 || id >= EVT_NB) return &DEFS[0];
    return &DEFS[(int)id];
}

void evenements_init(EtatEvenementGlobal *e) {
    if (!e) return;
    e->id = EVT_AUCUN;
    e->tours_restants = 0;
    e->zone_debut = -1;
    e->zone_longueur = 0;
}

static int actif(const EtatEvenementGlobal *e) {
    return e && e->id != EVT_AUCUN && e->tours_restants > 0;
}

int evenements_tenter_demarrage(EtatEvenementGlobal *e, Alea *a, double proba_base, double bonus_risque, int taille_plateau) {
    if (!e || !a) return 0;
    if (actif(e)) return 0;

    double p = proba_base + bonus_risque;
    if (p < 0.0) p = 0.0;
    if (p > 1.0) p = 1.0;

    if (!alea_chance(a, p)) return 0;

    /* Tirage uniforme parmi les evenements (hors EVT_AUCUN) */
    IdEvenement choix = (IdEvenement)(1 + (int)alea_borne(a, (uint32_t)(EVT_NB - 1)));
    const DefEvenement *d = evenements_def(choix);

    int duree = d->duree_par_defaut;
    if (choix == EVT_CRISE_ENERGETIQUE) duree = 2 + (int)alea_borne(a, 2); /* 2..3 */
    if (choix == EVT_INONDATIONS)       duree = 1 + (int)alea_borne(a, 2); /* 1..2 */

    e->id = choix;
    e->tours_restants = duree;
    e->zone_debut = -1;
    e->zone_longueur = 0;

    if (choix == EVT_INONDATIONS && taille_plateau > 0) {
        int len = 3;
        if (taille_plateau < 6) len = (taille_plateau >= 3) ? 2 : 1;
        int max_debut = taille_plateau - len;
        if (max_debut < 0) max_debut = 0;
        int debut = (int)alea_borne(a, (uint32_t)(max_debut + 1));
        e->zone_debut = debut;
        e->zone_longueur = len;
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