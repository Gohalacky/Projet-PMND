#include <stdio.h>
#include <stdlib.h>
#include "property.h"
#include "player.h"

int main(){
    Property * tableau_prop[NB_PROPRIETE] = {NULL}; /*création d'un tableau de toute les propriete*/
    Gares* tableau_gar[NB_GARE] = {NULL};
    Compagnies * tableau_comp[NB_COMPAGNIE] = {NULL};
    int nbp = chargement_propriete_loyer(tableau_prop, "loyer_insapoly.txt"); /*chargement des 22 propriétés classiques*/
    int nbg = chargement_GARE_loyer(tableau_gar, "gares.txt"); /*chargement des 4 Gares*/
    int nbc = chargement_COMPAGNIE_loyer(tableau_comp, "compagnie.txt"); /*chargement des 2 compagnies*/


    
    /*à la fin du jeu on libère les propriétés qui sont chargées*/
    for (int i = 0; i < nbp; i++) {
        if (tableau_prop[i] != NULL){
            free(tableau_prop[i]);
        }
    }
    for (int i = 0; i < nbg; i++) {
        if (tableau_gar[i] != NULL){
            free(tableau_gar[i]);
        }
    }
    for (int i = 0; i < nbc; i++) {
        if (tableau_comp[i] != NULL){
            free(tableau_comp[i]);
        }
    }
    return 0;
}
