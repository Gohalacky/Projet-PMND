#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "plateau.h"
#include "../Modele/player.h"
#include "game_engine.h"

int main() {
    srand(time(NULL));

    // --- 1. CHARGEMENT DES DONNEES ---
    Property *tableau_prop[NB_PROPRIETE] = {NULL};
    Gares *tableau_gar[NB_GARE] = {NULL};
    Compagnies *tableau_comp[NB_COMPAGNIE] = {NULL};

    printf("[INFO] Chargement des fichiers...\n");
    int nbp = chargement_propriete_loyer(tableau_prop, "loyer_insapoly.txt");
    int nbg = chargement_GARE_loyer(tableau_gar, "gares.txt");
    int nbc = chargement_COMPAGNIE_loyer(tableau_comp, "compagnie.txt");

    //INITIALISATION DU PLATEAU
    Plateau mon_plateau;
    init_plateau(&mon_plateau, tableau_prop, tableau_gar, tableau_comp);
    printf("[INFO] Plateau initialise avec %d terrains.\n", nbp);

    // INITIALISATION DES JOUEURS
    Player *j1 = init_player("Jean", 1);
    Player *j2 = init_player("Bob", 2);
    Player *liste_joueurs[2] = {j1, j2};

    // BOUCLE DE JEU (Simulation de 10 tours)
    printf("\n=== DEBUT DE LA PARTIE INSA'POLY ===\n");
    
    char buffer[10]; // Pour la pause
    for (int tour = 1; tour <= 10; tour++) {
        printf("\n--- TOUR %d ---\n", tour);
        
        for (int i = 0; i < 2; i++) {
        
            engine_play_turn(liste_joueurs[i], liste_joueurs, 2, tableau_prop);
            
            printf("\n(Entree pour continuer...)");
            fgets(buffer, sizeof(buffer), stdin);
        }
    }

    // NETTOYAGE
    printf("\n[INFO] Nettoyage de la memoire...\n");
    free(j1); free(j2);
    for (int i = 0; i < nbp; i++) if (tableau_prop[i]) free(tableau_prop[i]);
    for (int i = 0; i < nbg; i++) if (tableau_gar[i]) free(tableau_gar[i]);
    for (int i = 0; i < nbc; i++) if (tableau_comp[i]) free(tableau_comp[i]);

    printf("Fin du programme.\n");
    return 0;
}