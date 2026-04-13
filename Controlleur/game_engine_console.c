#include <stdio.h>
#include <stdlib.h>
#include "game_engine_console.h"
#include "plateau.h"

/**
 * Lancer de dés : gère le mode Admin et le compteur de doubles
 */
int engine_roll_dice(int admin_mode, int *compteur_doubles) {
    int de1, de2;

    if (admin_mode) {
        int choix = 0;
        while (choix < 1 || choix > 40) {
            printf("\n[MODE ADMIN] Entrez le déplacement souhaité (1-40) : ");
            if (scanf("%d", &choix) != 1) while(getchar() != '\n');
        }
        *compteur_doubles = 0; // Pas de doubles en mode admin par simplicité
        return choix;
    }

    de1 = rand() % 6 + 1;
    de2 = rand() % 6 + 1;

    if (de1 == de2) {
        (*compteur_doubles)++;
        printf(">> DOUBLE de %d ! (Cumul : %d)\n", de1, *compteur_doubles);
    } else {
        *compteur_doubles = 0;
        printf(">> Dés : %d + %d = %d\n", de1, de2, de1 + de2);
    }

    return de1 + de2;
}

/**
 * Gère le tour complet (Boucle de doubles + Prison)
 */
void engine_play_turn(Player *p, Player *all_players[], Property *tableau_prop[], 
                      Gares *tableau_gare[], Compagnies *tableau_comp[], 
                      Plateau *plateau, Prison *jail, int admin_mode) {
    
    if (!p || !plateau || !jail) return;

    printf("\n========================================");
    printf("\n>> TOUR DE : %s", p->name);
    printf("\n>> Portefeuille : %d ECTS | Grade : %d", p->credit, p->eco_index);
    printf("\n========================================\n");

    int compteur_doubles = 0;
    int rejouer = 0;

    do {
        rejouer = 0;
        int total_des = engine_roll_dice(admin_mode, &compteur_doubles);

        // GESTION PRISON
        if (is_in_jail(p, jail)) {
            if (compteur_doubles > 0) {
                printf(">> Coup de chance ! Un double vous fait sortir de prison.\n");
                get_out_of_jail(p, jail);
            } else {
                if (istreetour_in_jail(p, jail)) {
                    printf(">> 3ème tour en prison : vous devez payer 50 ECTS pour sortir.\n");
                    pay_to_get_out(p, jail);
                } else {
                    printf(">> Vous êtes en prison. Voulez-vous payer 50 ECTS pour sortir ? (1:Oui/0:Non) : ");
                    int rep;
                    if (scanf("%d", &rep) == 1 && rep == 1) {
                        pay_to_get_out(p, jail);
                    } else {
                        printf(">> Vous restez en cellule ce tour-ci.\n");
                        add_tour_jail(p, jail);
                        return; // Fin du tour immédiate
                    }
                }
            }
        }

        //  GESTION 3 DOUBLES 
        if (compteur_doubles == 3) {
            printf("!! %s a fait 3 doubles consécutifs ! Direction la PRISON !!\n", p->name);
            go_to_jail(p, jail);
            return;
        }

        // DÉPLACEMENT ET RÉSOLUTION
        player_move(p, total_des);
        engine_resolve_case(p, all_players, tableau_prop, tableau_gare, tableau_comp, plateau, jail, total_des);

        // On rejoue si on a fait un double et qu'on n'est pas en prison
        if (compteur_doubles > 0 && !is_in_jail(p, jail)) {
            printf("\n>> C'est un double ! %s rejoue...\n", p->name);
            rejouer = 1;
        }

    } while (rejouer);

    // On incrémente le tour de prison si le joueur y est encore (ou vient d'y entrer)
    if (is_in_jail(p, jail)) add_tour_jail(p, jail);
}

/**
 * Résolution des actions sur la case (Logique d'achat et loyer)
 */
