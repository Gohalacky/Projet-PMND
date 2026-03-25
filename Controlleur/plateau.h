#ifndef PLATEAU_H
#define PLATEAU_H

#include "../Modele/property.h"

// Types de cases possibles sur le plateau
typedef enum {
    CASE_DEPART,
    CASE_TERRAIN,
    CASE_GARE,
    CASE_COMPAGNIE,
    CASE_SPECIAL // Taxes, Prison, Chance,
} TypeCase;

// Structure d'une case du plateau
typedef struct {
    char nom_affichage[50];
    TypeCase type;
    void *data; // Pointeur vers Property, Gares ou Compagnies (NULL si SPECIAL/DEPART)
} Case;

// Le plateau est un tableau de 40 cases
typedef struct {
    Case cases[40];
} Plateau;


//Initialise le plateau en plaçant les propriétés, gares et compagnies aux bons indices.

void init_plateau(Plateau *plt, Property *t_prop[], Gares *t_gar[], Compagnies *t_comp[]);

#endif