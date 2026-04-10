#include <stdio.h>
#include <stdlib.h>
#include "game_engine.h"
#include "plateau.h"
#include "tool.h"
#include "prison.h"




void engine_play_turn(Player *p, Player *all_players[],
                      Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[], Plateau *plateau, Prison * jail, int admin_mode) {
    
    // Sécurité
    if (p==NULL || plateau==NULL) return;

    printf("\n--- TOUR DE %s ---\n", p->name);
    printf("Portefeuille : %d ECTS | Grade Éco : %d\n", p->credit, p->eco_index);

    // DÉPLACEMENT
    int compteur_doubles =0; // compteur de double pour la case prison = 0 au début de tour
    do{                            // on fait notre tour une premiere fois, le do while sert si le premier lancé était un double de rejoué et de refaire son tour
        int total_des = engine_roll_dice(admin_mode,&compteur_doubles);
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
        player_move(p, total_des);

        // ANALYSE DE LA CASE
        Case *current_case = &plateau->cases[p->position];
        printf("[Position] : %d (%s)\n", p->position, current_case->nom_affichage);

        // ACTIONS SELON LE TYPE DE CASE
        switch (current_case->type) {
        
            case CASE_DEPART:
                printf(">> Passage par le départ : +200 ECTS !\n");
                player_add_credits(p, 200);
                break;

            case CASE_TERRAIN: {
                Property *prop = (Property *)current_case->data;
                if (prop==NULL){
                    break;
                }

                if (prop->owner == 0) {
                    // Achat possible
                    if (p->credit >= prop->value) {
                        printf(">> Terrain libre (%d ECTS). Voulez-vous acheter ? (1:Oui/0:Non) : ", prop->value);
                        int choix;
                        if (scanf("%d", &choix) == 1 && choix == 1) {
                            player_remove_credits(p, prop->value);
                            player_add_property(p, prop);
                            printf(">> Achat effectué !\n");
                        }
                    }
                } 
                else if (prop->owner != p->number) {
                    //ches soi : construction
                    if (full_color(tableau_prop, p, prop->color)) {
                        if (prop->level < 3) {
                            if (check_uniformity(tableau_prop, prop, 1)) {
                                printf(">> Voulez-vous améliorer %s ? (%d ECTS)\n", prop->name, prop->construction_cost);
                                printf("1: Maison | 2: Social | 3: Parc | 0: Non : ");
                                int type_c;
                                if (scanf("%d", &type_c) == 1 && type_c >= 1 && type_c <= 3) {
                                    if (p->credit >= prop->construction_cost) {
                                        add_level(tableau_prop, prop, p, type_c);
                                        printf(">> Construction validée ! Niveau actuel : %d\n", prop->level);
                                    } else {
                                        printf(">> Pas assez d'ECTS pour construire.\n");
                                    }
                                }
                            } else {
                                printf(">> Règle d'uniformité : construisez sur vos autres terrains %s d'abord.\n", color_names[prop->color]);
                            }
                        } 
                        else {
                            printf(">> Niveau maximum (3) déjà atteint ici.\n");
                        }   
                    } 
                    else {
                        printf(">> Possédez tout le quartier %s pour construire.\n", color_names[prop->color]);
                    }
                }
                else {
                    // Loyer à payer
                    Player *proprio = all_players[prop->owner - 1]; // On récupère le proprio
                    printf(">> Propriété de %s. Paiement du loyer...\n", proprio->name);
                    must_rent_Property(tableau_prop, prop, p, proprio);
                }
                break;
            }

            case CASE_GARE: {
                Gares *gare = (Gares *)current_case->data;
                if (!gare){
                    break;
                } 

                if (gare->owner == 0) {
                    if (p->credit >= gare->value) {
                        printf(">> Gare libre (%d ECTS). Acheter ? (1:Oui/0:Non) : ", gare->value);
                        int choix;
                        if (scanf("%d", &choix) == 1 && choix == 1) {
                            player_remove_credits(p, gare->value);
                            player_add_gar(p, gare);
                        }
                    }
                } else if (gare->owner != p->number) {
                    Player *proprio = all_players[gare->owner - 1];
                    printf(">> Gare de %s. Paiement du loyer \n", proprio->name);
                    must_rent_Gares(tableau_gare, gare, p, proprio); 
                }
                break;
            }

            case CASE_COMPAGNIE: {
                Compagnies *comp = (Compagnies *)current_case->data;
                if (!comp){
                    break;
                } 

                if (comp->owner == 0) {
                    if (p->credit >= comp->value) {
                        printf(">> Compagnie libre (%d ECTS). Acheter ? (1:Oui/0:Non) : ", comp->value);
                        int choix;
                        if (scanf("%d", &choix) == 1 && choix == 1) {
                            player_remove_credits(p, comp->value);
                            player_add_comp(p, comp);
                        }
                    }
                } else if (comp->owner != p->number) {
                    Player *proprio = all_players[comp->owner - 1];
                    printf(">> %s appartient à %s. Calcul du loyer (Dés x Multiplicateur)...\n", comp->name, proprio->name);
                    must_rent_Compagnies(tableau_comp, comp, p, proprio, total_des);
                }
                break;
            }

            default:
                printf(">> Case spéciale : pas d'action immédiate.\n");
                break;
        }

        // Mise à jour écologique (Fin de tour)
        // On prépare une liste propre pour le calcul de l'indice
        Property board_static[NB_PROPRIETE];
        int valid_count = 0;

        for (int i = 0; i < NB_PROPRIETE; i++) {
            if (tableau_prop[i] != NULL) {
                board_static[valid_count] = *(tableau_prop[i]);
                valid_count++;
            }
        }

        if (valid_count > 0) {
            calcul_moyenne_eco_index_player(p, board_static, valid_count);
        }
    }while((compteur_doubles == 1) || (compteur_doubles == 2));
    add_tour_jail(p,jail);
}