/**
 * @file cartes.c
 * @brief Implémentation du module de gestion des cartes.
 */

#include "cartes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ------------------------------------------------------------------------- */
/* FONCTIONS INTERNES                                                        */
/* ------------------------------------------------------------------------- */

/**
 * @brief Échange deux entiers dans un tableau.
 */
static void echanger_int(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

/**
 * @brief Initialise le générateur pseudo-aléatoire une seule fois.
 */
static void init_hasard_once(void) {
    static int deja_init = 0;
    if (!deja_init) {
        srand((unsigned int)time(NULL));
        deja_init = 1;
    }
}

/**
 * @brief Renvoie l'indice d'une carte disponible dans une pioche Chance.
 *
 * On parcourt les cartes selon l'ordre mélangé à partir du sommet.
 * Si une carte n'est pas disponible (par ex. Sortie de prison déjà en main),
 * on la saute.
 *
 * @return Indice réel dans le tableau cartes, ou -1 si aucune carte disponible.
 */
static int trouver_carte_disponible_chance(PiocheChance *pioche) {
    if (pioche == NULL) return -1;

    int tentative ;
    for (tentative = 0; tentative < pioche->nb_cartes; tentative++) {
        int pos = (pioche->sommet + tentative) % pioche->nb_cartes;
        int idx = pioche->ordre[pos];

        if (pioche->cartes[idx].etat == ETAT_CARTE_DANS_PAQUET) {
            pioche->sommet = (pos + 1) % pioche->nb_cartes;
            return idx;
        }
    }

    return -1;
}

/**
 * @brief Version équivalente pour la pioche Communauté.
 */
static int trouver_carte_disponible_communaute(PiocheCommunaute *pioche) {
    if (pioche == NULL) return -1;

    int tentative ;
    for (tentative = 0; tentative < pioche->nb_cartes; tentative++) {
        int pos = (pioche->sommet + tentative) % pioche->nb_cartes;
        int idx = pioche->ordre[pos];

        if (pioche->cartes[idx].etat == ETAT_CARTE_DANS_PAQUET) {
            pioche->sommet = (pos + 1) % pioche->nb_cartes;
            return idx;
        }
    }

    return -1;
}

/* ------------------------------------------------------------------------- */
/* INITIALISATION DES PIOCHES                                                */
/* ------------------------------------------------------------------------- */

void init_pioche_chance(PiocheChance *pioche) {
    if (pioche == NULL) return;

    pioche->nb_cartes = NB_CARTES_CHANCE;
    pioche->sommet = 0;

    strcpy(pioche->cartes[0].titre, "Bourse de projet");
    strcpy(pioche->cartes[0].description, "Vous recevez 30 ECTS.");
    pioche->cartes[0].paquet = PAQUET_CHANCE;
    pioche->cartes[0].effet = CARTE_GAIN_ECTS;
    pioche->cartes[0].valeur = 30;

    strcpy(pioche->cartes[1].titre, "Stage bien paye");
    strcpy(pioche->cartes[1].description, "Vous recevez 50 ECTS.");
    pioche->cartes[1].paquet = PAQUET_CHANCE;
    pioche->cartes[1].effet = CARTE_GAIN_ECTS;
    pioche->cartes[1].valeur = 50;

    strcpy(pioche->cartes[2].titre, "Aide du BDE");
    strcpy(pioche->cartes[2].description, "Vous recevez 20 ECTS.");
    pioche->cartes[2].paquet = PAQUET_CHANCE;
    pioche->cartes[2].effet = CARTE_GAIN_ECTS;
    pioche->cartes[2].valeur = 20;

    strcpy(pioche->cartes[3].titre, "Amende stationnement");
    strcpy(pioche->cartes[3].description, "Vous perdez 15 ECTS.");
    pioche->cartes[3].paquet = PAQUET_CHANCE;
    pioche->cartes[3].effet = CARTE_PERTE_ECTS;
    pioche->cartes[3].valeur = 15;

    strcpy(pioche->cartes[4].titre, "Materiel casse en TP");
    strcpy(pioche->cartes[4].description, "Vous perdez 25 ECTS.");
    pioche->cartes[4].paquet = PAQUET_CHANCE;
    pioche->cartes[4].effet = CARTE_PERTE_ECTS;
    pioche->cartes[4].valeur = 25;

    strcpy(pioche->cartes[5].titre, "Retard rendu projet");
    strcpy(pioche->cartes[5].description, "Vous perdez 20 ECTS.");
    pioche->cartes[5].paquet = PAQUET_CHANCE;
    pioche->cartes[5].effet = CARTE_PERTE_ECTS;
    pioche->cartes[5].valeur = 20;

    strcpy(pioche->cartes[6].titre, "Izly en negatif");
    strcpy(pioche->cartes[6].description, "Vous perdez 10 ECTS.");
    pioche->cartes[6].paquet = PAQUET_CHANCE;
    pioche->cartes[6].effet = CARTE_PERTE_ECTS;
    pioche->cartes[6].valeur = 10;

    strcpy(pioche->cartes[7].titre, "Avance de 3 cases");
    strcpy(pioche->cartes[7].description, "Avancez de 3 cases.");
    pioche->cartes[7].paquet = PAQUET_CHANCE;
    pioche->cartes[7].effet = CARTE_DEPLACEMENT_RELATIF;
    pioche->cartes[7].valeur = 3;

    strcpy(pioche->cartes[8].titre, "Recule de 2 cases");
    strcpy(pioche->cartes[8].description, "Reculez de 2 cases.");
    pioche->cartes[8].paquet = PAQUET_CHANCE;
    pioche->cartes[8].effet = CARTE_DEPLACEMENT_RELATIF;
    pioche->cartes[8].valeur = -2;

    strcpy(pioche->cartes[9].titre, "Semaine de partiels");
    strcpy(pioche->cartes[9].description, "Impossible de construire au prochain tour.");
    pioche->cartes[9].paquet = PAQUET_CHANCE;
    pioche->cartes[9].effet = CARTE_BLOQUE_CONSTRUCTION;
    pioche->cartes[9].valeur = 1;

    strcpy(pioche->cartes[10].titre, "Sortie de prison");
    strcpy(pioche->cartes[10].description, "Conservez cette carte jusqu'à utilisation.");
    pioche->cartes[10].paquet = PAQUET_CHANCE;
    pioche->cartes[10].effet = CARTE_SORTIE_PRISON;
    pioche->cartes[10].valeur = 1;

    int i;
    for (i = 0; i < pioche->nb_cartes; i++) {
        pioche->cartes[i].etat = ETAT_CARTE_DANS_PAQUET;
        pioche->cartes[i].owner_number = 0;
        pioche->ordre[i] = i;
    }

    melanger_pioche_chance(pioche);
}

void init_pioche_communaute(PiocheCommunaute *pioche) {
    if (pioche == NULL) return;

    pioche->nb_cartes = NB_CARTES_COMMUNAUTE;
    pioche->sommet = 0;

    strcpy(pioche->cartes[0].titre, "Plan sobriete energetique");
    strcpy(pioche->cartes[0].description, "Tous les joueurs paient 10 ECTS.");
    pioche->cartes[0].paquet = PAQUET_COMMUNAUTE;
    pioche->cartes[0].effet = CARTE_PERTE_ECTS;
    pioche->cartes[0].valeur = 10;

    strcpy(pioche->cartes[1].titre, "Subvention campus vert");
    strcpy(pioche->cartes[1].description, "Tous les joueurs gagnent 10 ECTS.");
    pioche->cartes[1].paquet = PAQUET_COMMUNAUTE;
    pioche->cartes[1].effet = CARTE_GAIN_ECTS;
    pioche->cartes[1].valeur = 10;

    strcpy(pioche->cartes[2].titre, "Travaux reseau chaleur");
    strcpy(pioche->cartes[2].description, "Taxe ecologique de base 20 ECTS.");
    pioche->cartes[2].paquet = PAQUET_COMMUNAUTE;
    pioche->cartes[2].effet = CARTE_TAXE_ECO;
    pioche->cartes[2].valeur = 20;

    strcpy(pioche->cartes[3].titre, "Reparation degats climatiques");
    strcpy(pioche->cartes[3].description, "Taxe ecologique de base 30 ECTS.");
    pioche->cartes[3].paquet = PAQUET_COMMUNAUTE;
    pioche->cartes[3].effet = CARTE_TAXE_ECO;
    pioche->cartes[3].valeur = 30;

    strcpy(pioche->cartes[4].titre, "Hausse du cout de la vie");
    strcpy(pioche->cartes[4].description, "Tous les joueurs paient 5 ECTS.");
    pioche->cartes[4].paquet = PAQUET_COMMUNAUTE;
    pioche->cartes[4].effet = CARTE_PERTE_ECTS;
    pioche->cartes[4].valeur = 5;

    strcpy(pioche->cartes[5].titre, "Prime associative");
    strcpy(pioche->cartes[5].description, "Tous les joueurs gagnent 5 ECTS.");
    pioche->cartes[5].paquet = PAQUET_COMMUNAUTE;
    pioche->cartes[5].effet = CARTE_GAIN_ECTS;
    pioche->cartes[5].valeur = 5;

    strcpy(pioche->cartes[6].titre, "Inondations");
    strcpy(pioche->cartes[6].description, "Taxe ecologique de base 25 ECTS.");
    pioche->cartes[6].paquet = PAQUET_COMMUNAUTE;
    pioche->cartes[6].effet = CARTE_TAXE_ECO;
    pioche->cartes[6].valeur = 25;

    strcpy(pioche->cartes[7].titre, "Canicule");
    strcpy(pioche->cartes[7].description, "Taxe ecologique de base 15 ECTS.");
    pioche->cartes[7].paquet = PAQUET_COMMUNAUTE;
    pioche->cartes[7].effet = CARTE_TAXE_ECO;
    pioche->cartes[7].valeur = 15;

    int i;
    for (i = 0; i < pioche->nb_cartes; i++) {
        pioche->cartes[i].etat = ETAT_CARTE_DANS_PAQUET;
        pioche->cartes[i].owner_number = 0;
        pioche->ordre[i] = i;
    }

    melanger_pioche_communaute(pioche);
}

/* ------------------------------------------------------------------------- */
/* MELANGE                                                                   */
/* ------------------------------------------------------------------------- */

void melanger_pioche_chance(PiocheChance *pioche) {
    if (pioche == NULL) return;

    init_hasard_once();

    int i;
    for (i = pioche->nb_cartes - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        echanger_int(&pioche->ordre[i], &pioche->ordre[j]);
    }

    pioche->sommet = 0;
}

void melanger_pioche_communaute(PiocheCommunaute *pioche) {
    if (pioche == NULL) return;

    init_hasard_once();

    int i;
    for (i = pioche->nb_cartes - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        echanger_int(&pioche->ordre[i], &pioche->ordre[j]);
    }

    pioche->sommet = 0;
}

/* ------------------------------------------------------------------------- */
/* PIOCHE                                                                    */
/* ------------------------------------------------------------------------- */

Carte *piocher_carte_chance(PiocheChance *pioche, int numero_joueur) {
    if (pioche == NULL) return NULL;

    int idx = trouver_carte_disponible_chance(pioche);
    if (idx == -1) return NULL;

    Carte *carte = &pioche->cartes[idx];

    if (carte->effet == CARTE_SORTIE_PRISON) {
        donner_carte_a_joueur(carte, numero_joueur);
    }

    return carte;
}

Carte *piocher_carte_communaute(PiocheCommunaute *pioche, int numero_joueur) {
    (void)numero_joueur; 
    if (pioche == NULL) return NULL;

    int idx = trouver_carte_disponible_communaute(pioche);
    if (idx == -1) return NULL;

    return &pioche->cartes[idx];
}

/* ------------------------------------------------------------------------- */
/* GESTION DES CARTES POSSEDEES                                              */
/* ------------------------------------------------------------------------- */

int est_carte_sortie_prison(const Carte *carte) {
    if (carte == NULL) return 0;
    return carte->effet == CARTE_SORTIE_PRISON;
}

void donner_carte_a_joueur(Carte *carte, int numero_joueur) {
    if (carte == NULL) return;

    carte->etat = ETAT_CARTE_EN_MAIN;
    carte->owner_number = numero_joueur;
}

void utiliser_carte(Carte *carte) {
    if (carte == NULL) return;

    carte->etat = ETAT_CARTE_UTILISEE;
    carte->owner_number = 0;
}

void remettre_carte_dans_paquet(Carte *carte) {
    if (carte == NULL) return;

    carte->etat = ETAT_CARTE_DANS_PAQUET;
    carte->owner_number = 0;
}

int joueur_possede_sortie_prison(const Carte *cartes, int nb_cartes, int numero_joueur) {
    if (cartes == NULL || nb_cartes <= 0) return 0;

    int i;
    for (i = 0; i < nb_cartes; i++) {
        if (cartes[i].effet == CARTE_SORTIE_PRISON &&
            cartes[i].etat == ETAT_CARTE_EN_MAIN &&
            cartes[i].owner_number == numero_joueur) {
            return 1;
        }
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* OUTILS                                                                    */
/* ------------------------------------------------------------------------- */

int calcul_taxe_ecologique(int base, EcoGrade indice) {
    double coeff = 1.0;

    switch (indice) {
        case GRADE_A: coeff = 0.5; break;
        case GRADE_B: coeff = 0.8; break;
        case GRADE_C: coeff = 1.0; break;
        case GRADE_D: coeff = 1.2; break;
        case GRADE_E: coeff = 1.5; break;
        case GRADE_F: coeff = 2.0; break;
        default: coeff = 1.0; break;
    }

    return (int)(base * coeff + 0.5);
}