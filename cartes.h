#ifndef CARTES_H
#define CARTES_H

#include "alea.h"
#include "eco.h"

typedef enum {
    PAQUET_CHANCE = 0,
    PAQUET_COMMUNAUTE,
    PAQUET_NB
} TypePaquet;

typedef enum {
    EFFET_GAIN_ECTS = 0,
    EFFET_PERTE_ECTS,
    EFFET_TAXE_ECO,          /* base * coeff(note) */
    EFFET_DEPLACEMENT_ABS,   /* aller à une case */
    EFFET_DEPLACEMENT_REL,   /* avancer/reculer */
    EFFET_INTERDIT_CONSTRUIRE_1_TOUR /* penalite temporaire */
} TypeEffetCarte;

typedef struct {
    TypePaquet paquet;
    const char *titre;
    const char *description;

    TypeEffetCarte effet;
    int valeur; /* ECTS, base taxe, position, deplacements... */
} Carte;

typedef struct {
    const Carte *cartes;
    int nb_cartes;

    int *ordre; /* indices melanges */
    int sommet; /* prochaine carte */
} Paquet;

/* Initialise les deux paquets (melange inclus).
   Les tableaux ordre_* servent d’espace de travail (evite malloc). */
void cartes_init_paquets(Paquet paquets[PAQUET_NB],
                         int *ordre_chance, int cap_chance,
                         int *ordre_commu, int cap_commu,
                         Alea *a);

/* Pioche une carte ; remelange quand vide. */
const Carte* cartes_piocher(Paquet *p, Alea *a);

/* Calcule la taxe ecologique pour une carte EFFET_TAXE_ECO. */
int cartes_taxe_eco(const Carte *c, NoteEco note);

#endif