#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "voe.h"


AuctionState* init_auction_prop(Property *prop, Player *p_refus, Player *all_players[], int nb_joueurs_total) {
    AuctionState *auction = (AuctionState *)calloc(1, sizeof(AuctionState));
    if (!auction) return NULL;

    auction->prop_item = prop; // On remplit la propriété
    // Note : gare_item et comp_item sont déjà à NULL grâce au calloc 

    auction->current_bid = 10; // Mise à prix de base
    auction->highest_bidder = NULL;
    
    // Tous les joueurs participent, y compris celui qui a refusé (règles officielles)
    auction->players_left = nb_joueurs_total;

    for (int i = 0; i < nb_joueurs_total; i++) {
        auction->in_auction[i] = all_players[i];
    }

    return auction;
}

AuctionState* init_auction_comp(Compagnies *c, Player *p_refus, Player *all_players[], int nb_joueurs_total) {
    AuctionState *auction = (AuctionState *)calloc(1, sizeof(AuctionState));
    if (!auction) return NULL;

    auction->comp_item = c; // On remplit la compagnie
    // Note : prop_item et gare_item sont déjà à NULL grâce au calloc 

    auction->current_bid = 10; // Mise à prix de base
    auction->highest_bidder = NULL;
    
    // Tous les joueurs participent, y compris celui qui a refusé (règles officielles)
    auction->players_left = nb_joueurs_total;

    for (int i = 0; i < nb_joueurs_total; i++) {
        auction->in_auction[i] = all_players[i];
    }

    return auction;
}

AuctionState* init_auction_gar(Gares *g, Player *p_refus, Player *all_players[], int nb_joueurs_total) {
    AuctionState *auction = (AuctionState *)calloc(1, sizeof(AuctionState));
    if (!auction) return NULL;

    auction->gar_item = g; // On remplit la gare
    // Note : prop_item et comp_item sont déjà à NULL grâce au calloc 

    auction->current_bid = 10; // Mise à prix de base
    auction->highest_bidder = NULL;
    
    // Tous les joueurs participent, y compris celui qui a refusé (règles officielles)
    auction->players_left = nb_joueurs_total;

    for (int i = 0; i < nb_joueurs_total; i++) {
        auction->in_auction[i] = all_players[i];
    }

    return auction;
}

// Fonction utilitaire : vérifie si le joueur est encore dans le tableau
static int is_player_in_auction(AuctionState *auction, Player *p) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (auction->in_auction[i] == p) return 1;
    }
    return 0;
}

int process_bid(AuctionState *auction, Player *p, int amount) {
    if (auction == NULL || p == NULL) return 0;
    
    if (!is_player_in_auction(auction, p)) {
        printf("Refusé : %s ne participe plus à cette enchère.\n", p->name);
        return 0;
    }

    if (amount <= auction->current_bid) {
        printf("Refusé : l'offre doit être supérieure à %d ECTS.\n", auction->current_bid);
        return 0;
    }

    if (p->credit < amount) {
        printf("Refusé : %s n'a pas assez d'argent.\n", p->name);
        return 0;
    }

    auction->current_bid = amount;
    auction->highest_bidder = p;
    printf(">>> %s prend la tête avec %d ECTS !\n", p->name, amount);
    
    return 1;
}

void fold_player(AuctionState *auction, Player *p) {
    if (auction == NULL || p == NULL) return;

    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (auction->in_auction[i] == p) {
            auction->in_auction[i] = NULL; // On le retire
            auction->players_left--;
            printf("%s se couche.\n", p->name);
            break;
        }
    }
}

void finalize_auction(AuctionState *auction) {
    if (auction == NULL) return;

    if (auction->highest_bidder != NULL) {
        Player *gagnant = auction->highest_bidder;
        int prix = auction->current_bid;

        // Le gagnant paie
        player_remove_credits(gagnant, prix);

        // On cherche ce qu'il a gagné
        if (auction->prop_item != NULL) {
            player_add_property(gagnant, auction->prop_item);
            printf("Adjugé ! %s remporte la propriété %s pour %d ECTS !\n", gagnant->name, auction->prop_item->name, prix);
        } 
        else if (auction->gar_item != NULL) {
            player_add_gar(gagnant, auction->gar_item);
            printf("Adjugé ! %s remporte la %s pour %d ECTS !\n", gagnant->name, auction->gar_item->name, prix);
        } 
        else if (auction->comp_item != NULL) {
            player_add_comp(gagnant, auction->comp_item);
            printf("Adjugé ! %s remporte la compagnie %s pour %d ECTS !\n", gagnant->name, auction->comp_item->name, prix);
        }
    } else {
        printf("Personne n'a enchéri. Le bien reste à la banque.\n");
    }
}