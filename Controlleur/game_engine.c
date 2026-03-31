#include <stdio.h>
#include <stdlib.h>
#include "game_engine.h"
#include "plateau.h"

/**
 * Simule un lancer de 2 dés de 6 faces.
 */
int engine_roll_dice() {
    return (rand() % 6 + 1) + (rand() % 6 + 1);
}

/**
 * Gère l'intégralité du tour d'un joueur.
 */
void engine_play_turn(Player *p, Player *all_players[],
                      Property *tableau_prop[], Plateau *plateau) {
    
    // Sécurité
    if (p==NULL || plateau==NULL) return;

    printf("\n--- TOUR DE %s ---\n", p->name);
    printf("Portefeuille : %d ECTS | Grade Éco : %d\n", p->credit, p->eco_index);

    // DÉPLACEMENT
    int total_des = engine_roll_dice();
    printf("[Dés] : Résultat = %d\n", total_des);
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
            if (prop==NULL) break;

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
            } else if (prop->owner != p->number) {
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
                    } else {
                        printf(">> Niveau maximum (3) déjà atteint ici.\n");
                    }
                } else {
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

        case CASE_GARE:
            printf(">> Case Gare : logique de loyer gare à faire.\n");
            break;

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
}