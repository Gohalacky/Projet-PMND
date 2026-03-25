#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "plateau.h"
#include "../Modele/player.h"
#include "game_engine.h"

int main() {
    // Initialisation de l'aléatoire
    srand(time(NULL));

    // CHARGEMENT DES DONNÉES
    Property *tableau_prop[NB_PROPRIETE] = {NULL};
    Gares *tableau_gar[NB_GARE] = {NULL};
    Compagnies *tableau_comp[NB_COMPAGNIE] = {NULL};

    printf("Chargement des fichiers...\n");
    int nbp = chargement_propriete_loyer(tableau_prop, "loyer_insapoly.txt");
    int nbg = chargement_GARE_loyer(tableau_gar, "gares.txt");
    int nbc = chargement_COMPAGNIE_loyer(tableau_comp, "compagnie.txt");

    // CRÉATION DU PLATEAU
    Plateau mon_plateau;
    init_plateau(&mon_plateau, tableau_prop, tableau_gar, tableau_comp);
    printf("Plateau prêt : %d propriétés , %d gares chargées et %d compagnies chargées.\n" , nbp, nbg, nbc);

    // INITIALISATION DES JOUEURS
    Player *j1 = init_player("Jean", 1);
    Player *j2 = init_player("Bob", 2);
    Player *liste_joueurs[2] = {j1, j2};

    // BOUCLE PRINCIPALE
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
    printf("\n[Système] Fermeture du jeu et libération de la mémoire...\n");
    free(j1); 
    free(j2);
    
    for (int i = 0; i < NB_PROPRIETE; i++) if (tableau_prop[i]) free(tableau_prop[i]);
    for (int i = 0; i < NB_GARE; i++) if (tableau_gar[i]) free(tableau_gar[i]);
    for (int i = 0; i < NB_COMPAGNIE; i++) if (tableau_comp[i]) free(tableau_comp[i]);

    return 0;
}