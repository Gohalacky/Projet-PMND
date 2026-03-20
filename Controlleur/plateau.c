#include <string.h>
#include "plateau.h"
#include <stdio.h>
#include <stdlib.h>

void init_plateau(Plateau *plt, Property *t_prop[], Gares *t_gar[], Compagnies *t_comp[]) {
    // 1. Initialiser tout par défaut en "SPECIAL"
    for(int i=0; i<40; i++) {
        plt->cases[i].type = CASE_SPECIAL;
        plt->cases[i].data = NULL;
        sprintf(plt->cases[i].nom_affichage, "Case %d", i);
    }

    // 2. Case Départ
    plt->cases[0].type = CASE_DEPART;
    strcpy(plt->cases[0].nom_affichage, "DEPART");

    // 3. Placement des TERRAINS (Exemple partiel à adapter selon votre fichier texte)
    // On suit l'ordre du Monopoly classique ou celui de votre fichier
    int p_idx = 0;
    int terrains_pos[] = {1, 3, 6, 8, 9, 11, 13, 14, 16, 18, 19, 21, 23, 24, 26, 27, 29, 31, 32, 34, 37, 39};
    for(int i=0; i < 22; i++) {
        int pos = terrains_pos[i];
        plt->cases[pos].type = CASE_TERRAIN;
        plt->cases[pos].data = t_prop[p_idx];
        strcpy(plt->cases[pos].nom_affichage, t_prop[p_idx]->name);
        p_idx++;
    }

    // 4. Placement des GARES (Positions: 5, 15, 25, 35)
    int gares_pos[] = {5, 15, 25, 35};
    for(int i=0; i < 4; i++) {
        plt->cases[gares_pos[i]].type = CASE_GARE;
        plt->cases[gares_pos[i]].data = t_gar[i];
        strcpy(plt->cases[gares_pos[i]].nom_affichage, t_gar[i]->name);
    }

    // 5. Placement des COMPAGNIES (Positions: 12, 28)
    plt->cases[12].type = CASE_COMPAGNIE;
    plt->cases[12].data = t_comp[0];
    strcpy(plt->cases[12].nom_affichage, t_comp[0]->name);

    plt->cases[28].type = CASE_COMPAGNIE;
    plt->cases[28].data = t_comp[1];
    strcpy(plt->cases[28].nom_affichage, t_comp[1]->name);
}