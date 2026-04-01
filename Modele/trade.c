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