void engine_resolve_case(Player *p, Player *all_players[], Property *tableau_prop[], 
                         Gares *tableau_gare[], Compagnies *tableau_comp[], 
                         Plateau *plateau, Prison *jail, int total_des) {

    Case *current_case = &plateau->cases[p->position];
    printf("[Position] : %d (%s)\n", p->position, current_case->nom_affichage);

    switch (current_case->type) {
        
        case CASE_DEPART:
            printf(">> Passage par le Départ : +200 ECTS !\n");
            player_add_credits(p, 200);
            break;

        case CASE_TERRAIN: {
            Property *prop = (Property *)current_case->data;
            if (!prop) break;

            if (prop->owner == 0) {
                if (p->credit >= prop->value) {
                    printf(">> Terrain libre (%d ECTS). Acheter ? (1:Oui/0:Non) : ", prop->value);
                    int choix;
                    if (scanf("%d", &choix) == 1 && choix == 1) {
                        player_remove_credits(p, prop->value);
                        player_add_property(p, prop);
                        printf(">> Félicitations, vous possédez %s !\n", prop->name);
                    }
                }
            } else if (prop->owner == p->number) {
                // LOGIQUE CONSTRUCTION
                if (full_color(tableau_prop, p, prop->color)) {
                    if (prop->level < 3) {
                        if (check_uniformity(tableau_prop, prop, 1)) {
                            printf(">> Améliorer %s (%d ECTS) ?\n", prop->name, prop->construction_cost);
                            printf("1: Maison | 2: Social | 3: Parc | 0: Non : ");
                            int type_c;
                            if (scanf("%d", &type_c) == 1 && type_c >= 1 && type_c <= 3) {
                                if (p->credit >= prop->construction_cost) {
                                    add_level(tableau_prop, prop, p, type_c);
                                    printf(">> Niveau %d atteint !\n", prop->level);
                                } else printf(">> Crédits insuffisants.\n");
                            }
                        } else printf(">> Uniformité requise : construisez ailleurs dans le quartier %s.\n", color_names[prop->color]);
                    } else printf(">> Niveau maximum atteint.\n");
                }
            } else {
                Player *proprio = all_players[prop->owner - 1];
                printf(">> Propriété de %s. Paiement du loyer...\n", proprio->name);
                must_rent_Property(tableau_prop, prop, p, proprio);
            }
            break;
        }

        case CASE_GARE: {
            Gares *gare = (Gares *)current_case->data;
            if (!gare) break;
            if (gare->owner == 0) {
                printf(">> Gare libre (%d ECTS). Acheter ? (1:Oui/0:Non) : ", gare->value);
                int choix;
                if (scanf("%d", &choix) == 1 && choix == 1) {
                    player_remove_credits(p, gare->value);
                    player_add_gar(p, gare);
                }
            } else if (gare->owner != p->number) {
                Player *proprio = all_players[gare->owner - 1];
                must_rent_Gares(tableau_gare, gare, p, proprio);
            }
            break;
        }

        case CASE_COMPAGNIE: {
            Compagnies *comp = (Compagnies *)current_case->data;
            if (!comp) break;
            if (comp->owner == 0) {
                printf(">> Compagnie libre (%d ECTS). Acheter ? (1:Oui/0:Non) : ", comp->value);
                int choix;
                if (scanf("%d", &choix) == 1 && choix == 1) {
                    player_remove_credits(p, comp->value);
                    player_add_comp(p, comp);
                }
            } else if (comp->owner != p->number) {
                Player *proprio = all_players[comp->owner - 1];
                must_rent_Compagnies(tableau_comp, comp, p, proprio, total_des);
            }
            break;
        }
    
        case CASE_PRISON:{ 
            printf(">> Malchance ! Direction la prison !\n");
            go_to_jail(p, jail); 
            
            break;
        }

        default:
            printf(">> Case spéciale : repos.\n");
            break;
    }

    // Mise à jour éco fin de tour
    Property board_static[NB_PROPRIETE];
    int valid_count = 0;
    for (int i = 0; i < NB_PROPRIETE; i++) {
        if (tableau_prop[i]) board_static[valid_count++] = *(tableau_prop[i]);
    }
    if (valid_count > 0) calcul_moyenne_eco_index_player(p, board_static, valid_count);
}