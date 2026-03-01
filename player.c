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


void calcul_moyenne_eco_index_player(Player *p, Property *board, int board_size) {
    if (p == NULL || board == NULL) return;

    int somme = 0;
    int nb_propriete = 0;
    int moyenne =0;

    for (int i = 0; i < board_size; i++) {
        // On vérifie si la propriété appartient au joueur
        if (board[i].owner == p->number) {
            somme += board[i].indice;
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