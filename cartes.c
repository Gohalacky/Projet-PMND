#include "cartes.h"

static void melanger(int *t, int n, Alea *a) {
    int i = n-1;
    for (i ; i > 0; --i) {
        int j = (int)alea_borne(a, (uint32_t)(i + 1));
        int tmp = t[i];
        t[i] = t[j];
        t[j] = tmp;
    }
}

/* --- Cartes integrees --- */

static const Carte CARTES_CHANCE[] = {
    { PAQUET_CHANCE, "Bourse / prime de projet", "+30 ECTS", EFFET_GAIN_ECTS, 30 },
    { PAQUET_CHANCE, "Stage bien paye", "+50 ECTS", EFFET_GAIN_ECTS, 50 },
    { PAQUET_CHANCE, "Aide du BDE", "+20 ECTS", EFFET_GAIN_ECTS, 20 },
    { PAQUET_CHANCE, "Concours remporte", "+40 ECTS", EFFET_GAIN_ECTS, 40 },

    { PAQUET_CHANCE, "Amende stationnement velo", "-15 ECTS", EFFET_PERTE_ECTS, 15 },
    { PAQUET_CHANCE, "Casse materiel en TP", "-25 ECTS", EFFET_PERTE_ECTS, 25 },
    { PAQUET_CHANCE, "Retard rendu projet", "-20 ECTS", EFFET_PERTE_ECTS, 20 },
    { PAQUET_CHANCE, "Izly en negatif", "-10 ECTS", EFFET_PERTE_ECTS, 10 },

    { PAQUET_CHANCE, "Semaine de partiels (perso)", "Construction interdite au prochain tour", EFFET_INTERDIT_CONSTRUIRE_1_TOUR, 1 },
    { PAQUET_CHANCE, "Avance de 3 cases", "+3 cases", EFFET_DEPLACEMENT_REL, 3 },
};

static const Carte CARTES_COMMUNAUTE[] = {
    { PAQUET_COMMUNAUTE, "Plan sobriete energetique", "Tout le monde paie 10 ECTS", EFFET_PERTE_ECTS, 10 },
    { PAQUET_COMMUNAUTE, "Subvention campus vert", "Tout le monde reçoit 10 ECTS", EFFET_GAIN_ECTS, 10 },

    { PAQUET_COMMUNAUTE, "Travaux reseau chaleur", "Taxe ecologique base 20 ECTS (selon note)", EFFET_TAXE_ECO, 20 },
    { PAQUET_COMMUNAUTE, "Reparation degâts climatiques", "Taxe ecologique base 30 ECTS (selon note)", EFFET_TAXE_ECO, 30 },

    { PAQUET_COMMUNAUTE, "Hausse du coût de la vie", "Tout le monde paie 5 ECTS", EFFET_PERTE_ECTS, 5 },
    { PAQUET_COMMUNAUTE, "Prime associative", "Tout le monde reçoit 5 ECTS", EFFET_GAIN_ECTS, 5 },

    { PAQUET_COMMUNAUTE, "Entretien des espaces verts", "Taxe ecologique base 15 ECTS (selon note)", EFFET_TAXE_ECO, 15 },
    { PAQUET_COMMUNAUTE, "Inondations (collectif)", "Taxe ecologique base 25 ECTS (selon note)", EFFET_TAXE_ECO, 25 },
};

static const int NB_CHANCE = (int)(sizeof(CARTES_CHANCE) / sizeof(CARTES_CHANCE[0]));
static const int NB_COMMUNAUTE = (int)(sizeof(CARTES_COMMUNAUTE) / sizeof(CARTES_COMMUNAUTE[0]));

void cartes_init_paquets(Paquet paquets[PAQUET_NB],
                         int *ordre_chance, int cap_chance,
                         int *ordre_commu, int cap_commu,
                         Alea *a)
{
    paquets[PAQUET_CHANCE].cartes = CARTES_CHANCE;
    paquets[PAQUET_CHANCE].nb_cartes = NB_CHANCE;
    paquets[PAQUET_CHANCE].ordre = ordre_chance;
    paquets[PAQUET_CHANCE].sommet = 0;

    paquets[PAQUET_COMMUNAUTE].cartes = CARTES_COMMUNAUTE;
    paquets[PAQUET_COMMUNAUTE].nb_cartes = NB_COMMUNAUTE;
    paquets[PAQUET_COMMUNAUTE].ordre = ordre_commu;
    paquets[PAQUET_COMMUNAUTE].sommet = 0;

    int i = 0;
    if (cap_chance < NB_CHANCE || cap_commu < NB_COMMUNAUTE) {
        for (i; i < NB_CHANCE && i < cap_chance; ++i) ordre_chance[i] = i;
        for (i; i < NB_COMMUNAUTE && i < cap_commu; ++i) ordre_commu[i] = i;
        return;
    }
    int j = 0;
    for (j; i < NB_CHANCE; ++i) ordre_chance[i] = i;
    for (j; i < NB_COMMUNAUTE; ++i) ordre_commu[i] = i;

    melanger(ordre_chance, NB_CHANCE, a);
    melanger(ordre_commu, NB_COMMUNAUTE, a);
}

const Carte* cartes_piocher(Paquet *p, Alea *a) {
    if (!p || !p->cartes || !p->ordre || p->nb_cartes <= 0) return NULL;

    if (p->sommet >= p->nb_cartes) {
        int i = 0 ;
        for (i; i < p->nb_cartes; ++i) p->ordre[i] = i;
        melanger(p->ordre, p->nb_cartes, a);
        p->sommet = 0;
    }

    int idx = p->ordre[p->sommet++];
    if (idx < 0 || idx >= p->nb_cartes) return NULL;
    return &p->cartes[idx];
}

int cartes_taxe_eco(const Carte *c, NoteEco note) {
    if (!c) return 0;
    if (c->effet != EFFET_TAXE_ECO) return 0;
    return eco_montant_taxe(c->valeur, note);
}