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

    // Boucle principal
    printf("\n=================================");
    printf("\n       BIENVENUE SUR INSA'POLY   ");
    printf("\n=================================\n");
    
    int continuer = 1;
    for (int tour = 1; tour <= 10 && continuer; tour++) {
        for (int i = 0; i < 2; i++) {
            engine_play_turn(liste_joueurs[i], liste_joueurs,  tableau_prop, &mon_plateau);
            
            // Gestion de l'arrêt
            printf("\n[Entrée] Continuer | [q + Entrée] Quitter : ");
            char buffer[10];
            if (fgets(buffer, sizeof(buffer), stdin)) {
                if (buffer[0] == 'q' || buffer[0] == 'Q') {
                    continuer = 0;
                    break;
                }
            }
        }
    }

    // LIBÉRATION DE LA MÉMOIRE
    printf("\nFermeture du jeu et libération de la mémoire...\n");
    for (int i = 0; i < nb_joueurs; i++) free(liste_joueurs[i]);
    
    for (int i = 0; i < NB_PROPRIETE; i++) if (tableau_prop[i]) free(tableau_prop[i]);
    for (int i = 0; i < NB_GARE; i++) if (tableau_gar[i]) free(tableau_gar[i]);
    for (int i = 0; i < NB_COMPAGNIE; i++) if (tableau_comp[i]) free(tableau_comp[i]);

    return 0;
}