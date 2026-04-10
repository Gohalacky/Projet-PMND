#include <stdio.h>
#include <stdlib.h>
#include "game_engine.h"
#include "plateau.h"



const char* engine_get_case_joueur_actif(int joueurActif,Player* joueurs[],const Plateau* plateau) {
    if (!joueurs[joueurActif] || !plateau) return "Inconnue";
    int pos = joueurs[joueurActif]->position;
    return plateau->cases[pos].nom_affichage;
}


/**  
 * Simule un lancer de 2 dés de 6 faces.
 */
int engine_roll_dice(int admin_mode) {
    return (rand() % 6 + 1) + (rand() % 6 + 1);
}


void engine_play_turn(Player *p, Player *all_players[],Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[],
                      Plateau *plateau, int admin_mode) {
 
    if (!p || !plateau) return;
  
    int total_des = engine_roll_dice(admin_mode);
    printf("[Dés] : Résultat = %d\n", total_des);
    player_move(p, total_des);
 
    engine_resolve_case(p, all_players, tableau_prop, tableau_gare, tableau_comp, plateau, total_des);
}

int engine_roll_and_move(Player* p, const Plateau* plateau) {
    if (!p || !plateau) return 0;
    int total_des = engine_roll_dice(0);
    player_move(p, total_des);
    return total_des;
}

void engine_resolve_case(Player *p, Player *all_players[],Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[],Plateau *plateau, int total_des) {
 
    if (!p || !plateau) return;
 
    Case *current_case = &plateau->cases[p->position];
 
    switch (current_case->type) {
 
        case CASE_DEPART:
            player_add_credits(p, 200);
            break;
 
        case CASE_TERRAIN: {
            Property *prop = (Property *)current_case->data;
            if (!prop) break;

            if (prop->owner !=0 && prop->owner == p->number) {
                Player *proprio = all_players[prop->owner - 1];
                must_rent_Property(tableau_prop, prop, p, proprio);
            }
            break;
        }
 
        case CASE_GARE: {
            Gares *gare = (Gares *)current_case->data;
            if (!gare) break;

            if (gare->owner != 0 && gare->owner != p->number) {
                Player *proprio = all_players[gare->owner - 1];
                must_rent_Gares(tableau_gare, gare, p, proprio);
            } 
            break;
        }
 
        case CASE_COMPAGNIE: {
            Compagnies *comp = (Compagnies *)current_case->data;
            if (!comp) break;

            if (comp->owner!=0 && comp->owner != p->number) {
                Player *proprio = all_players[comp->owner - 1];
                must_rent_Compagnies(tableau_comp, comp, p, proprio, total_des);
            }
            break;
        }
 
        default:
            break;
    }
 
    // Mise à jour écologique
    Property board_static[NB_PROPRIETE];
    int valid_count = 0;
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau_prop[i] != NULL)
            board_static[valid_count++] = *(tableau_prop[i]);
    }
    if (valid_count > 0)
        calcul_moyenne_eco_index_player(p, board_static, valid_count);
}