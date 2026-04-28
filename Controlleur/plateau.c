#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "plateau.h"

/**
 * Initialise la structure du plateau de 40 cases.
 * Associe les données chargées (terrains, gares, compagnies) aux bonnes positions.
 */
void init_plateau(Plateau *plt, Property *t_prop[], Gares *t_gar[], Compagnies *t_comp[]) {
    
    // Initialisation par défaut de toutes les cases
    for (int i = 0; i < 40; i++) {
        plt->cases[i].type = CASE_SPECIAL;
        plt->cases[i].data = NULL;
        sprintf(plt->cases[i].nom_affichage, "Case Spéciale %d", i);
    }

    // Configuration case fixes
    plt->cases[0].type = CASE_DEPART;
    strcpy(plt->cases[0].nom_affichage, "DÉPART ( +200 ECTS )");

    plt->cases[4].type = CASE_TAXE; // Impôt sur le revenu
    strcpy(plt->cases[4].nom_affichage, "IMPÔT SUR LE REVENU ( -200 ECTS )");

    plt->cases[10].type = CASE_PRISON;
    strcpy(plt->cases[10].nom_affichage, "PRISON ( Visite Simple )");

    plt->cases[20].type = CASE_SPECIAL; // Parc Gratuit
    strcpy(plt->cases[20].nom_affichage, "PARC GRATUIT");

    plt->cases[30].type = CASE_ALLEZ_PRISON;
    strcpy(plt->cases[30].nom_affichage, "ALLEZ EN PRISON !");

    plt->cases[38].type = CASE_TAXE; // Taxe de luxe
    strcpy(plt->cases[38].nom_affichage, "TAXE DE LUXE ( -100 ECTS )");

    // Cases Chance (positions officielles Monopoly : 7, 22, 36)
    int chance_pos[] = {7, 22, 36};
    for (int i = 0; i < 3; i++) {
        plt->cases[chance_pos[i]].type = CASE_CHANCE;
        strcpy(plt->cases[chance_pos[i]].nom_affichage, "? CHANCE ?");
    }

    // Cases Caisse de Communauté (positions officielles : 2, 17, 33)
    int communaute_pos[] = {2, 17, 33};
    for (int i = 0; i < 3; i++) {
        plt->cases[communaute_pos[i]].type = CASE_COMMUNAUTE;
        strcpy(plt->cases[communaute_pos[i]].nom_affichage, "CAISSE COMMUNAUTÉ");
    }


    // Placement des terrains (22 propriétés)
    // Liste des indices officiels du plateau pour les terrains
    int terrains_pos[] = {1, 3, 6, 8, 9, 11, 13, 14, 16, 18, 19, 21, 23, 24, 26, 27, 29, 31, 32, 34, 37, 39};
    
    for (int i = 0; i < 22; i++) {
        int pos = terrains_pos[i];
        if (t_prop[i] != NULL) {
            plt->cases[pos].type = CASE_TERRAIN;
            plt->cases[pos].data = t_prop[i];
            // On copie le nom pour l'affichage rapide
            strncpy(plt->cases[pos].nom_affichage, t_prop[i]->name, 49);
        }
    }

    // Placement des gares (Positions: 5, 15, 25, 35)
    int gares_pos[] = {5, 15, 25, 35};
    for (int i = 0; i < 4; i++) {
        int pos = gares_pos[i];
        if (t_gar[i] != NULL) {
            plt->cases[pos].type = CASE_GARE;
            plt->cases[pos].data = t_gar[i];
            strncpy(plt->cases[pos].nom_affichage, t_gar[i]->name, 49);
        }
    }
    
    // Placement des compagnies (Electricité / Eau)
    if (t_comp[0] != NULL) {
        plt->cases[12].type = CASE_COMPAGNIE;
        plt->cases[12].data = t_comp[0];
        strncpy(plt->cases[12].nom_affichage, t_comp[0]->name, 49);
    }
    if (t_comp[1] != NULL) {
        plt->cases[28].type = CASE_COMPAGNIE;
        plt->cases[28].data = t_comp[1];
        strncpy(plt->cases[28].nom_affichage, t_comp[1]->name, 49);
    }
}