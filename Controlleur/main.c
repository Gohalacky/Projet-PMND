#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "plateau.h"
#include "../Modele/player.h"
#include "game_engine_console.h"


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
    Prison * jail = init_prison();

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
            if (liste_joueurs[i]->credit < 0) continue; // Joueur éliminé

            engine_play_turn(liste_joueurs[i], liste_joueurs, tableau_prop, tableau_gar, tableau_comp, &mon_plateau, jail, admin_mode, nb_joueurs);
            
            // Gestion de la faillite immédiate
            if (liste_joueurs[i]->credit == 0) {
                engine_handle_bankruptcy(liste_joueurs[i], tableau_prop, tableau_gar, tableau_comp);
            }

            // Vérification : reste-t-il plus d'un joueur ?
            int survivants = 0;
            for (int k = 0; k < nb_joueurs; k++) if (liste_joueurs[k]->credit >= 0) survivants++;

            if (survivants <= 1) {
                continuer = 0;
                break;
            }

            // Attente entre les joueurs
            if (i < nb_joueurs - 1) {
                printf("\n[Entrée] Joueur suivant...");
                getchar();
            }
        }
        tour_actuel++;
    }

    // CALCUL DU GAGNANT
    printf("\n=================================");
    printf("\n       FIN DE LA PARTIE          ");
    printf("\n=================================\n");

    Player *gagnant = NULL;
    int max_valeur = -2;

    for (int i = 0; i < nb_joueurs; i++) {
        int valeur_totale = engine_get_player_total_value(liste_joueurs[i], tableau_prop, tableau_gar, tableau_comp);
        
        printf("%s : ", liste_joueurs[i]->name);
        if (valeur_totale < 0) printf("ÉLIMINÉ\n");
        else {
            printf("%d ECTS de valeur totale (Cash: %d)\n", valeur_totale, liste_joueurs[i]->credit);
            if (valeur_totale > max_valeur) {
                max_valeur = valeur_totale;
                gagnant = liste_joueurs[i];
            }
        }
    }

    if (gagnant) {
        printf("\n🏆 LE VAINQUEUR EST : %s avec une fortune de %d ECTS !\n", gagnant->name, max_valeur);
    }

    // --- LIBÉRATION ---
    for (int i = 0; i < nb_joueurs; i++) free(liste_joueurs[i]);
    for (int i = 0; i < NB_PROPRIETE; i++) if (tableau_prop[i]) free(tableau_prop[i]);
    for (int i = 0; i < NB_GARE; i++) if (tableau_gar[i]) free(tableau_gar[i]);
    for (int i = 0; i < NB_COMPAGNIE; i++) if (tableau_comp[i]) free(tableau_comp[i]);
    free(jail);

    return 0;
}