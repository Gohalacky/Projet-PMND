#include <stdio.h>
#include <stdlib.h>
#include "game_engine.h"

int engine_roll_dice() {
    return (rand() % 6 + 1) + (rand() % 6 + 1);
}

void engine_play_turn(Player *p,Player *all_players[],int nb_players,Property *tableau_prop[]) {
    printf("\n=== TOUR DE %s (Crédits: %d ECTS | IndiceEco: %d) ===\n",p->name,p->credit,p->eco_index);

    // Déplacement
    int des = engine_roll_dice();  //lancé des dés
    printf("Total des Dés : %d\n",des);
    player_move(p,des);

    // Identification de la case (dans le tableau de propriétés)
    int case_idx = p->position % NB_PROPRIETE; 
    Property *current_prop = tableau_prop[case_idx];

    printf("[Déplacement] Atterrissage sur : %s\n",current_prop->name);

    // Interaction avec la case
    if (current_prop->owner == 0) {
        // CASE LIBRE : Achat
        if (p->credit >= current_prop->value) {
            printf("Case libre ! Voulez-vous l'acheter pour %d ECTS ? (1:Oui / 0:Non) : ",current_prop->value);
            int choix;
            scanf("%d",&choix);
            if (choix == 1) {
                player_remove_credits(p,current_prop->value);
                player_add_property(p,current_prop);
            }
        } else {
            printf("Fonds insuffisants pour acheter cette propriété.\n");
        }
    } 
    else if (current_prop->owner != p->number) {
        // case occupée: Loyer
        Player *proprietaire = NULL;
        for(int i=0;i<nb_players;i++) {
            if(all_players[i]->number == current_prop->owner) {
                proprietaire = all_players[i];
            }
        }
        
        if (proprietaire != NULL) {
            printf("Propriété de %s. Paiement du loyer en cours...\n",proprietaire->name);
            must_rent_Property(tableau_prop,current_prop,p,proprietaire);
        }
    }
    else {
        // On tombe sur notre: Construction (regle simplifier pour tester nos fonctions)
        printf("Vous êtes chez vous. Voulez-vous améliorer ce terrain ?\n");
        printf("1 : Maison | 2 : Logement Social | 3 : Parc | 0 : Rien : ");
        int type_const;
        scanf("%d",&type_const);
        
        if (type_const > 0) {
            if (add_level(tableau_prop,current_prop,p,type_const)) {
                // Si on construit, on met à jour la note éco de la case
                if (type_const == 3) {
                    current_prop->indice = GRADE_A;
                }
                else if (type_const == 1) {
                    current_prop->indice = GRADE_C;
                }
                
                printf("Amélioration réussie ! Nouveau niveau : %d\n",current_prop->level);
            }
        }
    }

    // Mise à jour du bilan écologique du joueur (Fin de tour)
    Property board_static[NB_PROPRIETE];
    for(int i=0;i<NB_PROPRIETE;i++) {
        board_static[i] = *(tableau_prop[i]);
    }
    
    calcul_moyenne_eco_index_player(p, board_static, NB_PROPRIETE);
    
    printf("Fin du tour. Nouvel indice écologique de %s : Grade %d\n",p->name,p->eco_index);
    printf("===========================================\n");
}