#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prison.h"

Prison * init_prison(){
    Prison * jail = (Prison *)calloc(1,sizeof(Prison));
    return jail;
}

void go_to_jail(Player * p, Prison * jail){
    // Sécurité : on vérifie que la prison n'est pas pleine
    if (jail->nb_j < NB_J_MAX) { 
        jail->liste[jail->nb_j] = p;
        jail->comptage[jail->nb_j] = 0;
        jail->nb_j++;
        p->position=10;
        printf("%s va en prison !\n",p->name);
    }
}

void get_out_of_jail(Player *p, Prison * jail){
    for(int i = 0; i < jail->nb_j; i++){
        if(jail->liste[i] == p){
            // On a trouvé le joueur, on décale les suivants vers la gauche
            // On utilise une nouvelle variable 'j' pour ne pas casser la boucle
            for(int j = i; j < jail->nb_j - 1; j++){
                jail->liste[j] = jail->liste[j+1];
                jail->comptage[j] = jail->comptage[j+1];
            }
            
            // On met à jour le nombre de joueurs
            jail->nb_j--;
            
            // On nettoie la dernière case pour éviter les pointeurs fantômes
            jail->liste[jail->nb_j] = NULL;
            jail->comptage[jail->nb_j] = 0;
            printf("%s sort de prison !\n",p->name);
            return; // On sort directement de la fonction
        }
    }
}

int is_in_jail(Player * p, Prison * jail){
    int i;
    for(i=0; i<jail->nb_j;i++){
        if(jail->liste[i] == p){
            return 1;
        }
    }
    return 0;
}

void pay_to_get_out(Player * p, Prison * jail){
    player_remove_credits(p,50);
    get_out_of_jail(p,jail);
}

void add_tour_jail(Player * p, Prison * jail){
    int i;
    for(i=0; i<jail->nb_j;i++){
        if(jail->liste[i] == p){
            (jail->comptage[i])++ ;
            return ;
        }
    }
}

int istreetour_in_jail(Player * p, Prison * jail){
    int i;
    for(i=0 ;i<jail->nb_j;i++){
        if((jail->liste[i] == p) && (jail->comptage[i] >= 3 )){
            printf("Cela fait 3 tour que %s est en prison ! \n",p->name);
            return 1; 
        }
    }
    return 0;
}