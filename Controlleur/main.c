#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "plateau.h"
#include "../Modele/player.h"
#include "game_engine.h"


void vider_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


int main() {
    // Init aléatoire
    srand(time(NULL));

    // Chargement des données
    Property *tableau_prop[NB_PROPRIETE] = {NULL};
    Gares *tableau_gar[NB_GARE] = {NULL};
    Compagnies *tableau_comp[NB_COMPAGNIE] = {NULL};

    printf("Chargement des fichiers...\n");
    int nbp = chargement_propriete_loyer(tableau_prop, "loyer_insapoly.txt");
    int nbg = chargement_GARE_loyer(tableau_gar, "gares.txt");
    int nbc = chargement_COMPAGNIE_loyer(tableau_comp, "compagnie.txt");

    // Création du plateau
    Plateau mon_plateau;
    init_plateau(&mon_plateau, tableau_prop, tableau_gar, tableau_comp);
    printf("Plateau prêt : %d propriétés , %d gares chargées et %d compagnies chargées.\n" , nbp, nbg, nbc);

    // Nombre de joueurs
    int nb_joueurs = 0;
    while (nb_joueurs < 1 || nb_joueurs > 4) {
        printf("Nombre de joueurs (1-4) : ");
        if (scanf("%d", &nb_joueurs) != 1) vider_buffer();
    }
    vider_buffer();

    // Noms des joueurs
    Player *liste_joueurs[4] = {NULL};
    for (int i = 0; i < nb_joueurs; i++) {
        char nom[50];
        printf("Nom du joueur %d : ", i + 1);
        if (fgets(nom, sizeof(nom), stdin)) {
            nom[strcspn(nom, "\n")] = 0; // Enlever le \n à la fin
            liste_joueurs[i] = init_player(nom, i + 1);
        }
    }

    // Choix du mode de jeu
    int mode_jeu = 0;
    int max_tours = -1; // -1 signifie infini (mode faillite)
    int admin_mode = 0;
    printf("\nChoisissez le mode de fonctionnement :\n");
    printf("0. Mode Normal (Dés aléatoires)\n");
    printf("1. Mode Admin (Choix manuel des déplacements)\n");
    printf("Choix : ");
    scanf("%d", &admin_mode);
    vider_buffer();
    printf("\nChoisissez le mode de jeu :\n");
    printf("1. Nombre de tours limité\n");
    printf("2. Jusqu'à la première faillite (Mort subite)\n");
    printf("Choix : ");
    scanf("%d", &mode_jeu);
    
    if (mode_jeu == 1) {
        printf("Nombre de tours maximum : ");
        scanf("%d", &max_tours);
    }
    vider_buffer();


    // Boucle principal
    printf("\n=================================");
    printf("\n       BIENVENUE SUR INSA'POLY   ");
    printf("\n=================================\n");

    printf("\n=== QUE LA PARTIE COMMENCE ! ===\n");
    
    int continuer = 1;
    int tour_actuel = 1;
    
    while (continuer) {
        
        if (max_tours != -1 && tour_actuel > max_tours) break;

        printf("\n--- TOUR %d ---\n", tour_actuel);
        
        for (int i = 0; i < nb_joueurs; i++) {
            engine_play_turn(liste_joueurs[i], liste_joueurs, tableau_prop, tableau_gar ,tableau_comp,  &mon_plateau, admin_mode);
            
            // Vérification de la faillite (Mode 2)
            if (mode_jeu == 2 && liste_joueurs[i]->credit <= 0) {
                printf("\n!!! %s est en faillite ! FIN DE LA PARTIE !!!\n", liste_joueurs[i]->name);
                continuer = 0;
                break;
            }
            // Gestion de l'arrêt
            printf("\n[Entrée] Continuer | [q + Entrée] Quitter : ");
            char c = getchar();
            if (c == 'q' || c == 'Q') {
                continuer = 0;
                break;
            }
            if (c != '\n') vider_buffer();
        }
        tour_actuel++;
    }
    // Fin de partie
    printf("\n=================================");
    printf("\n       FIN DE LA PARTIE          ");
    printf("\n=================================\n");

    Player *gagnant = liste_joueurs[0];
    for (int i = 1; i < nb_joueurs; i++) {
        if (liste_joueurs[i]->credit > gagnant->credit) {
            gagnant = liste_joueurs[i];
        }
    }
    printf("LE GAGNANT EST : %s avec %d ECTS !\n", gagnant->name, gagnant->credit);

    // LIBÉRATION DE LA MÉMOIRE
    printf("\nFermeture du jeu et libération de la mémoire...\n");
    for (int i = 0; i < nb_joueurs; i++) free(liste_joueurs[i]);
    
    for (int i = 0; i < NB_PROPRIETE; i++) if (tableau_prop[i]) free(tableau_prop[i]);
    for (int i = 0; i < NB_GARE; i++) if (tableau_gar[i]) free(tableau_gar[i]);
    for (int i = 0; i < NB_COMPAGNIE; i++) if (tableau_comp[i]) free(tableau_comp[i]);

    return 0;
}