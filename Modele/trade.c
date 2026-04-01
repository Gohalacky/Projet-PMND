#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trade.h"

TradeOffer * init_empty_offer(Player *p) {
    // calloc alloue la mémoire et met tout (entiers, pointeurs, tableaux) à 0/NULL
    TradeOffer * tr = (TradeOffer *)calloc(1, sizeof(TradeOffer));
    
    if (tr != NULL) {
        tr->proposant = p; // On a juste besoin de lier le joueur
    }
    
    return tr;
}

int is_trade_valid(TradeOffer *off_p1, TradeOffer *off_p2){
    if ((off_p1 == NULL) || (off_p2 == NULL)) {
        printf("Les 2 joueurs doivent avoir proposé un échange.\n");
        return 0;
    }
    if (off_p1->proposant->credit < off_p1->ects){
        printf("%s n'a pas assez de crédit pour proposer cette offre.\n",off_p1->proposant->name);
        return 0;
    }
    if (off_p2->proposant->credit < off_p2->ects){
        printf("%s n'a pas assez de crédit pour proposer cette offre.\n",off_p2->proposant->name);
        return 0;
    }
    int i;
    for(i=0;i<off_p1->nb_props;i++){
        if(off_p1->props[i]->owner != off_p1->proposant->number){
            printf("Echange impossible, %s ne possède pas %s.\n",off_p1->proposant->name,off_p1->props[i]->name);
            return 0;
        }
        if(off_p1->props[i]->level !=0){
            printf("Echange impossible, il reste des construction sur %s\n",off_p1->props[i]->name);
            return 0;
        }
    }
    for(i=0;i<off_p2->nb_props;i++){
        if(off_p2->props[i]->level !=0){
            printf("Echange impossible, il reste des construction sur %s\n",off_p2->props[i]->name);
            return 0;
        }
        if(off_p2->props[i]->owner != off_p2->proposant->number){
            printf("Echange impossible, %s ne possède pas %s.\n",off_p2->proposant->name,off_p2->props[i]->name);
            return 0;
        }
    }
    for(i=0;i<off_p1->nb_comps;i++){
        if(off_p1->comps[i]->owner != off_p1->proposant->number){
            printf("Echange impossible, %s ne possède pas %s.\n",off_p1->proposant->name,off_p1->comps[i]->name);
            return 0;
        }
    }
    for(i=0;i<off_p2->nb_comps;i++){
        if(off_p2->comps[i]->owner != off_p2->proposant->number){
            printf("Echange impossible, %s ne possède pas %s.\n",off_p2->proposant->name,off_p2->comps[i]->name);
            return 0;
        }
    }
    for(i=0;i<off_p1->nb_gares;i++){
        if(off_p1->gares[i]->owner != off_p1->proposant->number){
            printf("Echange impossible, %s ne possède pas %s.\n",off_p1->proposant->name,off_p1->gares[i]->name);
            return 0;
        }
    }
    for(i=0;i<off_p2->nb_gares;i++){
        if(off_p2->gares[i]->owner != off_p2->proposant->number){
            printf("Echange impossible, %s ne possède pas %s.\n",off_p2->proposant->name,off_p2->gares[i]->name);
            return 0;
        }
    }
    // à faire : vérifier les cartes
    return 1;
}