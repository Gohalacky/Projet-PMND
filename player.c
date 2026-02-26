#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player.h"

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