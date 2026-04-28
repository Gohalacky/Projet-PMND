#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "plateau.h"
#include "game_state.h"
#include "engine_logic.h"
#include "../Modele/player.h"
#include "game_engine_console.h"
 
int main() {
    srand(time(NULL));
 
    // Chargement des données
    Property *tableau_prop[NB_PROPRIETE] = {NULL};
    Gares *tableau_gar[NB_GARE] = {NULL};
    Compagnies *tableau_comp[NB_COMPAGNIE] = {NULL};
    Prison *jail = init_prison();
 
    printf("Chargement des fichiers...\n");
    int nbp = chargement_propriete_loyer(tableau_prop, "loyer_insapoly.txt");
    int nbg = chargement_GARE_loyer(tableau_gar, "gares.txt");
    int nbc = chargement_COMPAGNIE_loyer(tableau_comp, "compagnie.txt");
 
    // Création du plateau
    Plateau mon_plateau;
    init_plateau(&mon_plateau, tableau_prop, tableau_gar, tableau_comp);
    printf("Plateau prêt : %d propriétés, %d gares, %d compagnies.\n", nbp, nbg, nbc);
 
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
            nom[strcspn(nom, "\n")] = 0;
            liste_joueurs[i] = init_player(nom, i + 1);
        }
    }
 
    // Initialisation du GameContext
    GameContext ctx;
    ctx.nb_players = nb_joueurs;
    for (int i = 0; i < nb_joueurs; i++) ctx.players[i] = liste_joueurs[i];
    for (int i = 0; i < NB_PROPRIETE; i++) ctx.properties[i] = tableau_prop[i];
    for (int i = 0; i < NB_GARE; i++) ctx.gares[i] = tableau_gar[i];
    for (int i = 0; i < NB_COMPAGNIE; i++) ctx.compagnies[i] = tableau_comp[i];
    for (int i = 0; i < MAX_PLAYER; i++) ctx.bloque_construction[i] = 0;
    ctx.indice_eco_global = GRADE_A;
    ctx.numero_round = 1;
    init_pioche_chance(&ctx.pioche_chance);
    init_pioche_communaute(&ctx.pioche_communaute);
    evenements_init(&ctx.evenement_mondial);
 
    // Mode de jeu
    int mode_jeu = 0, max_tours = -1, admin_mode = 0;
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
 
    printf("\n=================================");
    printf("\n       BIENVENUE SUR INSA'POLY   ");
    printf("\n=================================\n");
    printf("\n=== QUE LA PARTIE COMMENCE ! ===\n");
 
    int continuer = 1;
    int tour_actuel = 1;
 
    while (continuer) {
        if (max_tours != -1 && tour_actuel > max_tours) break;
 
        for (int i = 0; i < nb_joueurs; i++) {
            if (liste_joueurs[i]->credit < 0) continue;
 
            engine_play_turn(liste_joueurs[i], liste_joueurs, tableau_prop, tableau_gar,
                             tableau_comp, &mon_plateau, jail, admin_mode, nb_joueurs, &ctx);
 
            int survivants = 0;
            for (int k = 0; k < nb_joueurs; k++)
                if (liste_joueurs[k]->credit >= 0) survivants++;
 
            if (survivants <= 1 && mode_jeu == 2) { continuer = 0; break; }
        }
 
        // Fin de round : événements mondiaux + mise à jour indice éco global
        logic_update_global_eco(&ctx);
        logic_handle_global_events(&ctx);
        ctx.numero_round++;
 
        int survivants = 0;
        for (int k = 0; k < nb_joueurs; k++)
            if (liste_joueurs[k]->credit >= 0) survivants++;
        if (survivants <= 1) continuer = 0;
 
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
            printf("%d ECTS (Cash: %d | Éco: %d)\n", valeur_totale, liste_joueurs[i]->credit, liste_joueurs[i]->eco_index);
            if (valeur_totale > max_valeur) { max_valeur = valeur_totale; gagnant = liste_joueurs[i]; }
        }
    }
    if (gagnant)
        printf("\n LE VAINQUEUR EST : %s avec une fortune de %d ECTS !\n", gagnant->name, max_valeur);
 
    // Libération
    for (int i = 0; i < nb_joueurs; i++) free(liste_joueurs[i]);
    for (int i = 0; i < NB_PROPRIETE; i++) if (tableau_prop[i]) free(tableau_prop[i]);
    for (int i = 0; i < NB_GARE; i++) if (tableau_gar[i]) free(tableau_gar[i]);
    for (int i = 0; i < NB_COMPAGNIE; i++) if (tableau_comp[i]) free(tableau_comp[i]);
    free(jail);
 
    return 0;
}