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
int engine_roll_dice( int *compteur_doubles) {
    
    int de1 = rand() % 6 + 1;
    int de2 = rand() % 6 + 1;
    
    // Si c'est un double, on incrémente la valeur pointée. 
    // Sinon, on la remet à zéro.
    if (de1 == de2) {
        (*compteur_doubles)++; // Attention aux parenthèses, elles sont obligatoires 
        printf("Double de %d !\n",de1);
    } else {
        *compteur_doubles = 0;
        printf("%d + %d = %d\n",de1,de2,de1+de2);
    }
    
    return de1 + de2;
}


void engine_play_turn(Player *p, Player *all_players[],Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[],
                      Plateau *plateau, Prison * jail) {
 
    if (!p || !plateau) return;
  
    int compteur_doubles =0; // compteur de double pour la case prison = 0 au début de tour
    do{                            // on fait notre tour une premiere fois, le do while sert si le premier lancé était un double de rejoué et de refaire son tour
        int total_des = engine_roll_dice(&compteur_doubles);
        printf("[Dés] : Résultat = %d\n", total_des);
        if((compteur_doubles == 1) && is_in_jail(p,jail)){
            get_out_of_jail(p,jail);
        }
        if((compteur_doubles == 0) && is_in_jail(p,jail)){
            if(istreetour_in_jail(p,jail))
                pay_to_get_out(p,jail);
            else{
                printf("Voulez vous payez 50 pour sortir de prison ? (1:Oui/0:Non)\n");
                int rep;
                if (scanf("%d", &rep) == 1 && rep == 1){
                    pay_to_get_out(p,jail);
                }
            }
        }
        if((compteur_doubles == 3 )&& ( is_in_jail(p,jail) == 0)){
            printf("%s à fait 3 doubles à la suite !", p->name);
            go_to_jail(p,jail);
        }
        if(is_in_jail(p,jail) == 0){
            player_move(p, total_des);
        }
 
        engine_resolve_case(p, all_players, tableau_prop, tableau_gare, tableau_comp, plateau, total_des,jail);
    }while((compteur_doubles == 1) || (compteur_doubles == 2));
    add_tour_jail(p,jail);
}

int engine_roll_and_move(Player* p, const Plateau* plateau, int * compteur_doubles) {
    if (!p || !plateau) return 0;
    int total_des = engine_roll_dice(compteur_doubles);
    player_move(p, total_des);
    return total_des;
}

void engine_resolve_case(Player *p, Player *all_players[],Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[],Plateau *plateau, int total_des, Prison * jail) {
 
    if (!p || !plateau) return;
 
    Case *current_case = &plateau->cases[p->position];
 
    switch (current_case->type) {
 
        case CASE_DEPART:
            player_add_credits(p, 200);
            break;
        
        case CASE_GOTOJAIL:
            go_to_jail(p,jail);
            break ;

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