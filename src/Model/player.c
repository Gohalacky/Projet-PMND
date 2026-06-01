#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"
#include "property.h"

Player* init_player(const char* name, int number) {
    // Allocation dynamique
    Player* p = malloc(sizeof(Player));
    
    if (p == NULL) {
        return NULL; // Vérification du malloc
    }

    // Initialisation des champs
    strncpy(p->name, name, 19);
    p->name[19] = '\0'; // Terminer la chaine de caractère
    
    p->number = number;
    p->credit = 1500;    // Valeur de départ standard
    p->position = 0;      // Case départ
    p->eco_index = GRADE_A; // Joueur commence avec Grade A

    return p;
}


void calcul_moyenne_eco_index_player(Player *p, Property **board, int board_size) {
    if (p == NULL || board == NULL) return;

    int somme = 0;
    int nb_propriete = 0;
    int moyenne = 0;

    for (int i = 0; i < board_size; i++) {
        if (board[i] != NULL && board[i]->owner == p->number) {
            somme += board[i]->indice;
            nb_propriete++;
        }
    }

    // Si le joueur ne possède rien, il reste Grade A
    if (nb_propriete == 0) {
        p->eco_index = GRADE_A;
    } else {
        // Calcul de la moyenne arrondie à l'entier le plus proche
           moyenne = (somme + (nb_propriete/ 2)) / nb_propriete;
        
	
        if (moyenne < GRADE_A) moyenne = GRADE_A;
        if (moyenne > GRADE_F) moyenne = GRADE_F;
        
        p->eco_index = (EcoGrade)moyenne;
    }
}

void player_move(Player *p, int steps) {
    if (p == NULL) return;

    int old_position = p->position;
    // Modulo 40 pour rester sur le plateau
    p->position = (p->position + steps) % 40;

    // Si la nouvelle position est inférieure à l'ancienne, 
    // c'est qu'on a fait un tour complet (Passage par la case Départ)
    if (p->position < old_position) {
        printf("%s passe par la case départ et reçoit 200 ECTS !\n", p->name);
        player_add_credits(p, 200);
    }
}

void player_add_credits(Player *p, int montant) {
    if (p != NULL && montant > 0) {
        p->credit += montant;
        printf("%s à gagné %d crédits.\n",p->name,montant);
    }
}

//Retire des crédits. Retourne 0 si le joueur n'a plus assez d'argent. 1 sinon
int player_remove_credits(Player *p, int montant) {
    if (p == NULL) return 1;

    p->credit -= montant;
    printf("%s a perdu %d ECTS. (Nouveau solde : %d)\n", p->name, montant, p->credit);

    if (p->credit < 0) {
        p->credit = 0;
        return 0; // Signale l'insolvabilité, engine_resolve_debt prend le relais
    }
    
    return 1;
}

void player_add_property(Player *p, Property *prop) {
    if (p == NULL || prop == NULL) return;
    
    prop->owner = p->number;
    printf("%s a acquis %s !\n", p->name, prop->name);
}

void player_add_comp(Player *p, Compagnies *prop) {
    if (p == NULL || prop == NULL) return;
    
    prop->owner = p->number;
    printf("%s a acquis %s !\n", p->name, prop->name);
}

void player_add_gar(Player *p, Gares *prop) {
    if (p == NULL || prop == NULL) return;
    
    prop->owner = p->number;
    printf("%s a acquis %s !\n", p->name, prop->name);
}


void player_remove_property(Player *p, Property *prop) {
    if (p == NULL || prop == NULL) return;

    if (prop->owner == p->number) {
        prop->owner = 0; // 0 elle n'appartient à personne
        prop->type = 0 ; /*on enlève les constructions*/
        prop->level = 0;
	printf("%s a perdu %s !\n", p->name, prop->name);
    }
}

void player_remove_comp(Player *p, Compagnies *prop) {
    if (p == NULL || prop == NULL) return;

    if (prop->owner == p->number) {
        prop->owner = 0; // 0 elle n'appartient à personne
	printf("%s a perdu %s !\n", p->name, prop->name);
    }
}

void player_remove_gar(Player *p, Gares *prop) {
    if (p == NULL || prop == NULL) return;

    if (prop->owner == p->number) {
        prop->owner = 0; // 0 elle n'appartient à personne
	printf("%s a perdu %s !\n", p->name, prop->name);
    }
}