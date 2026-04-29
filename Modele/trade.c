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

int is_trade_valid(TradeOffer *off_p1, TradeOffer *off_p2, Property *tableau_prop[]){
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
    int i,j;

    // VÉRIFICATION PROPRIÉTÉS JOUEUR 1
    for(i=0; i<off_p1->nb_props; i++){
        // Vérification propriétaire
        if(off_p1->props[i]->owner != off_p1->proposant->number){
            printf("Echange impossible, %s ne possède pas %s.\n",off_p1->proposant->name,off_p1->props[i]->name);
            return 0;
        }
        
        // Vérification du GROUPE de couleur (Règle : pas de bâtiments dans tout le quartier)
        for (j = 0; j < NB_PROPRIETE; j++) {
            if (tableau_prop[j]->color == off_p1->props[i]->color && tableau_prop[j]->level > 0) {
                printf("Echange impossible, il reste des constructions dans le quartier de %s.\n", off_p1->props[i]->name);
                return 0;
            }
        }
    }

    // VÉRIFICATION PROPRIÉTÉS JOUEUR 2
    for(i=0; i<off_p2->nb_props; i++){
        // Vérification propriétaire
        if(off_p2->props[i]->owner != off_p2->proposant->number){
            printf("Echange impossible, %s ne possède pas %s.\n",off_p2->proposant->name,off_p2->props[i]->name);
            return 0;
        }

        // Vérification du GROUPE de couleur
        for (j = 0; j < NB_PROPRIETE; j++) {
            if (tableau_prop[j]->color == off_p2->props[i]->color && tableau_prop[j]->level > 0) {
                printf("Echange impossible, il reste des constructions dans le quartier de %s.\n", off_p2->props[i]->name);
                return 0;
            }
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

void trade_set_credits(TradeOffer *offer, int amount) {
    if (offer != NULL) {
        offer->ects = amount;
    }
}

int trade_add_property(TradeOffer *offer, Property *prop) {
    if (offer == NULL || prop == NULL) return 0;
    
    // Sécurité : vérifier que la propriété n'est pas déjà dans l'offre
    for (int i = 0; i < offer->nb_props; i++) {
        if (offer->props[i] == prop){
            printf("%s déja présente dans l'offre.\n",offer->props[i]->name);
            return 0; // Déjà présente !
        }
    }

    // Ajout à la fin du tableau
    offer->props[offer->nb_props] = prop;
    offer->nb_props++;
    return 1;
}

int trade_remove_property(TradeOffer *offer, Property *prop) {
    if (offer == NULL || prop == NULL) return 0;

    for (int i = 0; i < offer->nb_props; i++) {
        if (offer->props[i] == prop) { // On a trouvé la carte à retirer
            
            // On décale toutes les cartes suivantes vers la gauche pour boucher le trou
            for (int j = i; j < offer->nb_props - 1; j++) {
                offer->props[j] = offer->props[j + 1];
            }
            
            // On nettoie la dernière case et on met à jour le compteur
            offer->props[offer->nb_props - 1] = NULL;
            offer->nb_props--;
            return 1; // Succès
        }
    }
    printf("Erreur, %s n'est pas dans l'offre.\n",prop->name);
    return 0; // La carte n'était pas dans l'offre
}

int trade_add_gare(TradeOffer *offer, Gares *gare) {
    if (offer == NULL || gare == NULL) return 0;
    
    for (int i = 0; i < offer->nb_gares; i++) {
        if (offer->gares[i] == gare){
            printf("%s déja présente dans l'offre.\n",offer->gares[i]->name);
            return 0; 
        }
    }

    offer->gares[offer->nb_gares] = gare;
    offer->nb_gares++;
    return 1;
}

int trade_remove_gare(TradeOffer *offer, Gares *gare) {
    if (offer == NULL || gare == NULL) return 0;

    for (int i = 0; i < offer->nb_gares; i++) {
        if (offer->gares[i] == gare) { 
            for (int j = i; j < offer->nb_gares - 1; j++) {
                offer->gares[j] = offer->gares[j + 1];
            }
            offer->gares[offer->nb_gares - 1] = NULL;
            offer->nb_gares--;
            return 1; 
        }
    }
    printf("Erreur, %s n'est as dans l'offre.\n",gare->name);
    return 0; 
}

int trade_add_compagnie(TradeOffer *offer, Compagnies *comp) {
    if (offer == NULL || comp == NULL) return 0;
    
    for (int i = 0; i < offer->nb_comps; i++) {
        if (offer->comps[i] == comp){
            printf("%s déja présente dans l'offre.\n",offer->comps[i]->name);
            return 0; 
        }
    }

    offer->comps[offer->nb_comps] = comp;
    offer->nb_comps++;
    return 1;
}

int trade_remove_compagnie(TradeOffer *offer, Compagnies *comp) {
    if (offer == NULL || comp == NULL) return 0;

    for (int i = 0; i < offer->nb_comps; i++) {
        if (offer->comps[i] == comp) { 
            for (int j = i; j < offer->nb_comps - 1; j++) {
                offer->comps[j] = offer->comps[j + 1];
            }
            offer->comps[offer->nb_comps - 1] = NULL;
            offer->nb_comps--;
            return 1; 
        }
    }
    printf("Erreur, %s n'est as dans l'offre.\n",comp->name);
    return 0; 
}

void execute_trade(TradeOffer *off_p1, TradeOffer *off_p2,Property *tableau_prop[]){
    if(is_trade_valid(off_p1,off_p2,tableau_prop)==0){
        printf("Echange impossible car non valide.\n");
    }
    else{
        int i;
        //ajout des crédits
        player_add_credits(off_p1->proposant,off_p2->ects);
        player_add_credits(off_p2->proposant,off_p1->ects);
        //supression des crédit seulement après
        player_remove_credits(off_p1->proposant,off_p1->ects);
        player_remove_credits(off_p2->proposant,off_p2->ects);

        //on enlève les propriété de l'offre de p1 à p1 pour les donner à p2
        for (i = 0; i < off_p1->nb_props; i++){
            player_remove_property(off_p1->proposant,off_p1->props[i]);
            player_add_property(off_p2->proposant,off_p1->props[i]);
        } 
        //on enlève les propriété de l'offre de p2 à p2 pour les donner à p1
        for (i = 0; i < off_p2->nb_props; i++){
            player_remove_property(off_p2->proposant,off_p2->props[i]);
            player_add_property(off_p1->proposant,off_p2->props[i]);
        }
        //on enlève les compagnie de l'offre de p1 à p1 pour les donner à p2
        for ( i = 0; i < off_p1->nb_comps; i++){
            player_remove_comp(off_p1->proposant,off_p1->comps[i]);
            player_add_comp(off_p2->proposant,off_p1->comps[i]);
        } 
        //on enlève les compagnie de l'offre de p2 à p2 pour les donner à p1
        for ( i = 0; i < off_p2->nb_comps; i++){
            player_remove_comp(off_p2->proposant,off_p2->comps[i]);
            player_add_comp(off_p1->proposant,off_p2->comps[i]);
        }
        //on enlève les gares de l'offre de p1 à p1 pour les donner à p2
        for (i = 0; i < off_p1->nb_gares; i++){
            player_remove_gar(off_p1->proposant,off_p1->gares[i]);
            player_add_gar(off_p2->proposant,off_p1->gares[i]);
        }
        for ( i = 0; i < off_p2->nb_gares; i++){
            player_remove_gar(off_p2->proposant,off_p2->gares[i]);
            player_add_gar(off_p1->proposant,off_p2->gares[i]);
        } 

        printf("L'échange entre %s et %s a été effectué avec succès !\n", off_p1->proposant->name, off_p2->proposant->name);

    }
}
    
