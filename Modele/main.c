#include <stdio.h>
#include <stdlib.h>
#include "property.h"
#include "player.h"

int main(){
    printf("test \n");
    Property * plateau[NB_PROPRIETE] = {NULL}; /*création d'un tableau de toute les propriete*/
    int nb = chargement_propriete_loyer(plateau, "loyer_insapoly.txt"); /*chargement des 22 propriétés classiques*/
    
    
    /*à la fin du jeu on libère les propriétés qui sont chargées*/
    for (int i = 0; i < nb; i++) {
        if (plateau[i] != NULL){
            free(plateau[i]);
        }
    }
    return 0;
}
