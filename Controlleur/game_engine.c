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
    if (admin_mode) {
        int choix = 0;
        while (choix < 1 || choix > 40) {
            printf("\n[MODE ADMIN] Entrez le nombre de cases à avancer (1-40) : ");
            if (scanf("%d", &choix) != 1) {
                while(getchar() != '\n'); // Nettoyer si erreur
            }
        }
        return choix;
    }
    // Mode normal
    return (rand() % 6 + 1) + (rand() % 6 + 1);
}


void engine_play_turn(Player *p, Player *all_players[],Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[],
                      Plateau *plateau, int admin_mode) {
 
    if (!p || !plateau) return;
 
    printf("\n--- TOUR DE %s ---\n", p->name);
    printf("Portefeuille : %d ECTS | Grade Éco : %d\n", p->credit, p->eco_index);
 
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

void engine_resolve_case(Player *p, Player *all_players[],Property *tableau_prop[], Gares *tableau_gare[], Compagnies *tableau_comp[],
                         Plateau *plateau, int total_des) {
 
    if (!p || !plateau) return;
 
    Case *current_case = &plateau->cases[p->position];
    printf("[Position] : %d (%s)\n", p->position, current_case->nom_affichage);
 
    switch (current_case->type) {
 
        case CASE_DEPART:
            printf(">> Passage par le départ : +200 ECTS !\n");
            player_add_credits(p, 200);
            break;
 
        case CASE_TERRAIN: {
            Property *prop = (Property *)current_case->data;
            if (!prop) break;
 
            if (prop->owner == 0) {
                if (p->credit >= prop->value) {
                    printf(">> Terrain libre (%d ECTS). Voulez-vous acheter ? (1:Oui/0:Non) : ", prop->value);
                    int choix;
                    if (scanf("%d", &choix) == 1 && choix == 1) {
                        player_remove_credits(p, prop->value);
                        player_add_property(p, prop);
                        printf(">> Achat effectué !\n");
                    }
                }
            } else if (prop->owner == p->number) {
                if (full_color(tableau_prop, p, prop->color) && prop->level < 3
                    && check_uniformity(tableau_prop, prop, 1)) {
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
                printf(">> Gare de %s. Paiement du loyer\n", proprio->name);
                must_rent_Gares(tableau_gare, gare, p, proprio);
            }
            break;
        }
 
        case CASE_COMPAGNIE: {
            Compagnies *comp = (Compagnies *)current_case->data;
            if (!comp) break;
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
                printf(">> %s appartient à %s. Calcul du loyer...\n", comp->name, proprio->name);
                must_rent_Compagnies(tableau_comp, comp, p, proprio, total_des);
            }
            break;
        }
 
        default:
            printf(">> Case spéciale : pas d'action immédiate.\n");
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